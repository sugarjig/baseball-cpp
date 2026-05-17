#ifndef BASEBALL_CPP_IGAMESTATE_HPP
#define BASEBALL_CPP_IGAMESTATE_HPP

class IGameState {
public:
    virtual ~IGameState() = default;
    virtual int GetInning() const = 0;
    virtual int GetBattingTeam() const = 0;
    virtual int GetOuts() const = 0;
    virtual int GetScore(int team) const = 0;
};

#endif //BASEBALL_CPP_IGAMESTATE_HPP
