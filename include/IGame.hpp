#ifndef BASEBALL_CPP_IGAME_HPP
#define BASEBALL_CPP_IGAME_HPP

#include "IGameState.hpp"
#include "Records.hpp"
#include <string_view>

/**
 * @brief Interface for a baseball game that can process events.
 */
class IGame {
public:
    IGame() = default;
    virtual ~IGame() = default;
    IGame(const IGame&) = delete;
    auto operator=(const IGame&) -> IGame& = delete;
    IGame(IGame&&) = default;
    auto operator=(IGame&&) -> IGame& = default;

    /**
     * @brief Adds a play event to the game.
     * @param play The play information.
     */
    virtual void AddEvent(const PlayInfo& play) = 0;

    /**
     * @brief Adds a substitution to the game.
     * @param sub The substitution information.
     */
    virtual void AddSubstitution(const SubstitutionInfo& sub) = 0;

    /**
     * @brief Adds a comment to the game.
     * @param comment The comment text.
     */
    virtual void AddComment(std::string_view comment) = 0;

    /**
     * @brief Adds a data record to the game.
     * @param data The data record information.
     */
    virtual void AddData(const DataRecord& data) = 0;

    /**
     * @brief Adds a runner adjustment to the game.
     * @param radj The runner adjustment information.
     */
    virtual void AddRunnerAdjustment(const RunnerAdjustmentInfo& radj) = 0;

    /**
     * @brief Adds a batter adjustment to the game.
     * @param badj The batter adjustment information.
     */
    virtual void AddBatterAdjustment(const BatterAdjustmentInfo& badj) = 0;

    /**
     * @brief Adds a pitcher adjustment to the game.
     * @param padj The pitcher adjustment information.
     */
    virtual void AddPitcherAdjustment(const PitcherAdjustmentInfo& padj) = 0;

    /**
     * @brief Updates the internal state of the game based on added events.
     */
    virtual void UpdateState() = 0;

    /**
     * @brief Gets the current state of the game.
     * @return A reference to the game state.
     */
    [[nodiscard]] virtual auto GetGameState() const -> const IGameState& = 0;
};

#endif // BASEBALL_CPP_IGAME_HPP
