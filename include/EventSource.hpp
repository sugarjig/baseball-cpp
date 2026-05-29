#ifndef BASEBALL_CPP_EVENTSOURCE_HPP
#define BASEBALL_CPP_EVENTSOURCE_HPP

#include "Records.hpp"
#include <optional>
#include <string>
#include <variant>
#include <vector>

struct Record {
    RecordType type;
    std::variant<PlayInfo, SubstitutionInfo, std::string, StarterInfo, RunnerAdjustmentInfo, BatterAdjustmentInfo,
                 PitcherAdjustmentInfo>
        data;
};

class EventSource {
public:
    EventSource() = default;
    virtual ~EventSource() = default;
    EventSource(const EventSource&) = delete;
    auto operator=(const EventSource&) -> EventSource& = delete;
    EventSource(EventSource&&) = default;
    auto operator=(EventSource&&) -> EventSource& = default;

    virtual auto Next() -> std::optional<Record> = 0;
};

#endif // BASEBALL_CPP_EVENTSOURCE_HPP
