#include "Game.hpp"
#include <cstdlib>
#include <cstdio>

extern "C" {
#include "chadwick.h"
}

Game::Game(std::string_view gameId, std::string_view date) {
    game = cw_game_create(const_cast<char*>(std::string(gameId).c_str()));
    if (game) {
        cw_game_info_append(game, const_cast<char*>("date"), const_cast<char*>(std::string(date).c_str()));
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

void Game::AddInfo(std::string_view key, std::string_view value) {
    cw_game_info_append(game, const_cast<char*>(std::string(key).c_str()), const_cast<char*>(std::string(value).c_str()));
}

void Game::AddStarter(const StarterInfo& starter) {
    cw_game_starter_append(game, 
                           const_cast<char*>(starter.id.c_str()), 
                           const_cast<char*>(starter.name.c_str()), 
                           starter.isHome, starter.battingOrder, starter.position);
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
            cw_gameiter_next(iter);
        }
    }
}

void Game::AddEvent(const PlayInfo& play) {
    cw_game_event_append(game, play.inning, play.team,
                         const_cast<char*>(play.batter.c_str()),
                         const_cast<char*>(play.pitchCount.c_str()),
                         const_cast<char*>(play.pitchSequence.c_str()),
                         const_cast<char*>(play.text.c_str()));
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

const GameState& Game::GetGameState() const {
    return gameState;
}
