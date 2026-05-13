#ifndef BASEBALL_CPP_STATICEVENTSOURCE_HPP
#define BASEBALL_CPP_STATICEVENTSOURCE_HPP

#include "EventSource.hpp"
#include <vector>

class StaticEventSource : public EventSource {
public:
    StaticEventSource();
    std::optional<Record> Next() override;

private:
    std::vector<Record> records;
    size_t currentIndex;
};

#endif //BASEBALL_CPP_STATICEVENTSOURCE_HPP
