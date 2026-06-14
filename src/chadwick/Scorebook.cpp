#include "chadwick/Scorebook.hpp"
#include "chadwick/Game.hpp"
#include <cstdio>
#include <filesystem>

extern "C" {
// clang-format off
#include "parse.h"
#include "game.h"
// clang-format on
#include "book.h"
#include "gameiter.h"
}

namespace chadwick {

Scorebook::Scorebook() : scorebook(cw_scorebook_create()) {}

Scorebook::~Scorebook() {
    if (scorebook != nullptr) {
        cw_scorebook_cleanup(scorebook);
        free(scorebook);
    }
}

Scorebook::Scorebook(Scorebook&& other) noexcept : scorebook(other.scorebook) { other.scorebook = nullptr; }

auto Scorebook::operator=(Scorebook&& other) noexcept -> Scorebook& {
    if (this != &other) {
        if (scorebook != nullptr) {
            cw_scorebook_cleanup(scorebook);
            free(scorebook);
        }
        scorebook = other.scorebook;
        other.scorebook = nullptr;
    }
    return *this;
}

void Scorebook::AddGame(Game&& game) {
    if (game.game != nullptr) {
        // Transfer ownership of CWGame to the scorebook
        cw_scorebook_append_game(scorebook, game.game);
        game.game = nullptr;

        // Clean up the iterator, as Scorebook only manages CWGame
        if (game.iter != nullptr) {
            cw_gameiter_cleanup(game.iter);
            free(game.iter);
            game.iter = nullptr;
        }
    }
}

auto Scorebook::Read(const std::filesystem::path& path) const -> int {
    FILE* file = fopen(path.string().c_str(), "r");
    if (file == nullptr) {
        return -1;
    }
    int const gamesRead = cw_scorebook_read(scorebook, file);
    fclose(file);
    return gamesRead;
}

auto Scorebook::Write(const std::filesystem::path& path) const -> bool {
    FILE* file = fopen(path.string().c_str(), "w");
    if (file == nullptr) {
        return false;
    }
    cw_scorebook_write(scorebook, file);
    fclose(file);
    return true;
}

} // namespace chadwick
