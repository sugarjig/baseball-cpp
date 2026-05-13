#ifndef BASEBALL_CPP_STATICEVENTSOURCE_H
#define BASEBALL_CPP_STATICEVENTSOURCE_H

#include "EventSource.h"
#include <vector>

class StaticEventSource : public EventSource {
public:
    StaticEventSource();
    std::optional<EventInfo> Next() override;

private:
    std::vector<EventInfo> events;
    size_t currentIndex;
};

#endif //BASEBALL_CPP_STATICEVENTSOURCE_H
