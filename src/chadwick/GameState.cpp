#include "chadwick/GameState.hpp"
// ReSharper disable once CppUnusedIncludeDirective
#include <cstdio> // NOLINT(misc-include-cleaner)
#include <string>
extern "C" {
// clang-format off
// ReSharper disable once CppUnusedIncludeDirective
#include "parse.h" // NOLINT(misc-include-cleaner)
// clang-format on
#include "gameiter.h"
}

namespace chadwick {

GameState::GameState(CWGameState* state) : state(state) {}

GameState::GameState(GameState&& other) noexcept : state(other.state) { other.state = nullptr; }

auto GameState::operator=(GameState&& other) noexcept -> GameState& {
    if (this != &other) {
        state = other.state;
        other.state = nullptr;
    }
    return *this;
}

auto GameState::GetNextBatter(int const team) const -> std::string {
    if (state != nullptr && team >= 0 && team < 2) {
        int const slot = state->next_batter[team]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        if (slot >= 1 && slot <= numInningsInGame) {
            char const* playerId =
                state->lineups[slot][team].player_id; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
            return (playerId != nullptr) ? playerId : "";
        }
    }
    return "";
}

auto GameState::GetInning() const -> int { return (state != nullptr) ? state->inning : 1; }

auto GameState::GetBattingTeam() const -> int { return (state != nullptr) ? state->batting_team : 0; }

auto GameState::GetOuts() const -> int { return (state != nullptr) ? state->outs : 0; }

auto GameState::GetScore(const int team) const -> int {
    if (state != nullptr && team >= 0 && team < 2) {
        return state->score[team]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    }
    return 0;
}

} // namespace chadwick
