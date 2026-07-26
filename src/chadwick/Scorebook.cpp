#include "chadwick/Scorebook.hpp"
#include "chadwick/Game.hpp"
#include <cstdio>
#include <cstdlib>
#include <filesystem>

extern "C" {
// clang-format off
// ReSharper disable once CppUnusedIncludeDirective
#include "parse.h" // NOLINT(misc-include-cleaner)
#include "game.h" // NOLINT(misc-include-cleaner)
// clang-format on
#include "book.h"
}

namespace chadwick {

Scorebook::Scorebook() : cwScorebook(cw_scorebook_create()) {}

Scorebook::~Scorebook() {
    if (cwScorebook != nullptr) {
        cw_scorebook_cleanup(cwScorebook);
        free(cwScorebook); // NOLINT(cppcoreguidelines-owning-memory,cppcoreguidelines-no-malloc)
    }
}

Scorebook::Scorebook(Scorebook&& other) noexcept : cwScorebook(other.cwScorebook) { other.cwScorebook = nullptr; }

auto Scorebook::operator=(Scorebook&& other) noexcept -> Scorebook& {
    if (this != &other) {
        if (cwScorebook != nullptr) {
            cw_scorebook_cleanup(cwScorebook);
            free(cwScorebook); // NOLINT(cppcoreguidelines-owning-memory,cppcoreguidelines-no-malloc)
        }
        cwScorebook = other.cwScorebook;
        other.cwScorebook = nullptr;
    }
    return *this;
}

void Scorebook::AddGame(Game& game) const {
    if (CWGame* cwGame = game.ReleaseCWGame(); cwGame != nullptr) {
        // Transfer ownership of CWGame to the scorebook
        cw_scorebook_append_game(cwScorebook, cwGame);
    }
}

auto Scorebook::Read(const std::filesystem::path& path) const -> int {
    FILE* file = fopen(path.string().c_str(), "r");
    if (file == nullptr) {
        return -1;
    }
    int const gamesRead = cw_scorebook_read(cwScorebook, file);
    fclose(file); // NOLINT(cppcoreguidelines-owning-memory)
    return gamesRead;
}

auto Scorebook::Write(const std::filesystem::path& path) const -> bool {
    FILE* file = fopen(path.string().c_str(), "w");
    if (file == nullptr) {
        return false;
    }
    cw_scorebook_write(cwScorebook, file);
    fclose(file); // NOLINT(cppcoreguidelines-owning-memory)
    return true;
}

} // namespace chadwick
