#ifndef BASEBALL_CPP_STATICEVENTSOURCE_HPP
#define BASEBALL_CPP_STATICEVENTSOURCE_HPP

#include "EventSource.hpp"
#include <vector>

class StaticEventSource : public EventSource {
public:
    explicit StaticEventSource(std::vector<Record> records);
    ~StaticEventSource() override = default;

    // Disable copying
    StaticEventSource(const StaticEventSource&) = delete;
    auto operator=(const StaticEventSource&) -> StaticEventSource& = delete;

    // Support move semantics
    StaticEventSource(StaticEventSource&&) noexcept = default;
    auto operator=(StaticEventSource&&) noexcept -> StaticEventSource& = default;

    auto Next() -> std::optional<Record> override;

private:
    std::vector<Record> records;
    size_t currentIndex = 0;
};

#endif // BASEBALL_CPP_STATICEVENTSOURCE_HPP
