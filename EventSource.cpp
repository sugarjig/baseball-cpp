#include "EventSource.h"

EventSource::EventSource() : currentIndex(0) {
    events = {
        {1, 0, "AWAY01", "K"},
        {1, 0, "AWAY02", "K"},
        {1, 0, "AWAY03", "K"},
        {1, 1, "HOME01", "HR"},
        {1, 1, "HOME02", "K"},
        {1, 1, "HOME03", "K"},
        {1, 1, "HOME04", "K"}
    };
}

std::optional<EventInfo> EventSource::Next() {
    if (currentIndex < events.size()) {
        return events[currentIndex++];
    }
    return std::nullopt;
}
