#include "StaticEventSource.hpp"
#include "IGameState.hpp"
#include <optional>
#include <utility>
#include <vector>

StaticEventSource::StaticEventSource(std::vector<Record> records) : records(std::move(records)) {}

auto StaticEventSource::Next(const IGameState& /*state*/)
    -> std::optional<Record> { // NOLINT(readability-convert-member-functions-to-static)
    if (currentIndex < records.size()) {
        return records.at(currentIndex++);
    }
    return std::nullopt;
}
