#ifndef BASEBALL_CPP_RANDOMEVENTSOURCE_HPP
#define BASEBALL_CPP_RANDOMEVENTSOURCE_HPP

#include "EventSource.hpp"
#include "IGameState.hpp"
#include <optional>
#include <random>

/**
 * @brief An EventSource that generates random baseball events.
 *
 * Current implementation provides a simplified stream of events:
 * - 90% Strikeouts ("K")
 * - 10% Home Runs ("HR")
 *
 * It generates events until the game exceeds 9 innings.
 */
class RandomEventSource : public EventSource {
public:
    /**
     * @brief Constructs a RandomEventSource with an optional seed.
     * @param seed The seed for the random number generator.
     */
    explicit RandomEventSource(unsigned int seed = std::random_device{}());
    ~RandomEventSource() override = default;

    // Disable copying
    RandomEventSource(const RandomEventSource&) = delete;
    auto operator=(const RandomEventSource&) -> RandomEventSource& = delete;

    // Support move semantics
    RandomEventSource(RandomEventSource&&) noexcept = default;
    auto operator=(RandomEventSource&&) noexcept -> RandomEventSource& = default;

    /// @inheritdoc
    auto Next(const IGameState& state) -> std::optional<Record> override;

private:
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;
};

#endif // BASEBALL_CPP_RANDOMEVENTSOURCE_HPP
