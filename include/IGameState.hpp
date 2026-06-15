#ifndef BASEBALL_CPP_IGAMESTATE_HPP
#define BASEBALL_CPP_IGAMESTATE_HPP

/**
 * @brief Interface for the current state of a baseball game.
 */
class IGameState {
public:
    IGameState() = default;
    virtual ~IGameState() = default;
    IGameState(const IGameState&) = delete;
    auto operator=(const IGameState&) -> IGameState& = delete;
    IGameState(IGameState&&) = default;
    auto operator=(IGameState&&) -> IGameState& = default;

    /**
     * @brief Gets the current inning.
     * @return The inning number (1-based).
     */
    [[nodiscard]] virtual auto GetInning() const -> int = 0;

    /**
     * @brief Gets the team currently at bat.
     * @return 0 for visitor, 1 for home.
     */
    [[nodiscard]] virtual auto GetBattingTeam() const -> int = 0;

    /**
     * @brief Gets the number of outs in the current half-inning.
     * @return The number of outs (0, 1, or 2).
     */
    [[nodiscard]] virtual auto GetOuts() const -> int = 0;

    /**
     * @brief Gets the current score for a specific team.
     * @param team The team (0 for visitor, 1 for home).
     * @return The current score for that team.
     */
    [[nodiscard]] virtual auto GetScore(int team) const -> int = 0;
};

#endif // BASEBALL_CPP_IGAMESTATE_HPP
