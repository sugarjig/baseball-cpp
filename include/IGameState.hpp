#ifndef BASEBALL_CPP_IGAMESTATE_HPP
#define BASEBALL_CPP_IGAMESTATE_HPP

class IGameState {
public:
    virtual ~IGameState() = default;
    [[nodiscard]] virtual auto GetInning() const -> int = 0;
    [[nodiscard]] virtual auto GetBattingTeam() const -> int = 0;
    [[nodiscard]] virtual auto GetOuts() const -> int = 0;
    [[nodiscard]] virtual auto GetScore(int team) const -> int = 0;
};

#endif // BASEBALL_CPP_IGAMESTATE_HPP
