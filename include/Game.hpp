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

class IGame {
public:
    virtual ~IGame() = default;
    virtual void AddEvent(const PlayInfo& play) = 0;
    virtual void AddSubstitution(const SubstitutionInfo& sub) = 0;
    virtual void AddComment(std::string_view comment) = 0;
    virtual void AddInfo(std::string_view key, std::string_view value) = 0;
    virtual void AddStarter(const StarterInfo& starter) = 0;
    virtual void UpdateState() = 0;
    virtual const GameState& GetGameState() const = 0;
};

class Game : public IGame {
public:
    explicit Game(std::string_view gameId, std::string_view date, std::string_view version);
    ~Game() override;

    // Disable copying
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void AddInfo(std::string_view key, std::string_view value) override;
    void AddStarter(const StarterInfo& starter) override;
    bool Write(const std::filesystem::path& path);

    void UpdateState() override;
    void AddEvent(const PlayInfo& play) override;
    void AddSubstitution(const SubstitutionInfo& sub) override;
    void AddComment(std::string_view comment) override;
    const GameState& GetGameState() const override;

    explicit operator bool() const { return game != nullptr; }

private:
    CWGame* game;
    CWGameIterator* iter;
    GameState gameState;
};

#endif //BASEBALL_CPP_GAME_HPP
