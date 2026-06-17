#ifndef BASEBALL_CPP_RANDOMEVENTSOURCE_HPP
#define BASEBALL_CPP_RANDOMEVENTSOURCE_HPP

#include "EventSource.hpp"
#include "IGameState.hpp"
#include <cstddef>
#include <optional>
#include <random>
#include <string>
#include <vector>

/**
 * @brief An EventSource that generates random baseball events.
 *
 * The current implementation provides a simplified stream of events:
 * - 90% Strikeouts ("K")
 * - 10% Home Runs ("HR")
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
    std::discrete_distribution<size_t> playDist;
    std::vector<std::string> outcomes;
    static constexpr double probabilityStrikeout = 0.9;
    static constexpr double probabilityHomeRun = 0.1;
};

#endif // BASEBALL_CPP_RANDOMEVENTSOURCE_HPP
