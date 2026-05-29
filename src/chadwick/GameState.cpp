#include "chadwick/GameState.hpp"
#include <cstdio>
extern "C" {
#include "chadwick.h"
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

auto GameState::GetInning() const -> int { return state ? state->inning : 1; }

auto GameState::GetBattingTeam() const -> int { return state ? state->batting_team : 0; }

auto GameState::GetOuts() const -> int { return state ? state->outs : 0; }

auto GameState::GetScore(int team) const -> int {
    if (state && team >= 0 && team < 2) {
        return state->score[team];
    }
    return 0;
}

} // namespace chadwick
