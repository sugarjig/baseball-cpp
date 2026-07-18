#ifndef BASEBALL_CPP_RANDOMEVENTSOURCE_HPP
#define BASEBALL_CPP_RANDOMEVENTSOURCE_HPP

#include "EventSource.hpp"
#include "IGameState.hpp"
#include "Records.hpp"
#include <cstddef>
#include <optional> // IWYU pragma: keep
#include <random>   // IWYU pragma: keep
#include <string>   // IWYU pragma: keep
#include <vector>   // IWYU pragma: keep

/**
 * @brief An EventSource that generates random baseball events.
 *
 * The current implementation provides a simplified stream of events:
 * - 70% Strikeouts ("K")
 * - 10% Home Runs ("HR")
 * - 20% Singles ("S")
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
    auto GetOutcome(int stateId) -> std::string;
    static void ApplyAdvancements(PlayInfo& play, unsigned int bases);

    std::mt19937 rng{std::random_device{}()};
    static constexpr double probabilityStrikeout = 0.7;
    static constexpr double probabilityHomeRun = 0.1;
    static constexpr double probabilitySingle = 0.2;
    std::discrete_distribution<size_t> playDist{probabilityStrikeout, probabilityHomeRun, probabilitySingle};
    std::vector<std::string> outcomes{"K", "HR", "S"};
    static constexpr int numBaseStates = 8;
};

#endif // BASEBALL_CPP_RANDOMEVENTSOURCE_HPP
