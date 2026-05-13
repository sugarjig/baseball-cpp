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
    }
}

Game::~Game() {
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
