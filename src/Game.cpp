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

void Game::AddStarter(const char* id, const char* name, int isHome, int battingOrder, int position) {
    cw_game_starter_append(game, const_cast<char*>(id), const_cast<char*>(name), isHome, battingOrder, position);
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

void Game::AddEvent(int inning, int team, const char* batter, const char* pitchCount, const char* pitchSequence, const char* text) {
    cw_game_event_append(game, inning, team,
                         const_cast<char*>(batter),
                         const_cast<char*>(pitchCount),
                         const_cast<char*>(pitchSequence),
                         const_cast<char*>(text));
}

void Game::AddSubstitution(const char* playerID, const char* name, int team, int slot, int pos) {
    cw_game_substitute_append(game, const_cast<char*>(playerID), const_cast<char*>(name), team, slot, pos);
}

void Game::AddComment(const char* comment) {
    cw_game_comment_append(game, const_cast<char*>(comment));
}

CWGameState* Game::GetState() const {
    return iter ? iter->state : nullptr;
}
