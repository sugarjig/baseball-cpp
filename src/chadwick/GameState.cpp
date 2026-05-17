#include "chadwick/GameState.hpp"
#include <cstdio>
extern "C" {
#include "chadwick.h"
}

namespace chadwick {

GameState::GameState(CWGameState* state) : state(state) {}

int GameState::GetInning() const {
    return state ? state->inning : 1;
}

int GameState::GetBattingTeam() const {
    return state ? state->batting_team : 0;
}

int GameState::GetOuts() const {
    return state ? state->outs : 0;
}

int GameState::GetScore(int team) const {
    if (state && team >= 0 && team < 2) {
        return state->score[team];
    }
    return 0;
}

} // namespace chadwick
