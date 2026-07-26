#include "chadwick/Game.hpp"
#include "EventSource.hpp"
#include "IGameState.hpp"
#include "Records.hpp"
#include "chadwick/GameIterator.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string_view>
#include <utility>
#include <vector>
extern "C" {
// clang-format off
// ReSharper disable once CppUnusedIncludeDirective
#include "parse.h" // NOLINT(misc-include-cleaner)
// clang-format on
#include "game.h"
}

namespace chadwick {

Game::Game(const std::string_view gameId, // NOLINT(bugprone-easily-swappable-parameters)
           const std::string_view version, const std::vector<InfoRecord>& infoRecords,
           const std::vector<StarterRecord>& starters)
    : Game(InitializeCWGame(gameId, version, infoRecords, starters)) {}

CWGame* Game::InitializeCWGame(const std::string_view gameId, const std::string_view version,
                               const std::vector<InfoRecord>& infoRecords, const std::vector<StarterRecord>& starters) {
    CWGame* cw = cw_game_create(std::string(gameId).data());
    if (cw != nullptr) {
        cw_game_set_version(cw, std::string(version).data());
        for (const auto& info : infoRecords) {
            cw_game_info_append(cw, std::string(info.key).data(), std::string(info.value).data());
        }
        for (const auto& starter : starters) {
            cw_game_starter_append(cw, std::string(starter.id).data(), std::string(starter.name).data(),
                                   static_cast<int>(starter.isHome), starter.battingOrder, starter.position);
        }
    }
    return cw;
}

Game::Game(CWGame* cw) : cwGame(cw), iterator(cw) {}

Game::~Game() {
    if (cwGame != nullptr) {
        cw_game_cleanup(cwGame);
        free(cwGame); // NOLINT(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory)
    }
}

Game::Game(Game&& other) noexcept
    : cwGame(other.cwGame), iterator(std::move(other.iterator)), pendingAutoRunner(std::move(other.pendingAutoRunner)),
      pendingAutoBase(other.pendingAutoBase),
      pendingBatterAdjustmentPlayerId(std::move(other.pendingBatterAdjustmentPlayerId)),
      pendingBatterAdjustmentHand(other.pendingBatterAdjustmentHand),
      pendingPitcherAdjustmentPlayerId(std::move(other.pendingPitcherAdjustmentPlayerId)),
      pendingPitcherAdjustmentHand(other.pendingPitcherAdjustmentHand) {
    other.cwGame = nullptr;
    other.pendingAutoBase = 0;
    other.pendingBatterAdjustmentHand = ' ';
    other.pendingPitcherAdjustmentHand = ' ';
}

auto Game::operator=(Game&& other) noexcept -> Game& {
    if (this != &other) {
        if (cwGame != nullptr) {
            cw_game_cleanup(cwGame);
            free(cwGame); // NOLINT(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory)
        }
        cwGame = other.cwGame;
        iterator = std::move(other.iterator);
        pendingAutoRunner = std::move(other.pendingAutoRunner);
        pendingAutoBase = other.pendingAutoBase;
        pendingBatterAdjustmentPlayerId = std::move(other.pendingBatterAdjustmentPlayerId);
        pendingBatterAdjustmentHand = other.pendingBatterAdjustmentHand;
        pendingPitcherAdjustmentPlayerId = std::move(other.pendingPitcherAdjustmentPlayerId);
        pendingPitcherAdjustmentHand = other.pendingPitcherAdjustmentHand;
        other.cwGame = nullptr;
        other.pendingAutoBase = 0;
        other.pendingBatterAdjustmentHand = ' ';
        other.pendingPitcherAdjustmentHand = ' ';
    }
    return *this;
}

auto Game::Write(const std::filesystem::path& path) const -> bool {
    FILE* file = fopen(path.string().c_str(), "w");
    if (file == nullptr) {
        return false;
    }
    cw_game_write(cwGame, file);
    fclose(file); // NOLINT(cppcoreguidelines-owning-memory)
    return true;
}

void Game::AddPlay(const PlayInfo& play) {
    cw_game_event_append(cwGame, play.inning, play.team, std::string(play.batter).data(),
                         std::string(play.pitchCount).data(), std::string(play.pitchSequence).data(),
                         std::string(play.text).data());

    if (cwGame->last_event != nullptr) {
        if (pendingAutoBase != 0) {
            cwGame->last_event->auto_base = pendingAutoBase;
            cwGame->last_event->auto_runner_id =                              // NOLINT(cppcoreguidelines-owning-memory)
                static_cast<char*>(malloc(pendingAutoRunner.length() + 1)); // NOLINT(cppcoreguidelines-no-malloc)
            strcpy(cwGame->last_event->auto_runner_id, pendingAutoRunner.c_str());

            pendingAutoBase = 0;
            pendingAutoRunner.clear();
        }

        if (pendingBatterAdjustmentHand != ' ') {
            if (pendingBatterAdjustmentPlayerId == play.batter) {
                cwGame->last_event->batter_hand = pendingBatterAdjustmentHand;
            }
            pendingBatterAdjustmentHand = ' ';
            pendingBatterAdjustmentPlayerId.clear();
        }

        if (pendingPitcherAdjustmentHand != ' ') {
            cwGame->last_event->pitcher_hand = pendingPitcherAdjustmentHand;
            cwGame->last_event->pitcher_hand_id = // NOLINT(cppcoreguidelines-owning-memory)
                static_cast<char*>(
                    malloc(pendingPitcherAdjustmentPlayerId.length() + 1)); // NOLINT(cppcoreguidelines-no-malloc)
            strcpy(cwGame->last_event->pitcher_hand_id, pendingPitcherAdjustmentPlayerId.c_str());

            pendingPitcherAdjustmentHand = ' ';
            pendingPitcherAdjustmentPlayerId.clear();
        }
    }
}

void Game::AddSubstitution(const SubstitutionInfo& sub) const {
    cw_game_substitute_append(cwGame, std::string(sub.playerId).data(), std::string(sub.name).data(), sub.team, sub.slot,
                              sub.pos);
}

void Game::AddComment(std::string_view comment) const { cw_game_comment_append(cwGame, std::string(comment).data()); }

void Game::AddData(const DataRecord& data) {
    std::vector<std::string> copies(data.fields.begin(), data.fields.end());
    std::vector<char*> cFields;
    cFields.reserve(copies.size());
    for (auto& field : copies) {
        cFields.push_back(field.data());
    }
    cw_game_data_append(cwGame, static_cast<int>(cFields.size()), cFields.data());
}

void Game::AddRunnerAdjustment(const RunnerAdjustmentInfo& radj) {
    pendingAutoRunner = radj.playerId;
    pendingAutoBase = radj.base;
}

void Game::AddBatterAdjustment(const BatterAdjustmentInfo& badj) {
    pendingBatterAdjustmentPlayerId = badj.playerId;
    pendingBatterAdjustmentHand = badj.hand;
}

void Game::AddPitcherAdjustment(const PitcherAdjustmentInfo& padj) {
    pendingPitcherAdjustmentPlayerId = padj.playerId;
    pendingPitcherAdjustmentHand = padj.hand;
}

auto Game::GetGameState() const -> const IGameState& { return iterator.GetGameState(); }

void Game::AddEvent(const Event& event) {
    switch (event.type) {
    case EventType::Play:
        AddPlay(std::get<PlayInfo>(event.data));
        break;
    case EventType::Substitution:
        AddSubstitution(std::get<SubstitutionInfo>(event.data));
        break;
    case EventType::Comment:
        AddComment(std::get<std::string>(event.data));
        break;
    case EventType::RunnerAdjustment:
        AddRunnerAdjustment(std::get<RunnerAdjustmentInfo>(event.data));
        break;
    case EventType::BatterAdjustment:
        AddBatterAdjustment(std::get<BatterAdjustmentInfo>(event.data));
        break;
    case EventType::PitcherAdjustment:
        AddPitcherAdjustment(std::get<PitcherAdjustmentInfo>(event.data));
        break;
    default:
        break;
    }
    iterator.UpdateState();
}

} // namespace chadwick
