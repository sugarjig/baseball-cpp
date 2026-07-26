#include "StaticEventSource.hpp"
#include "EventSource.hpp"
#include "IGameState.hpp"
#include <optional>
#include <utility>
#include <vector>

StaticEventSource::StaticEventSource(std::vector<Event> events) : events(std::move(events)) {}

auto StaticEventSource::Next(const IGameState& /*state*/) -> std::optional<Event> {
    if (currentIndex < events.size()) {
        return events.at(currentIndex++);
    }
    return std::nullopt;
}
