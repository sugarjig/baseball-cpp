#ifndef BASEBALL_CPP_GAME_HPP
#define BASEBALL_CPP_GAME_HPP

struct cw_game_struct;
typedef struct cw_game_struct CWGame;

class Game {
public:
    explicit Game(const char* gameId, const char* date);
    ~Game();

    // Disable copying
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void AddInfo(const char* key, const char* value);
    void AddStarter(const char* id, const char* name, int isHome, int battingOrder, int position);

    CWGame* getCWGame() const { return game; }

private:
    CWGame* game;
};

#endif //BASEBALL_CPP_GAME_HPP
