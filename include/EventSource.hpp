#ifndef BASEBALL_CPP_EVENTSOURCE_HPP
#define BASEBALL_CPP_EVENTSOURCE_HPP

#include "IGameState.hpp"
#include "Records.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <variant>

/**
 * @brief Types of events that can be encountered in a baseball simulation.
 */
enum class EventType : std::uint8_t {
    Play,             ///< A play event (e.g., hit, out).
    Substitution,     ///< A player substitution.
    Comment,          ///< A comment event.
    RunnerAdjustment, ///< An adjustment to a runner's position.
    BatterAdjustment, ///< An adjustment to the batter's hand.
    PitcherAdjustment ///< An adjustment to a pitcher's hand.
};

/**
 * @brief Represents a single event in a simulation stream.
 */
struct Event {
    EventType type = EventType::Play; ///< The type of event.
    /**
     * @brief The data associated with the event.
     */
    std::variant<PlayInfo, SubstitutionInfo, std::string, RunnerAdjustmentInfo, BatterAdjustmentInfo,
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
