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

    Game game(gameId, date);
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

    // Compare files
    std::ifstream expectedFile(inputPath);
    std::ifstream actualFile(outputPath);
    std::vector<std::string> expectedLines;
    std::vector<std::string> actualLines;
    std::string l;
    while (std::getline(expectedFile, l)) if (!l.empty()) expectedLines.push_back(l);
    while (std::getline(actualFile, l)) if (!l.empty()) actualLines.push_back(l);

    // Simple line-by-line comparison
    size_t minLines = std::min(expectedLines.size(), actualLines.size());
    for (size_t i = 0; i < minLines; ++i) {
        EXPECT_EQ(expectedLines[i], actualLines[i]) << "Difference at line " << (i + 1);
    }
    EXPECT_EQ(expectedLines.size(), actualLines.size()) << "Files have different number of lines";
}
