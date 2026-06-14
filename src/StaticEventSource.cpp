#include "StaticEventSource.hpp"
#include "EventSource.hpp"
#include <optional>
#include <utility>
#include <vector>

StaticEventSource::StaticEventSource(std::vector<Record> records) : records(std::move(records)) {}

auto StaticEventSource::Next() -> std::optional<Record> {
    if (currentIndex < records.size()) {
        return records.at(currentIndex++);
    }
    return std::nullopt;
}
