#include "Game.hpp"
#include <cstdlib>
#include <cstdio>

extern "C" {
#include "chadwick.h"
}

Game::Game(const char* gameId) {
    game = cw_game_create(const_cast<char*>(gameId));
}

Game::~Game() {
    if (game) {
        cw_game_cleanup(game);
        free(game);
    }
}
