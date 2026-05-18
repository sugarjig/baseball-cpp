#include "StaticEventSource.hpp"
#include <utility>

StaticEventSource::StaticEventSource(std::vector<Record> records) : records(std::move(records)), currentIndex(0) {}

auto StaticEventSource::Next() -> std::optional<Record> {
    if (currentIndex < records.size()) {
        return records[currentIndex++];
    }
    return std::nullopt;
}
