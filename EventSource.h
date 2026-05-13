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
    EventSource();
    std::optional<EventInfo> Next();

private:
    std::vector<EventInfo> events;
    size_t currentIndex;
};

#endif //BASEBALL_CPP_EVENTSOURCE_H
