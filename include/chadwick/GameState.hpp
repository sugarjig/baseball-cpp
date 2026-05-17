#ifndef BASEBALL_CPP_GAMESTATE_HPP
#define BASEBALL_CPP_GAMESTATE_HPP

#include "../IGameState.hpp"

struct cw_game_state;
typedef struct cw_game_state CWGameState;

namespace chadwick {

class GameState : public IGameState {
public:
    GameState() : state(nullptr) {}
    int GetInning() const override;
    int GetBattingTeam() const override;
    int GetOuts() const override;
    int GetScore(int team) const override;

private:
    friend class Game;
    explicit GameState(CWGameState* state);
    CWGameState* state;
};

} // namespace chadwick

#endif // BASEBALL_CPP_GAMESTATE_HPP
