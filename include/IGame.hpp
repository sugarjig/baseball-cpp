#ifndef BASEBALL_CPP_IGAME_HPP
#define BASEBALL_CPP_IGAME_HPP

#include "EventSource.hpp"
#include "IGameState.hpp"
#include "Records.hpp"

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
     * @brief Adds an event to the game.
     * @param event The event to add.
     */
    virtual void AddEvent(const Event& event) = 0;

    /**
     * @brief Adds a data record to the game.
     * @param data The data record information.
     */
    virtual void AddData(const DataRecord& data) = 0;

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
