#ifndef BASEBALL_CPP_GAME_HPP
#define BASEBALL_CPP_GAME_HPP

#include <string_view>
#include <filesystem>
#include <vector>
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
    virtual void AddData(const DataRecord& data) = 0;
    virtual void AddRunnerAdjustment(const RunnerAdjustmentInfo& radj) = 0;
    virtual void AddBatterAdjustment(const BatterAdjustmentInfo& badj) = 0;
    virtual void AddPitcherAdjustment(const PitcherAdjustmentInfo& padj) = 0;
    virtual void UpdateState() = 0;
    virtual const GameState& GetGameState() const = 0;
};

class Game : public IGame {
public:
    explicit Game(std::string_view gameId, std::string_view version, 
                  const std::vector<InfoRecord>& infoRecords,
                  const std::vector<StarterInfo>& starters);
    ~Game() override;

    // Disable copying
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // Support move semantics
    Game(Game&& other) noexcept;
    Game& operator=(Game&& other) noexcept;

    bool Write(const std::filesystem::path& path);

    void UpdateState() override;
    void AddEvent(const PlayInfo& play) override;
    void AddSubstitution(const SubstitutionInfo& sub) override;
    void AddComment(std::string_view comment) override;
    void AddData(const DataRecord& data) override;
    void AddRunnerAdjustment(const RunnerAdjustmentInfo& radj) override;
    void AddBatterAdjustment(const BatterAdjustmentInfo& badj) override;
    void AddPitcherAdjustment(const PitcherAdjustmentInfo& padj) override;
    const GameState& GetGameState() const override;

    explicit operator bool() const { return game != nullptr; }

private:
    friend class Scorebook;
    CWGame* game;
    CWGameIterator* iter;
    GameState gameState;

    std::string pendingAutoRunner;
    int pendingAutoBase = 0;

    std::string pendingBatterAdjustmentPlayerID;
    char pendingBatterAdjustmentHand = ' ';

    std::string pendingPitcherAdjustmentPlayerID;
    char pendingPitcherAdjustmentHand = ' ';
};

#endif //BASEBALL_CPP_GAME_HPP
