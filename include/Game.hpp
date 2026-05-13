#ifndef BASEBALL_CPP_GAME_HPP
#define BASEBALL_CPP_GAME_HPP

#include <string_view>
#include <filesystem>
#include "GameState.hpp"
#include "Records.hpp"

struct cw_game_struct;
typedef struct cw_game_struct CWGame;
struct cw_gameiter_struct;
typedef struct cw_gameiter_struct CWGameIterator;

class Game {
public:
    explicit Game(std::string_view gameId, std::string_view date);
    ~Game();

    // Disable copying
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void AddInfo(std::string_view key, std::string_view value);
    void AddStarter(const StarterInfo& starter);
    bool Write(const std::filesystem::path& path);

    void UpdateState();
    void AddEvent(const PlayInfo& play);
    void AddSubstitution(const SubstitutionInfo& sub);
    void AddComment(std::string_view comment);
    const GameState& GetGameState() const;

    explicit operator bool() const { return game != nullptr; }

private:
    CWGame* game;
    CWGameIterator* iter;
    GameState gameState;
};

#endif //BASEBALL_CPP_GAME_HPP
