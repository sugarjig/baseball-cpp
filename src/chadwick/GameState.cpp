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

GameState::GameState(CWGameState* cwGameState) : cwGameState(cwGameState) {}

GameState::GameState(GameState&& other) noexcept
    : cwGameState(other.cwGameState), numInningsInGame(other.numInningsInGame), homeBatsFirst(other.homeBatsFirst) {
    other.cwGameState = nullptr;
}

auto GameState::operator=(GameState&& other) noexcept -> GameState& {
    if (this != &other) {
        cwGameState = other.cwGameState;
        numInningsInGame = other.numInningsInGame;
        homeBatsFirst = other.homeBatsFirst;
        other.cwGameState = nullptr;
    }
    return *this;
}

auto GameState::GetNextBatter(int const team) const -> std::string {
    if (cwGameState != nullptr && team >= 0 && team < 2) {
        int const slot = cwGameState->next_batter[team]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        if (slot >= 1 && slot <= numInningsInGame) {
            char const* playerId =
                cwGameState->lineups[slot][team].player_id; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
            return (playerId != nullptr) ? playerId : "";
        }
    }
    return "";
}

auto GameState::GetRunnerOnBase(int const base) const -> std::string {
    if (cwGameState != nullptr && base >= 1 && base <= 3) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,cppcoreguidelines-pro-bounds-constant-array-index)
        return cwGameState->runners[base].runner;
    }
    return "";
}

auto GameState::GetInning() const -> int { return (cwGameState != nullptr) ? cwGameState->inning : 1; }

auto GameState::GetBattingTeam() const -> int { return (cwGameState != nullptr) ? cwGameState->batting_team : 0; }

auto GameState::GetOuts() const -> int { return (cwGameState != nullptr) ? cwGameState->outs : 0; }

auto GameState::IsBaseOccupied(int const base) const -> bool {
    return (cwGameState != nullptr) && (cw_gamestate_base_occupied(cwGameState, base) != 0);
}

auto GameState::GetScore(const int team) const -> int {
    if (cwGameState != nullptr && team >= 0 && team < 2) {
        return cwGameState->score[team]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    }
    return 0;
}

auto GameState::KeepPlaying() const -> bool {
    int inning = GetInning();
    int team = GetBattingTeam();

    int const bottomTeam = homeBatsFirst ? 0 : 1;
    int const topTeam = homeBatsFirst ? 1 : 0;

    // Check if half-inning is over
    if (GetOuts() >= 3) {
        if (team == topTeam) {
            team = bottomTeam;
        } else {
            team = topTeam;
            inning++;
        }
    }

    int const bottomScore = GetScore(bottomTeam);
    int const topScore = GetScore(topTeam);

    // If it's the bottom of the last inning or later, and the bottom team is ahead, the game is over.
    if (inning >= numInningsInGame && team == bottomTeam && bottomScore > topScore) {
        return false;
    }

    // If it's the end of the last inning or later (about to start top of next inning), and there is not a tie,
    // the game is over.
    if (GetOuts() >= 3 && inning > numInningsInGame && team == topTeam && topScore != bottomScore) {
        return false;
    }

    return true;
}

} // namespace chadwick
