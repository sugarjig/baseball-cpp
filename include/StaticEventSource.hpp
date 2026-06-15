#ifndef BASEBALL_CPP_STATICEVENTSOURCE_HPP
#define BASEBALL_CPP_STATICEVENTSOURCE_HPP

#include "EventSource.hpp"
#include "IGameState.hpp"
#include <cstdlib>
#include <optional>
#include <vector>

/**
 * @brief An EventSource that provides events from a pre-defined list of records.
 *
 * This is primarily used for testing or scenarios where the sequence of events is fixed.
 */
class StaticEventSource : public EventSource {
public:
    /**
     * @brief Constructs a StaticEventSource with a vector of records.
     * @param records The list of records to provide.
     */
    explicit StaticEventSource(std::vector<Record> records);
    ~StaticEventSource() override = default;

    // Disable copying
    StaticEventSource(const StaticEventSource&) = delete;
    auto operator=(const StaticEventSource&) -> StaticEventSource& = delete;

    // Support move semantics
    StaticEventSource(StaticEventSource&&) noexcept = default;
    auto operator=(StaticEventSource&&) noexcept -> StaticEventSource& = default;

    /// @inheritdoc
    auto Next(const IGameState& state) -> std::optional<Record> override;

private:
    std::vector<Record> records;
    size_t currentIndex = 0;
};

#endif // BASEBALL_CPP_STATICEVENTSOURCE_HPP
