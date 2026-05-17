#include "chadwick/Scorebook.hpp"
#include <cstdlib>
#include <cstdio>

extern "C" {
#include "chadwick.h"
}

namespace chadwick {

Scorebook::Scorebook() {
    scorebook = cw_scorebook_create();
}

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

bool Scorebook::Write(const std::filesystem::path& path) {
    FILE* file = fopen(path.string().c_str(), "w");
    if (!file) return false;
    cw_scorebook_write(scorebook, file);
    fclose(file);
    return true;
}

} // namespace chadwick
