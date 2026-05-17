#ifndef BASEBALL_CPP_RECORDS_HPP
#define BASEBALL_CPP_RECORDS_HPP

#include <string>
#include <vector>

enum class RecordType { Play, Substitution, Comment, Starter, RunnerAdjustment, BatterAdjustment, PitcherAdjustment };

struct InfoRecord {
    std::string key;
    std::string value;
};

struct DataRecord {
    std::vector<std::string> fields;
};

struct PlayInfo {
    int inning;
    int team;
    std::string batter;
    std::string pitchCount;
    std::string pitchSequence;
    std::string text;
};

struct SubstitutionInfo {
    std::string playerId;
    std::string name;
    int team;
    int slot;
    int pos;
};

struct StarterInfo {
    std::string id;
    std::string name;
    bool isHome;
    int battingOrder;
    int position;
};

struct RunnerAdjustmentInfo {
    std::string playerId;
    int base;
};

struct BatterAdjustmentInfo {
    std::string playerId;
    char hand;
};

struct PitcherAdjustmentInfo {
    std::string playerId;
    char hand;
};

#endif // BASEBALL_CPP_RECORDS_HPP
