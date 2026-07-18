#ifndef BASEBALL_CPP_RECORDS_HPP
#define BASEBALL_CPP_RECORDS_HPP

#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief Types of records that can be encountered in a baseball event stream.
 */
enum class RecordType : std::uint8_t {
    Play,              ///< A play event (e.g., hit, out).
    Substitution,      ///< A player substitution.
    Comment,           ///< A comment record.
    Starter,           ///< A starting lineup record.
    RunnerAdjustment,  ///< An adjustment to a runner's position.
    BatterAdjustment,  ///< An adjustment to the batter's hand.
    PitcherAdjustment, ///< An adjustment to a pitcher's hand.
};

/**
 * @brief Represents an 'info' record containing game metadata.
 */
struct InfoRecord {
    std::string key;   ///< The metadata key (e.g., "date", "visteam").
    std::string value; ///< The metadata value.
};

/**
 * @brief Represents a 'data' record, typically used for miscellaneous game data.
 */
struct DataRecord {
    std::vector<std::string> fields; ///< The fields of the data record.
};

/**
 * @brief Represents a 'play' record, describing an action on the field.
 */
struct PlayInfo {
    int inning = 0;            ///< The inning number.
    int team = 0;              ///< The team (0 for visitor, 1 for home).
    std::string batter;        ///< The player ID of the batter.
    std::string pitchCount;    ///< The ball/strike count at the time of the play.
    std::string pitchSequence; ///< The sequence of pitches in the plate appearance.
    std::string text;          ///< The Retrosheet play description.
};

/**
 * @brief Represents a 'sub' record, describing a player substitution.
 */
struct SubstitutionInfo {
    std::string playerId; ///< The player ID of the substitute.
    std::string name;     ///< The name of the player.
    int team = 0;         ///< The team (0 for visitor, 1 for home).
    int slot = 0;         ///< The batting order slot (1-9).
    int pos = 0;          ///< The defensive position (1-12).
};

/**
 * @brief Represents a 'start' record, describing an initial lineup entry.
 */
struct StarterInfo {
    std::string id;       ///< The player ID.
    std::string name;     ///< The name of the player.
    bool isHome = false;  ///< True if home team, false if visitor.
    int battingOrder = 0; ///< The batting order slot (1-9).
    int position = 0;     ///< The defensive position (1-12).
};

/**
 * @brief Represents a 'radj' record, describing a runner adjustment.
 */
struct RunnerAdjustmentInfo {
    std::string playerId; ///< The player ID of the runner.
    int base = 0;         ///< The base the runner is being moved to (1, 2, or 3).
};

/**
 * @brief Represents a 'badj' record, describing a batter adjustment.
 */
struct BatterAdjustmentInfo {
    std::string playerId; ///< The player ID of the batter.
    char hand = '\0';     ///< The hand the batter is using ('L', 'R', or 'S').
};

/**
 * @brief Represents a 'padj' record, describing a pitcher adjustment.
 */
struct PitcherAdjustmentInfo {
    std::string playerId; ///< The player ID of the pitcher.
    char hand = '\0';     ///< The hand the pitcher is using ('L' or 'R').
};

#endif // BASEBALL_CPP_RECORDS_HPP
