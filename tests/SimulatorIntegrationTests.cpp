#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "Simulator.hpp"
#include "StaticEventSource.hpp"
#include "Game.hpp"

// Simple CSV parser for Retrosheet records
static std::vector<std::string> ParseCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string currentField;
    bool inQuotes = false;
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(currentField);
            currentField.clear();
        } else {
            currentField += c;
        }
    }
    fields.push_back(currentField);
    return fields;
}

struct NormalizedGame {
    std::vector<std::string> id;
    std::vector<std::string> version;
    std::vector<std::vector<std::string>> info;
    std::vector<std::vector<std::string>> start;
    std::vector<std::vector<std::string>> events;
    std::vector<std::vector<std::string>> data;
};

static NormalizedGame LoadAndNormalize(const std::string& path) {
    NormalizedGame ng;
    std::ifstream file(path);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto fields = ParseCsvLine(line);
        if (fields.empty()) continue;

        const std::string& type = fields[0];

        if (type == "id") {
            ng.id = fields;
        } else if (type == "version") {
            ng.version = fields;
        } else if (type == "info") {
            ng.info.push_back(fields);
        } else if (type == "start") {
            ng.start.push_back(fields);
        } else if (type == "play" || type == "sub" || type == "com") {
            ng.events.push_back(fields);
        } else if (type == "data") {
            ng.data.push_back(fields);
        }
    }
    std::sort(ng.info.begin(), ng.info.end());
    return ng;
}

TEST(SimulatorIntegrationTest, FullGameSimulation) {
#ifdef PROJECT_ROOT
    std::string projectRoot = PROJECT_ROOT;
#else
    std::string projectRoot = ".";
#endif
    std::string inputPath = projectRoot + "/tests/2025TEST.EVA";
    std::string outputPath = "output.EVA";

    std::ifstream inputFile(inputPath);
    ASSERT_TRUE(inputFile.is_open()) << "Could not open input file: " << inputPath;

    std::string line;
    std::string gameId;
    std::string date;
    std::string version;
    std::vector<std::pair<std::string, std::string>> infoRecords;
    std::vector<StarterInfo> starters;
    std::vector<Record> events;

    while (std::getline(inputFile, line)) {
        if (line.empty()) continue;
        auto fields = ParseCsvLine(line);
        if (fields.empty()) continue;

        const std::string& type = fields[0];
        if (type == "id") {
            gameId = fields[1];
        } else if (type == "version") {
            version = fields[1];
        } else if (type == "info") {
            infoRecords.push_back({fields[1], fields[2]});
            if (fields[1] == "date") {
                date = fields[2];
            }
        } else if (type == "start") {
            StarterInfo starter;
            starter.id = fields[1];
            starter.name = fields[2];
            starter.isHome = (fields[3] == "1");
            starter.battingOrder = std::stoi(fields[4]);
            starter.position = std::stoi(fields[5]);
            starters.push_back(starter);
        } else if (type == "play") {
            PlayInfo play;
            play.inning = std::stoi(fields[1]);
            play.team = std::stoi(fields[2]);
            play.batter = fields[3];
            play.pitchCount = fields[4];
            play.pitchSequence = fields[5];
            play.text = fields[6];
            events.push_back({RecordType::Play, play});
        } else if (type == "sub") {
            SubstitutionInfo sub;
            sub.playerID = fields[1];
            sub.name = fields[2];
            sub.team = std::stoi(fields[3]);
            sub.slot = std::stoi(fields[4]);
            sub.pos = std::stoi(fields[5]);
            events.push_back({RecordType::Substitution, sub});
        } else if (type == "com") {
            events.push_back({RecordType::Comment, fields[1]});
        }
    }
    inputFile.close();

    ASSERT_FALSE(gameId.empty()) << "Game ID not found in input file";
    ASSERT_FALSE(date.empty()) << "Date not found in input file";

    Game game(gameId, date, version);
    for (const auto& info : infoRecords) {
        if (info.first != "date") {
            game.AddInfo(info.first, info.second);
        }
    }
    for (const auto& starter : starters) {
        game.AddStarter(starter);
    }

    StaticEventSource eventSource(events);
    Simulator simulator(&eventSource);
    simulator.SimulateGame(game);

    ASSERT_TRUE(game.Write(outputPath));

    // Compare files using normalization
    auto expected = LoadAndNormalize(inputPath);
    auto actual = LoadAndNormalize(outputPath);

    EXPECT_EQ(expected.id, actual.id) << "Mismatch in ID record";
    EXPECT_EQ(expected.version, actual.version) << "Mismatch in version record";
    EXPECT_EQ(expected.info, actual.info) << "Mismatch in info records";
    EXPECT_EQ(expected.start, actual.start) << "Mismatch in start records";
    EXPECT_EQ(expected.events, actual.events) << "Mismatch in events (play/sub/com) records";
    EXPECT_EQ(expected.data, actual.data) << "Mismatch in data records";
}
