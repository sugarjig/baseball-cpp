#include "Simulator.hpp"
#include "StaticEventSource.hpp"
#include "chadwick/Game.hpp"
#include "chadwick/Scorebook.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static std::vector<std::string> GetFixtureFiles() {
#ifdef PROJECT_ROOT
    fs::path fixturesDir = fs::path(PROJECT_ROOT) / "tests" / "fixtures";
#else
    fs::path fixturesDir = fs::path(".") / "tests" / "fixtures";
#endif
    std::vector<std::string> files;

    if (!fs::exists(fixturesDir)) {
        return files;
    }

    for (const auto& entry : fs::directory_iterator(fixturesDir)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

            if (ext == ".EVA" || ext == ".EVN" || ext == ".EVE") {
                files.push_back(entry.path().string());
            }
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

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

static std::vector<std::string> ReadFileLines(const std::string& path) {
    std::vector<std::string> lines;
    std::ifstream file(path);
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

static void NormalizeRetrosheetFile(const std::string& inputPath, const std::string& outputPath) {
    chadwick::Scorebook scorebook;
    int gamesRead = scorebook.Read(inputPath);
    ASSERT_GE(gamesRead, 0) << "Failed to read scorebook from " << inputPath;
    ASSERT_TRUE(scorebook.Write(outputPath)) << "Failed to write scorebook to " << outputPath;
}

class SimulatorIntegrationTest : public testing::TestWithParam<std::string> {};

TEST_P(SimulatorIntegrationTest, FullGameSimulation) {
    std::string inputPath = GetParam();
    fs::path p(inputPath);

    fs::path tempDir = fs::temp_directory_path();
    fs::path normalizedPath = tempDir / ("normalized_" + p.filename().string());
    fs::path outputPath = tempDir / ("output_" + p.filename().string());

    ASSERT_NO_FATAL_FAILURE(NormalizeRetrosheetFile(inputPath, normalizedPath.string()));

    std::ifstream inputFile(normalizedPath);
    ASSERT_TRUE(inputFile.is_open()) << "Could not open normalized file: " << normalizedPath;

    std::string line;
    std::string gameId;
    std::string version;
    std::vector<InfoRecord> infoRecords;
    std::vector<StarterInfo> starters;
    std::vector<Record> events;
    std::vector<DataRecord> dataRecords;

    chadwick::Scorebook scorebook;

    auto processGame = [&]() {
        if (gameId.empty())
            return;
        chadwick::Game game(gameId, version, infoRecords, starters);
        StaticEventSource eventSource(events);
        Simulator simulator(&eventSource);
        simulator.SimulateGame(game);

        for (const auto& data : dataRecords) {
            game.AddData(data);
        }
        scorebook.AddGame(std::move(game));
    };

    while (std::getline(inputFile, line)) {
        auto fields = ParseCsvLine(line);
        if (fields.empty())
            continue;

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
        } else if (type == "radj") {
            RunnerAdjustmentInfo radj;
            radj.playerID = fields[1];
            radj.base = std::stoi(fields[2]);
            events.push_back({RecordType::RunnerAdjustment, radj});
        } else if (type == "badj") {
            BatterAdjustmentInfo badj;
            badj.playerID = fields[1];
            badj.hand = fields[2][0];
            events.push_back({RecordType::BatterAdjustment, badj});
        } else if (type == "padj") {
            PitcherAdjustmentInfo padj;
            padj.playerID = fields[1];
            padj.hand = fields[2][0];
            events.push_back({RecordType::PitcherAdjustment, padj});
        } else if (type == "data") {
            DataRecord data;
            data.fields = std::vector<std::string>(fields.begin() + 1, fields.end());
            dataRecords.push_back(data);
        }
    }
    processGame();
    inputFile.close();

    ASSERT_TRUE(scorebook.Write(outputPath.string()));

    // Compare files exactly
    auto expected = ReadFileLines(normalizedPath.string());
    auto actual = ReadFileLines(outputPath.string());

    EXPECT_EQ(expected, actual);

    // Cleanup
    fs::remove(normalizedPath);
    fs::remove(outputPath);
}

INSTANTIATE_TEST_SUITE_P(FixtureTests, SimulatorIntegrationTest, testing::ValuesIn(GetFixtureFiles()));
