#include "Game.hpp"
#include <cstdlib>
#include <cstdio>

extern "C" {
#include "chadwick.h"
}

Game::Game(const char* gameId, const char* date) {
    game = cw_game_create(const_cast<char*>(gameId));
    if (game) {
        cw_game_info_append(game, const_cast<char*>("date"), const_cast<char*>(date));
        iter = cw_gameiter_create(game);
    } else {
        iter = nullptr;
    }
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

void Game::AddInfo(const char* key, const char* value) {
    cw_game_info_append(game, const_cast<char*>(key), const_cast<char*>(value));
}

void Game::AddStarter(const StarterInfo& starter) {
    cw_game_starter_append(game, 
                           const_cast<char*>(starter.id.c_str()), 
                           const_cast<char*>(starter.name.c_str()), 
                           starter.isHome, starter.battingOrder, starter.position);
}

void Game::Write(FILE* file) {
    cw_game_write(game, file);
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

void Game::AddComment(const char* comment) {
    cw_game_comment_append(game, const_cast<char*>(comment));
}

GameState Game::GetGameState() const {
    return GameState(iter ? iter->state : nullptr);
}
