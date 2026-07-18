#ifndef BASEBALL_CPP_GAME_HPP
#define BASEBALL_CPP_GAME_HPP

#include "../IGame.hpp"
#include "../Records.hpp"
#include "GameState.hpp"
#include "IGameState.hpp"
#include <filesystem>
#include <string>      // IWYU pragma: keep
#include <string_view> // IWYU pragma: keep
#include <vector>      // IWYU pragma: keep

struct cw_game_struct; // NOLINT(readability-identifier-naming)
using CWGame = cw_game_struct;
struct cw_gameiter_struct; // NOLINT(readability-identifier-naming)
using CWGameIterator = cw_gameiter_struct;

namespace chadwick {

/**
 * @brief Implementation of IGame that wraps Chadwick's CWGame structure.
 *
 * This class manages the lifecycle of a Chadwick game object and its associated iterator.
 * It provides a modern C++ interface for adding events and querying game state.
 */
class Game : public IGame {
public:
    /**
     * @brief Constructs a Game with basic metadata and initial lineups.
     * @param gameId The unique identifier for the game.
     * @param version The Retrosheet format version.
     * @param infoRecords Game metadata records.
     * @param starters Starting lineups for both teams.
     */
    explicit Game(std::string_view gameId, std::string_view version, const std::vector<InfoRecord>& infoRecords,
                  const std::vector<StarterInfo>& starters);

    /**
     * @brief Destructor that ensures proper cleanup of Chadwick resources.
     */
    ~Game() override;

    // Disable copying
    Game(const Game&) = delete;
    auto operator=(const Game&) -> Game& = delete;

    /**
     * @brief Move constructor for Game.
     * @param other The Game to move from.
     */
    Game(Game&& other) noexcept;
    /**
     * @brief Move assignment operator for Game.
     * @param other The Game to move from.
     * @return A reference to this Game.
     */
    auto operator=(Game&& other) noexcept -> Game&;

    /**
     * @brief Writes the game events to a Retrosheet file.
     * @param path The path to the file to write.
     * @return True if successful, false otherwise.
     */
    [[nodiscard]] auto Write(const std::filesystem::path& path) const -> bool;

    /// @inheritdoc
    void UpdateState() override;
    /// @inheritdoc
    void AddEvent(const PlayInfo& play) override;
    /// @inheritdoc
    void AddSubstitution(const SubstitutionInfo& sub) override;
    /// @inheritdoc
    void AddComment(std::string_view comment) override;
    /// @inheritdoc
    void AddData(const DataRecord& data) override;
    /// @inheritdoc
    void AddRunnerAdjustment(const RunnerAdjustmentInfo& radj) override;
    /// @inheritdoc
    void AddBatterAdjustment(const BatterAdjustmentInfo& badj) override;
    /// @inheritdoc
    void AddPitcherAdjustment(const PitcherAdjustmentInfo& padj) override;
    /// @inheritdoc
    [[nodiscard]] auto GetGameState() const -> const IGameState& override;

    /**
     * @brief Checks if the game was successfully initialized.
     * @return True if the underlying Chadwick game object exists.
     */
    explicit operator bool() const { return game != nullptr; }

private:
    friend class Scorebook;
    CWGame* game = nullptr;
    CWGameIterator* iter = nullptr;
    GameState gameState;

    std::string pendingAutoRunner;
    int pendingAutoBase = 0;

    std::string pendingBatterAdjustmentPlayerId;
    char pendingBatterAdjustmentHand = ' ';

    std::string pendingPitcherAdjustmentPlayerId;
    char pendingPitcherAdjustmentHand = ' ';

    static constexpr int suspendedTextSize = 10;
};

} // namespace chadwick

#endif // BASEBALL_CPP_GAME_HPP
