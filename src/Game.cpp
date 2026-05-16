#include "Game.hpp"
#include <cstdlib>
#include <cstdio>
#include <cstring>

extern "C" {
#include "chadwick.h"
}

Game::Game(std::string_view gameId, std::string_view version, 
           const std::vector<InfoRecord>& infoRecords,
           const std::vector<StarterInfo>& starters) {
    game = cw_game_create(const_cast<char*>(std::string(gameId).c_str()));
    if (game) {
        cw_game_set_version(game, const_cast<char*>(std::string(version).c_str()));
        for (const auto& info : infoRecords) {
            cw_game_info_append(game, const_cast<char*>(std::string(info.key).c_str()), const_cast<char*>(std::string(info.value).c_str()));
        }
        for (const auto& starter : starters) {
            cw_game_starter_append(game, 
                                   const_cast<char*>(starter.id.c_str()), 
                                   const_cast<char*>(starter.name.c_str()), 
                                   starter.isHome, starter.battingOrder, starter.position);
        }
        iter = cw_gameiter_create(game);
    } else {
        iter = nullptr;
    }
    gameState.state = iter ? iter->state : nullptr;
}

Game::~Game() {
    if (iter) {
        cw_gameiter_cleanup(iter);
        free(iter);
    }
    if (game) {
        cw_game_cleanup(game);
        free(game);
    }
}

Game::Game(Game&& other) noexcept 
    : game(other.game), iter(other.iter), gameState(other.gameState), 
      pendingAutoRunner(std::move(other.pendingAutoRunner)), pendingAutoBase(other.pendingAutoBase),
      pendingBatterAdjustmentPlayerID(std::move(other.pendingBatterAdjustmentPlayerID)),
      pendingBatterAdjustmentHand(other.pendingBatterAdjustmentHand),
      pendingPitcherAdjustmentPlayerID(std::move(other.pendingPitcherAdjustmentPlayerID)),
      pendingPitcherAdjustmentHand(other.pendingPitcherAdjustmentHand) {
    other.game = nullptr;
    other.iter = nullptr;
    other.gameState.state = nullptr;
    other.pendingAutoBase = 0;
    other.pendingBatterAdjustmentHand = ' ';
    other.pendingPitcherAdjustmentHand = ' ';
}

Game& Game::operator=(Game&& other) noexcept {
    if (this != &other) {
        if (iter) {
            cw_gameiter_cleanup(iter);
            free(iter);
        }
        if (game) {
            cw_game_cleanup(game);
            free(game);
        }
        game = other.game;
        iter = other.iter;
        gameState = other.gameState;
        pendingAutoRunner = std::move(other.pendingAutoRunner);
        pendingAutoBase = other.pendingAutoBase;
        pendingBatterAdjustmentPlayerID = std::move(other.pendingBatterAdjustmentPlayerID);
        pendingBatterAdjustmentHand = other.pendingBatterAdjustmentHand;
        pendingPitcherAdjustmentPlayerID = std::move(other.pendingPitcherAdjustmentPlayerID);
        pendingPitcherAdjustmentHand = other.pendingPitcherAdjustmentHand;
        other.game = nullptr;
        other.iter = nullptr;
        other.gameState.state = nullptr;
        other.pendingAutoBase = 0;
        other.pendingBatterAdjustmentHand = ' ';
        other.pendingPitcherAdjustmentHand = ' ';
    }
    return *this;
}

bool Game::Write(const std::filesystem::path& path) {
    FILE* file = fopen(path.string().c_str(), "w");
    if (!file) return false;
    cw_game_write(game, file);
    fclose(file);
    return true;
}

void Game::UpdateState() {
    if (iter) {
        cw_gameiter_reset(iter);
        while (iter->event != nullptr) {
            CWEvent* currentEvent = iter->event;

            // Save "suspended" comments that Chadwick's cw_gameiter_process_comments might mangle with strtok
            // May be able to remove in versions of Chadwick higher than 0.10.0
            struct SavedComment {
                CWComment* comment;
                std::string originalText;
            };
            std::vector<SavedComment> saved;

            for (CWComment* c = currentEvent->first_comment; c != nullptr; c = c->next) {
                if (c->text && strncmp(c->text, "suspended,", 10) == 0) {
                    saved.push_back({c, c->text});
                }
            }

            cw_gameiter_next(iter);

            // Restore any mangled comments
            for (auto& s : saved) {
                strcpy(s.comment->text, s.originalText.c_str());
            }
        }
    }
}

void Game::AddEvent(const PlayInfo& play) {
    cw_game_event_append(game, play.inning, play.team,
                         const_cast<char*>(play.batter.c_str()),
                         const_cast<char*>(play.pitchCount.c_str()),
                         const_cast<char*>(play.pitchSequence.c_str()),
                         const_cast<char*>(play.text.c_str()));

    if (game->last_event) {
        if (pendingAutoBase != 0) {
            game->last_event->auto_base = pendingAutoBase;
            game->last_event->auto_runner_id = static_cast<char*>(malloc(pendingAutoRunner.length() + 1));
            strcpy(game->last_event->auto_runner_id, pendingAutoRunner.c_str());

            pendingAutoBase = 0;
            pendingAutoRunner.clear();
        }

        if (pendingBatterAdjustmentHand != ' ') {
            if (pendingBatterAdjustmentPlayerID == play.batter) {
                game->last_event->batter_hand = pendingBatterAdjustmentHand;
            }
            pendingBatterAdjustmentHand = ' ';
            pendingBatterAdjustmentPlayerID.clear();
        }

        if (pendingPitcherAdjustmentHand != ' ') {
            game->last_event->pitcher_hand = pendingPitcherAdjustmentHand;
            game->last_event->pitcher_hand_id = static_cast<char*>(malloc(pendingPitcherAdjustmentPlayerID.length() + 1));
            strcpy(game->last_event->pitcher_hand_id, pendingPitcherAdjustmentPlayerID.c_str());

            pendingPitcherAdjustmentHand = ' ';
            pendingPitcherAdjustmentPlayerID.clear();
        }
    }
}

void Game::AddSubstitution(const SubstitutionInfo& sub) {
    cw_game_substitute_append(game, 
                              const_cast<char*>(sub.playerID.c_str()), 
                              const_cast<char*>(sub.name.c_str()), 
                              sub.team, sub.slot, sub.pos);
}

void Game::AddComment(std::string_view comment) {
    cw_game_comment_append(game, const_cast<char*>(std::string(comment).c_str()));
}

void Game::AddData(const DataRecord& data) {
    std::vector<char*> c_fields;
    c_fields.reserve(data.fields.size());
    for (const auto& field : data.fields) {
        c_fields.push_back(const_cast<char*>(field.c_str()));
    }
    cw_game_data_append(game, static_cast<int>(c_fields.size()), c_fields.data());
}

void Game::AddRunnerAdjustment(const RunnerAdjustmentInfo& radj) {
    pendingAutoRunner = radj.playerID;
    pendingAutoBase = radj.base;
}

void Game::AddBatterAdjustment(const BatterAdjustmentInfo& badj) {
    pendingBatterAdjustmentPlayerID = badj.playerID;
    pendingBatterAdjustmentHand = badj.hand;
}

void Game::AddPitcherAdjustment(const PitcherAdjustmentInfo& padj) {
    pendingPitcherAdjustmentPlayerID = padj.playerID;
    pendingPitcherAdjustmentHand = padj.hand;
}

const GameState& Game::GetGameState() const {
    return gameState;
}
