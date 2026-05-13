#ifndef BASEBALL_CPP_GAME_HPP
#define BASEBALL_CPP_GAME_HPP

#include <cstdio>
#include "GameState.hpp"

struct cw_game_struct;
typedef struct cw_game_struct CWGame;
struct cw_gameiter_struct;
typedef struct cw_gameiter_struct CWGameIterator;

class Game {
public:
    explicit Game(const char* gameId, const char* date);
    ~Game();

    // Disable copying
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void AddInfo(const char* key, const char* value);
    void AddStarter(const char* id, const char* name, int isHome, int battingOrder, int position);
    void Write(FILE* file);

    void UpdateState();
    void AddEvent(int inning, int team, const char* batter, const char* pitchCount, const char* pitchSequence, const char* text);
    void AddSubstitution(const char* playerID, const char* name, int team, int slot, int pos);
    void AddComment(const char* comment);
    GameState GetGameState() const;

    explicit operator bool() const { return game != nullptr; }

private:
    CWGame* game;
    CWGameIterator* iter;
};

#endif //BASEBALL_CPP_GAME_HPP
