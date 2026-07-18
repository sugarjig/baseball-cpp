#include "chadwick/GameState.hpp"
// ReSharper disable once CppUnusedIncludeDirective
#include <cstdio> // NOLINT(misc-include-cleaner)
#include <string> // IWYU pragma: keep
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

auto GameState::IsBaseOccupied(int const base) const -> bool {
    return (state != nullptr) && (cw_gamestate_base_occupied(state, base) != 0);
}

auto GameState::GetScore(const int team) const -> int {
    if (state != nullptr && team >= 0 && team < 2) {
        return state->score[team]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    }
    return 0;
}

auto GameState::KeepPlaying() const -> bool {
    int inning = GetInning();
    int team = GetBattingTeam();

    // Check if half-inning is over
    if (GetOuts() >= 3) {
        if (team == 0) {
            team = 1;
        } else {
            team = 0;
            inning++;
        }
    }

    int const visitorScore = GetScore(0);
    int const homeScore = GetScore(1);

    // If it's the bottom of the 9th inning or later, and the home team is ahead, the game is over.
    if (inning >= numInningsInGame && team == 1 && homeScore > visitorScore) {
        return false;
    }

    // If it's the end of the 9th or later (about to start top of next inning), and there is not a tie, the game is
    // over.
    if (GetOuts() >= 3 && inning > numInningsInGame && team == 0 && visitorScore != homeScore) {
        return false;
    }

    return true;
}

} // namespace chadwick
