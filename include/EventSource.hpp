#ifndef BASEBALL_CPP_EVENTSOURCE_HPP
#define BASEBALL_CPP_EVENTSOURCE_HPP

#include "IGameState.hpp"
#include "Records.hpp"
#include <optional>
#include <string>
#include <variant>

/**
 * @brief Represents a single event in a simulation stream.
 */
struct Event {
    EventType type = EventType::Play; ///< The type of event.
    /**
     * @brief The data associated with the event.
     */
    std::variant<PlayInfo, SubstitutionInfo, std::string, StarterInfo, RunnerAdjustmentInfo, BatterAdjustmentInfo,
                 PitcherAdjustmentInfo>
        data;
};

/**
 * @brief Interface for providing a stream of baseball events.
 */
class EventSource {
public:
    EventSource() = default;
    virtual ~EventSource() = default;
    EventSource(const EventSource&) = delete;
    auto operator=(const EventSource&) -> EventSource& = delete;
    EventSource(EventSource&&) = default;
    auto operator=(EventSource&&) -> EventSource& = default;

    /**
     * @brief Retrieves the next event from the simulation stream.
     * @param state The current state of the game.
     * @return An optional containing the next event, or std::nullopt if the end of the stream is reached.
     */
    virtual auto Next(const IGameState& state) -> std::optional<Event> = 0;
};

#endif // BASEBALL_CPP_EVENTSOURCE_HPP
