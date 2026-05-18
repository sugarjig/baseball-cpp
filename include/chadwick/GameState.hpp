#ifndef BASEBALL_CPP_GAMESTATE_HPP
#define BASEBALL_CPP_GAMESTATE_HPP

#include "../IGameState.hpp"

struct cw_game_state;
typedef struct cw_game_state CWGameState;

namespace chadwick {

class GameState : public IGameState {
public:
    GameState() : state(nullptr) {}
    [[nodiscard]] auto GetInning() const -> int override;
    [[nodiscard]] auto GetBattingTeam() const -> int override;
    [[nodiscard]] auto GetOuts() const -> int override;
    [[nodiscard]] auto GetScore(int team) const -> int override;

private:
    friend class Game;
    explicit GameState(CWGameState* state);
    CWGameState* state;
};

} // namespace chadwick

#endif // BASEBALL_CPP_GAMESTATE_HPP
