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
    virtual ~EventSource() = default;
    virtual std::optional<Record> Next() = 0;
};

#endif // BASEBALL_CPP_EVENTSOURCE_HPP
