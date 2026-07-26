#ifndef BASEBALL_CPP_GAMEITERATOR_HPP
#define BASEBALL_CPP_GAMEITERATOR_HPP

#include "GameState.hpp"
#include "IGameState.hpp"

struct cw_game_struct; // NOLINT(readability-identifier-naming)
using CWGame = cw_game_struct;
struct cw_gameiter_struct; // NOLINT(readability-identifier-naming)
using CWGameIterator = cw_gameiter_struct;

namespace chadwick {

/**
 * @brief Internal wrapper for Chadwick's CWGameIterator.
 *
 * This class handles the iteration over game events to maintain the current game state.
 * It also includes workarounds for known Chadwick library issues.
 */
class GameIterator {
public:
    /**
     * @brief Constructs a GameIterator for a given Chadwick game.
     * @param game The Chadwick CWGame to iterate over.
     */
    explicit GameIterator(CWGame* game);
    /**
     * @brief Destructor that ensures proper cleanup of the Chadwick iterator.
     */
    ~GameIterator();

    // Disable copying
    GameIterator(const GameIterator&) = delete;
    auto operator=(const GameIterator&) -> GameIterator& = delete;

    /**
     * @brief Move constructor for GameIterator.
     * @param other The GameIterator to move from.
     */
    GameIterator(GameIterator&& other) noexcept;
    /**
     * @brief Move assignment operator for GameIterator.
     * @param other The GameIterator to move from.
     * @return A reference to this GameIterator.
     */
    auto operator=(GameIterator&& other) noexcept -> GameIterator&;

    /**
     * @brief Updates the game state by resetting the iterator and re-processing all events from the beginning.
     *
     * This implementation is necessary because Chadwick's internal state management relies on
     * sequential iteration. However, it means this operation is O(N) where N is the number of
     * events currently in the game.
     *
     * When called after every event (as in Simulator::SimulateGame), the total time complexity
     * for simulating a game with N events becomes O(N^2).
     */
    void UpdateState();

    /**
     * @brief Gets the wrapped game state.
     * @return A reference to the IGameState.
     */
    [[nodiscard]] auto GetGameState() const -> const IGameState&;

private:
    CWGameIterator* iter;
    GameState gameState;
    static constexpr int suspendedTextSize = 10;
};

} // namespace chadwick

#endif // BASEBALL_CPP_GAMEITERATOR_HPP
