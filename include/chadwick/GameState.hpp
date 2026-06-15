#ifndef BASEBALL_CPP_GAMESTATE_HPP
#define BASEBALL_CPP_GAMESTATE_HPP

#include "../IGameState.hpp"

struct cw_game_state; // NOLINT(readability-identifier-naming)
using CWGameState = cw_game_state;

namespace chadwick {

/**
 * @brief Implementation of IGameState backed by Chadwick's cw_game_state.
 */
class GameState : public IGameState {
public:
    /**
     * @brief Constructs an empty GameState.
     */
    GameState() : state(nullptr) {}
    ~GameState() override = default;

    // Disable copying
    GameState(const GameState&) = delete;
    auto operator=(const GameState&) -> GameState& = delete;

    /**
     * @brief Move constructor for GameState.
     * @param other The GameState to move from.
     */
    GameState(GameState&& other) noexcept;
    /**
     * @brief Move assignment operator for GameState.
     * @param other The GameState to move from.
     * @return A reference to this GameState.
     */
    auto operator=(GameState&& other) noexcept -> GameState&;

    /// @inheritdoc
    [[nodiscard]] auto GetInning() const -> int override;
    /// @inheritdoc
    [[nodiscard]] auto GetBattingTeam() const -> int override;
    /// @inheritdoc
    [[nodiscard]] auto GetOuts() const -> int override;
    /// @inheritdoc
    [[nodiscard]] auto GetScore(int team) const -> int override;

private:
    friend class Game;
    explicit GameState(CWGameState* state);
    CWGameState* state;
};

} // namespace chadwick

#endif // BASEBALL_CPP_GAMESTATE_HPP
