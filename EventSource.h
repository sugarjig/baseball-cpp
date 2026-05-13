#ifndef BASEBALL_CPP_EVENTSOURCE_H
#define BASEBALL_CPP_EVENTSOURCE_H

#include <string>
#include <vector>
#include <optional>

struct EventInfo {
    int inning;
    int team;
    std::string batter;
    std::string text;
};

class EventSource {
public:
    virtual ~EventSource() = default;
    virtual std::optional<EventInfo> Next() = 0;
};

#endif //BASEBALL_CPP_EVENTSOURCE_H
