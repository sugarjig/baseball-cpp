#ifndef BASEBALL_CPP_GAMESTATE_HPP
#define BASEBALL_CPP_GAMESTATE_HPP

struct cw_game_state;
typedef struct cw_game_state CWGameState;

class GameState {
public:
    int GetInning() const;
    int GetBattingTeam() const;
    int GetOuts() const;
    int GetScore(int team) const;
private:
    friend class Game;
    explicit GameState(CWGameState* state);
    CWGameState* state;
};

#endif //BASEBALL_CPP_GAMESTATE_HPP
