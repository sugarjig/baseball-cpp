#ifndef BASEBALL_CPP_EVENTSOURCE_H
#define BASEBALL_CPP_EVENTSOURCE_H

#include <string>
#include <vector>
#include <optional>
#include <variant>

struct PlayInfo {
    int inning;
    int team;
    std::string batter;
    std::string pitchCount;
    std::string pitchSequence;
    std::string text;
};

struct SubstitutionInfo {
    std::string playerID;
    std::string name;
    int team;
    int slot;
    int pos;
};

enum class RecordType { Play, Substitution, Comment };

struct Record {
    RecordType type;
    std::variant<PlayInfo, SubstitutionInfo, std::string> data;
};

class EventSource {
public:
    virtual ~EventSource() = default;
    virtual std::optional<Record> Next() = 0;
};

#endif //BASEBALL_CPP_EVENTSOURCE_H
