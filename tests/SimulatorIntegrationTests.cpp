#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "Simulator.hpp"
#include "StaticEventSource.hpp"
#include "Game.hpp"
#include "Scorebook.hpp"

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

static std::vector<std::string> LoadAndNormalize(const std::string& path) {
    std::vector<std::string> lines;
    std::ifstream file(path);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        if (line.starts_with("info,")) {
            auto fields = ParseCsvLine(line);
            std::string normalized;
            for (size_t i = 0; i < fields.size(); ++i) {
                normalized += fields[i];
                if (i < fields.size() - 1) {
                    normalized += ",";
                }
            }
            lines.push_back(normalized);
        } else {
            lines.push_back(line);
        }
    }
    return lines;
}

TEST(SimulatorIntegrationTest, FullGameSimulation) {
#ifdef PROJECT_ROOT
    std::string projectRoot = PROJECT_ROOT;
#else
    std::string projectRoot = ".";
#endif
    std::string inputPath = projectRoot + "/tests/2025BAL.EVA";
    std::string outputPath = "output.EVA";

    std::ifstream inputFile(inputPath);
    ASSERT_TRUE(inputFile.is_open()) << "Could not open input file: " << inputPath;

    std::string line;
    std::string gameId;
    std::string version;
    std::vector<InfoRecord> infoRecords;
    std::vector<StarterInfo> starters;
    std::vector<Record> events;
    std::vector<DataRecord> dataRecords;

    Scorebook scorebook;

    auto processGame = [&]() {
        if (gameId.empty()) return;
        Game game(gameId, version, infoRecords, starters);
        StaticEventSource eventSource(events);
        Simulator simulator(&eventSource);
        simulator.SimulateGame(game);

        for (const auto& data : dataRecords) {
            game.AddData(data);
        }
        scorebook.AddGame(std::move(game));
    };

    while (std::getline(inputFile, line)) {
        if (line.empty()) continue;
        auto fields = ParseCsvLine(line);
        if (fields.empty()) continue;

        const std::string& type = fields[0];
        if (type == "id") {
            processGame();
            gameId = fields[1];
            version.clear();
            infoRecords.clear();
            starters.clear();
            events.clear();
            dataRecords.clear();
        } else if (type == "version") {
            version = fields[1];
        } else if (type == "info") {
            infoRecords.push_back({fields[1], fields[2]});
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
        } else if (type == "data") {
            DataRecord data;
            data.fields = std::vector<std::string>(fields.begin() + 1, fields.end());
            dataRecords.push_back(data);
        }
    }
    processGame();
    inputFile.close();

    ASSERT_TRUE(scorebook.Write(outputPath));

    // Compare files using normalization
    auto expected = LoadAndNormalize(inputPath);
    auto actual = LoadAndNormalize(outputPath);

    EXPECT_EQ(expected, actual);
}
