#ifndef BASEBALL_CPP_GAME_HPP
#define BASEBALL_CPP_GAME_HPP

#include "../IGame.hpp"
#include "../Records.hpp"
#include "GameState.hpp"
#include <filesystem>
#include <string_view>
#include <vector>

struct cw_game_struct;
using CWGame = struct cw_game_struct;
struct cw_gameiter_struct;
using CWGameIterator = struct cw_gameiter_struct;

namespace chadwick {

class Game : public IGame {
public:
    explicit Game(std::string_view gameId, std::string_view version, const std::vector<InfoRecord>& infoRecords,
                  const std::vector<StarterInfo>& starters);
    ~Game() override;

    // Disable copying
    Game(const Game&) = delete;
    auto operator=(const Game&) -> Game& = delete;

    // Support move semantics
    Game(Game&& other) noexcept;
    auto operator=(Game&& other) noexcept -> Game&;

    auto Write(const std::filesystem::path& path) const -> bool;

    void UpdateState() override;
    void AddEvent(const PlayInfo& play) override;
    void AddSubstitution(const SubstitutionInfo& sub) override;
    void AddComment(std::string_view comment) override;
    void AddData(const DataRecord& data) override;
    void AddRunnerAdjustment(const RunnerAdjustmentInfo& radj) override;
    void AddBatterAdjustment(const BatterAdjustmentInfo& badj) override;
    void AddPitcherAdjustment(const PitcherAdjustmentInfo& padj) override;
    [[nodiscard]] auto GetGameState() const -> const IGameState& override;

    explicit operator bool() const { return game != nullptr; }

private:
    friend class Scorebook;
    CWGame* game;
    CWGameIterator* iter;
    GameState gameState;

    std::string pendingAutoRunner;
    int pendingAutoBase = 0;

    std::string pendingBatterAdjustmentPlayerId;
    char pendingBatterAdjustmentHand = ' ';

    std::string pendingPitcherAdjustmentPlayerId;
    char pendingPitcherAdjustmentHand = ' ';
};

} // namespace chadwick

#endif // BASEBALL_CPP_GAME_HPP
