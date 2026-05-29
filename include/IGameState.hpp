#ifndef BASEBALL_CPP_IGAMESTATE_HPP
#define BASEBALL_CPP_IGAMESTATE_HPP

class IGameState {
public:
    IGameState() = default;
    virtual ~IGameState() = default;
    IGameState(const IGameState&) = delete;
    auto operator=(const IGameState&) -> IGameState& = delete;
    IGameState(IGameState&&) = default;
    auto operator=(IGameState&&) -> IGameState& = default;

    [[nodiscard]] virtual auto GetInning() const -> int = 0;
    [[nodiscard]] virtual auto GetBattingTeam() const -> int = 0;
    [[nodiscard]] virtual auto GetOuts() const -> int = 0;
    [[nodiscard]] virtual auto GetScore(int team) const -> int = 0;
};

#endif // BASEBALL_CPP_IGAMESTATE_HPP
