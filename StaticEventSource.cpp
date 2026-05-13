#include "StaticEventSource.h"

StaticEventSource::StaticEventSource() : currentIndex(0) {
    records = {
        {RecordType::Comment, "Start of the game"},
        {RecordType::Play, PlayInfo{1, 0, "AWAY01", "K"}},
        {RecordType::Play, PlayInfo{1, 0, "AWAY02", "K"}},
        {RecordType::Play, PlayInfo{1, 0, "AWAY03", "K"}},
        {RecordType::Substitution, SubstitutionInfo{"H_SUB01", "Sub Name", 1, 1, 7}},
        {RecordType::Comment, "Home team lead-off home run coming up"},
        {RecordType::Play, PlayInfo{1, 1, "HOME01", "HR"}},
        {RecordType::Play, PlayInfo{1, 1, "HOME02", "K"}},
        {RecordType::Play, PlayInfo{1, 1, "HOME03", "K"}},
        {RecordType::Play, PlayInfo{1, 1, "HOME04", "K"}}
    };
}

std::optional<Record> StaticEventSource::Next() {
    if (currentIndex < records.size()) {
        return records[currentIndex++];
    }
    return std::nullopt;
}
