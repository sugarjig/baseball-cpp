#ifndef BASEBALL_CPP_GAME_HPP
#define BASEBALL_CPP_GAME_HPP

#include <cstdio>
#include "GameState.hpp"
#include "Records.hpp"

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
    void AddStarter(const StarterInfo& starter);
    void Write(FILE* file);

    void UpdateState();
    void AddEvent(const PlayInfo& play);
    void AddSubstitution(const SubstitutionInfo& sub);
    void AddComment(const char* comment);
    GameState GetGameState() const;

    explicit operator bool() const { return game != nullptr; }

private:
    CWGame* game;
    CWGameIterator* iter;
};

#endif //BASEBALL_CPP_GAME_HPP
