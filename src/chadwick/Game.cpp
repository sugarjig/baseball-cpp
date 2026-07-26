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
    : game(cw_game_create(std::string(gameId).data())), iterator(nullptr) {
    if (game != nullptr) {
        cw_game_set_version(game, std::string(version).data());
        for (const auto& info : infoRecords) {
            cw_game_info_append(game, std::string(info.key).data(), std::string(info.value).data());
        }
        for (const auto& starter : starters) {
            cw_game_starter_append(game, std::string(starter.id).data(), std::string(starter.name).data(),
                                   static_cast<int>(starter.isHome), starter.battingOrder, starter.position);
        }
    }
    iterator = GameIterator(game);
}

Game::~Game() {
    if (game != nullptr) {
        cw_game_cleanup(game);
        free(game); // NOLINT(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory)
    }
}

Game::Game(Game&& other) noexcept
    : game(other.game), iterator(std::move(other.iterator)), pendingAutoRunner(std::move(other.pendingAutoRunner)),
      pendingAutoBase(other.pendingAutoBase),
      pendingBatterAdjustmentPlayerId(std::move(other.pendingBatterAdjustmentPlayerId)),
      pendingBatterAdjustmentHand(other.pendingBatterAdjustmentHand),
      pendingPitcherAdjustmentPlayerId(std::move(other.pendingPitcherAdjustmentPlayerId)),
      pendingPitcherAdjustmentHand(other.pendingPitcherAdjustmentHand) {
    other.game = nullptr;
    other.pendingAutoBase = 0;
    other.pendingBatterAdjustmentHand = ' ';
    other.pendingPitcherAdjustmentHand = ' ';
}

auto Game::operator=(Game&& other) noexcept -> Game& {
    if (this != &other) {
        if (game != nullptr) {
            cw_game_cleanup(game);
            free(game); // NOLINT(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory)
        }
        game = other.game;
        iterator = std::move(other.iterator);
        pendingAutoRunner = std::move(other.pendingAutoRunner);
        pendingAutoBase = other.pendingAutoBase;
        pendingBatterAdjustmentPlayerId = std::move(other.pendingBatterAdjustmentPlayerId);
        pendingBatterAdjustmentHand = other.pendingBatterAdjustmentHand;
        pendingPitcherAdjustmentPlayerId = std::move(other.pendingPitcherAdjustmentPlayerId);
        pendingPitcherAdjustmentHand = other.pendingPitcherAdjustmentHand;
        other.game = nullptr;
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
    cw_game_write(game, file);
    fclose(file); // NOLINT(cppcoreguidelines-owning-memory)
    return true;
}

/**
 * Updates the game state by resetting the iterator and re-processing all events from the beginning.
 *
 * This implementation is necessary because Chadwick's internal state management relies on
 * sequential iteration. However, it means this operation is O(N) where N is the number of
 * events currently in the game.
 *
 * When called after every event (as in Simulator::SimulateGame), the total time complexity
 * for simulating a game with N events becomes O(N^2).
 */
void Game::UpdateState() { iterator.UpdateState(); }

void Game::AddPlay(const PlayInfo& play) {
    cw_game_event_append(game, play.inning, play.team, std::string(play.batter).data(),
                         std::string(play.pitchCount).data(), std::string(play.pitchSequence).data(),
                         std::string(play.text).data());

    if (game->last_event != nullptr) {
        if (pendingAutoBase != 0) {
            game->last_event->auto_base = pendingAutoBase;
            game->last_event->auto_runner_id =                              // NOLINT(cppcoreguidelines-owning-memory)
                static_cast<char*>(malloc(pendingAutoRunner.length() + 1)); // NOLINT(cppcoreguidelines-no-malloc)
            strcpy(game->last_event->auto_runner_id, pendingAutoRunner.c_str());

            pendingAutoBase = 0;
            pendingAutoRunner.clear();
        }

        if (pendingBatterAdjustmentHand != ' ') {
            if (pendingBatterAdjustmentPlayerId == play.batter) {
                game->last_event->batter_hand = pendingBatterAdjustmentHand;
            }
            pendingBatterAdjustmentHand = ' ';
            pendingBatterAdjustmentPlayerId.clear();
        }

        if (pendingPitcherAdjustmentHand != ' ') {
            game->last_event->pitcher_hand = pendingPitcherAdjustmentHand;
            game->last_event->pitcher_hand_id = // NOLINT(cppcoreguidelines-owning-memory)
                static_cast<char*>(
                    malloc(pendingPitcherAdjustmentPlayerId.length() + 1)); // NOLINT(cppcoreguidelines-no-malloc)
            strcpy(game->last_event->pitcher_hand_id, pendingPitcherAdjustmentPlayerId.c_str());

            pendingPitcherAdjustmentHand = ' ';
            pendingPitcherAdjustmentPlayerId.clear();
        }
    }
}

void Game::AddSubstitution(const SubstitutionInfo& sub) const {
    cw_game_substitute_append(game, std::string(sub.playerId).data(), std::string(sub.name).data(), sub.team, sub.slot,
                              sub.pos);
}

void Game::AddComment(std::string_view comment) const { cw_game_comment_append(game, std::string(comment).data()); }

void Game::AddData(const DataRecord& data) {
    std::vector<std::string> copies(data.fields.begin(), data.fields.end());
    std::vector<char*> cFields;
    cFields.reserve(copies.size());
    for (auto& field : copies) {
        cFields.push_back(field.data());
    }
    cw_game_data_append(game, static_cast<int>(cFields.size()), cFields.data());
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
    UpdateState();
}

} // namespace chadwick
