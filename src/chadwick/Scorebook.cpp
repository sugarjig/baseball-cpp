#include "chadwick/Scorebook.hpp"
#include <cstdio>
#include <cstdlib>

extern "C" {
#include "chadwick.h"
}

namespace chadwick {

Scorebook::Scorebook() { scorebook = cw_scorebook_create(); }

Scorebook::~Scorebook() {
    if (scorebook) {
        cw_scorebook_cleanup(scorebook);
        free(scorebook);
    }
}

void Scorebook::AddGame(Game&& game) {
    if (game.game) {
        // Transfer ownership of CWGame to the scorebook
        cw_scorebook_append_game(scorebook, game.game);
        game.game = nullptr;

        // Clean up the iterator, as Scorebook only manages CWGame
        if (game.iter) {
            cw_gameiter_cleanup(game.iter);
            free(game.iter);
            game.iter = nullptr;
        }
    }
}

int Scorebook::Read(const std::filesystem::path& path) {
    FILE* file = fopen(path.string().c_str(), "r");
    if (!file) {
        return -1;
    }
    int gamesRead = cw_scorebook_read(scorebook, file);
    fclose(file);
    return gamesRead;
}

bool Scorebook::Write(const std::filesystem::path& path) {
    FILE* file = fopen(path.string().c_str(), "w");
    if (!file) {
        return false;
    }
    cw_scorebook_write(scorebook, file);
    fclose(file);
    return true;
}

} // namespace chadwick
