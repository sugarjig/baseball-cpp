#ifndef BASEBALL_CPP_EVENTSOURCE_HPP
#define BASEBALL_CPP_EVENTSOURCE_HPP

#include "IGameState.hpp"
#include "Records.hpp"
#include <optional> // IWYU pragma: keep
#include <string>   // IWYU pragma: keep
#include <variant>  // IWYU pragma: keep

/**
 * @brief Represents a single record in an event stream.
 */
struct Record {
    RecordType type = RecordType::Play; ///< The type of record.
    /**
     * @brief The data associated with the record.
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
     * @brief Retrieves the next record from the event stream.
     * @param state The current state of the game.
     * @return An optional containing the next record, or std::nullopt if the end of the stream is reached.
     */
    virtual auto Next(const IGameState& state) -> std::optional<Record> = 0;
};

#endif // BASEBALL_CPP_EVENTSOURCE_HPP
