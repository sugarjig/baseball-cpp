#include "chadwick/Scorebook.hpp"
#include "chadwick/Game.hpp"
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <utility>

extern "C" {
// clang-format off
// ReSharper disable once CppUnusedIncludeDirective
#include "parse.h" // NOLINT(misc-include-cleaner)
#include "game.h" // NOLINT(misc-include-cleaner)
// clang-format on
#include "book.h"
#include "gameiter.h"
}

namespace chadwick {

Scorebook::Scorebook() : scorebook(cw_scorebook_create()) {}

Scorebook::~Scorebook() {
    if (scorebook != nullptr) {
        cw_scorebook_cleanup(scorebook);
        free(scorebook); // NOLINT(cppcoreguidelines-owning-memory,cppcoreguidelines-no-malloc)
    }
}

Scorebook::Scorebook(Scorebook&& other) noexcept : scorebook(other.scorebook) { other.scorebook = nullptr; }

auto Scorebook::operator=(Scorebook&& other) noexcept -> Scorebook& {
    if (this != &other) {
        if (scorebook != nullptr) {
            cw_scorebook_cleanup(scorebook);
            free(scorebook); // NOLINT(cppcoreguidelines-owning-memory,cppcoreguidelines-no-malloc)
        }
        scorebook = other.scorebook;
        other.scorebook = nullptr;
    }
    return *this;
}

void Scorebook::AddGame(Game&& game) const {
    if (game.game != nullptr) {
        Game target = std::move(game);
        // Transfer ownership of CWGame to the scorebook
        cw_scorebook_append_game(scorebook, target.game);
        target.game = nullptr;

        // Clean up the iterator, as Scorebook only manages CWGame
        if (target.iter != nullptr) {
            cw_gameiter_cleanup(target.iter);
            free(target.iter); // NOLINT(cppcoreguidelines-owning-memory,cppcoreguidelines-no-malloc)
            target.iter = nullptr;
        }
    }
}

auto Scorebook::Read(const std::filesystem::path& path) const -> int {
    FILE* file = fopen(path.string().c_str(), "r");
    if (file == nullptr) {
        return -1;
    }
    int const gamesRead = cw_scorebook_read(scorebook, file);
    fclose(file); // NOLINT(cppcoreguidelines-owning-memory)
    return gamesRead;
}

auto Scorebook::Write(const std::filesystem::path& path) const -> bool {
    FILE* file = fopen(path.string().c_str(), "w");
    if (file == nullptr) {
        return false;
    }
    cw_scorebook_write(scorebook, file);
    fclose(file); // NOLINT(cppcoreguidelines-owning-memory)
    return true;
}

} // namespace chadwick
