#ifndef BASEBALL_CPP_RECORDS_HPP
#define BASEBALL_CPP_RECORDS_HPP

#include <cstdint>
#include <string>
#include <vector>

enum class RecordType : std::uint8_t {
    Play,
    Substitution,
    Comment,
    Starter,
    RunnerAdjustment,
    BatterAdjustment,
    PitcherAdjustment
};

struct InfoRecord {
    std::string key;
    std::string value;
};

struct DataRecord {
    std::vector<std::string> fields;
};

struct PlayInfo {
    int inning = 0;
    int team = 0;
    std::string batter;
    std::string pitchCount;
    std::string pitchSequence;
    std::string text;
};

struct SubstitutionInfo {
    std::string playerId;
    std::string name;
    int team = 0;
    int slot = 0;
    int pos = 0;
};

struct StarterInfo {
    std::string id;
    std::string name;
    bool isHome = false;
    int battingOrder = 0;
    int position = 0;
};

struct RunnerAdjustmentInfo {
    std::string playerId;
    int base = 0;
};

struct BatterAdjustmentInfo {
    std::string playerId;
    char hand = '\0';
};

struct PitcherAdjustmentInfo {
    std::string playerId;
    char hand = '\0';
};

#endif // BASEBALL_CPP_RECORDS_HPP
