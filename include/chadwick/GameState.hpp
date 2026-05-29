#ifndef BASEBALL_CPP_GAMESTATE_HPP
#define BASEBALL_CPP_GAMESTATE_HPP

#include "../IGameState.hpp"

struct cw_game_state;
using CWGameState = struct cw_game_state;

namespace chadwick {

class GameState : public IGameState {
public:
    GameState() : state(nullptr) {}
    ~GameState() override = default;

    // Disable copying
    GameState(const GameState&) = delete;
    auto operator=(const GameState&) -> GameState& = delete;

    // Support move semantics
    GameState(GameState&& other) noexcept;
    auto operator=(GameState&& other) noexcept -> GameState&;

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
