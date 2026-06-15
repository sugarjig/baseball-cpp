#include "EventSource.hpp"
#include "Records.hpp"
#include "Simulator.hpp"
#include "StaticEventSource.hpp"
#include "chadwick/Game.hpp"
#include "chadwick/Scorebook.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

namespace {
auto GetFixtureFiles() -> std::vector<std::string> {
#ifdef PROJECT_ROOT
    fs::path const fixturesDir = fs::path(PROJECT_ROOT) / "tests" / "integration" / "fixtures";
#else
    fs::path fixturesDir = fs::path(".") / "tests" / "integration" / "fixtures";
#endif
    std::vector<std::string> files;

    if (!fs::exists(fixturesDir)) {
        return files;
    }

    for (const auto& entry : fs::recursive_directory_iterator(fixturesDir)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            std::ranges::transform(ext, ext.begin(), toupper);

            if (ext == ".EVA" || ext == ".EVN" || ext == ".EVE") {
                files.push_back(entry.path().string());
            }
        }
    }
    std::ranges::sort(files);
    return files;
}

// Simple CSV parser for Retrosheet records
auto ParseCsvLine(const std::string& line) -> std::vector<std::string> {
    std::vector<std::string> fields;
    std::string currentField;
    bool inQuotes = false;
    for (char const character : line) {
        if (character == ',' && !inQuotes) {
            fields.push_back(currentField);
            currentField.clear();
        } else if (character == '"') {
            inQuotes = !inQuotes;
            currentField += character;
        } else {
            currentField += character;
        }
    }
    fields.push_back(currentField);

    for (auto& field : fields) {
        if (field.size() >= 2 && field.front() == '"' && field.back() == '"') {
            field = field.substr(1, field.size() - 2);
        }
    }

    return fields;
}

auto ReadFileLines(const std::string& path) -> std::vector<std::string> {
    std::vector<std::string> lines;
    std::ifstream file(path);
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

void NormalizeRetrosheetFile(const std::string& inputPath, const std::string& outputPath) {
    chadwick::Scorebook const scorebook;
    int const gamesRead = scorebook.Read(inputPath);
    ASSERT_GE(gamesRead, 0) << "Failed to read scorebook from " << inputPath;
    ASSERT_TRUE(scorebook.Write(outputPath)) << "Failed to write scorebook to " << outputPath;
}

class SimulatorTest : public testing::TestWithParam<std::string> {};
} // namespace

TEST_P(SimulatorTest, FullGameSimulation) {
    std::string const& inputPath = GetParam();
    fs::path const path(inputPath);

    fs::path const tempDir = fs::temp_directory_path();
    fs::path const normalizedPath = tempDir / ("normalized_" + path.filename().string());
    fs::path const outputPath = tempDir / ("output_" + path.filename().string());

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

    auto processGame = [&]() -> void {
        if (gameId.empty()) {
            return;
        }
        chadwick::Game game(gameId, version, infoRecords, starters);
        StaticEventSource eventSource(events);
        Simulator const simulator(&eventSource);
        simulator.SimulateGame(game);

        for (const auto& data : dataRecords) {
            game.AddData(data);
        }
        scorebook.AddGame(std::move(game));
    };

    while (std::getline(inputFile, line)) {
        auto fields = ParseCsvLine(line);
        if (fields.empty()) {
            continue;
        }

        const std::string& type = fields.at(0);
        if (type == "id") {
            processGame();
            gameId = fields.at(1);
            version.clear();
            infoRecords.clear();
            starters.clear();
            events.clear();
            dataRecords.clear();
        } else if (type == "version") {
            version = fields.at(1);
        } else if (type == "info") {
            infoRecords.push_back({.key = fields.at(1), .value = fields.at(2)});
        } else if (type == "start") {
            StarterInfo starter;
            starter.id = fields.at(1);
            starter.name = fields.at(2);
            starter.isHome = fields.at(3) == "1";
            starter.battingOrder = std::stoi(fields.at(4));
            starter.position =
                std::stoi(fields.at(5)); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            starters.push_back(starter);
        } else if (type == "play") {
            PlayInfo play;
            play.inning = std::stoi(fields.at(1));
            play.team = std::stoi(fields.at(2));
            play.batter = fields.at(3);
            play.pitchCount = fields.at(4);
            play.pitchSequence =
                fields.at(5);         // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            play.text = fields.at(6); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            events.push_back({.type = RecordType::Play, .data = play});
        } else if (type == "sub") {
            SubstitutionInfo sub;
            sub.playerId = fields.at(1);
            sub.name = fields.at(2);
            sub.team = std::stoi(fields.at(3));
            sub.slot = std::stoi(fields.at(4));
            sub.pos =
                std::stoi(fields.at(5)); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            events.push_back({.type = RecordType::Substitution, .data = sub});
        } else if (type == "com") {
            events.push_back({.type = RecordType::Comment, .data = fields.at(1)});
        } else if (type == "radj") {
            RunnerAdjustmentInfo radj;
            radj.playerId = fields.at(1);
            radj.base = std::stoi(fields.at(2));
            events.push_back({.type = RecordType::RunnerAdjustment, .data = radj});
        } else if (type == "badj") {
            BatterAdjustmentInfo badj;
            badj.playerId = fields.at(1);
            badj.hand = fields.at(2).at(0);
            events.push_back({.type = RecordType::BatterAdjustment, .data = badj});
        } else if (type == "padj") {
            PitcherAdjustmentInfo padj;
            padj.playerId = fields.at(1);
            padj.hand = fields.at(2).at(0);
            events.push_back({.type = RecordType::PitcherAdjustment, .data = padj});
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

INSTANTIATE_TEST_SUITE_P(FixtureTests, SimulatorTest, testing::ValuesIn(GetFixtureFiles()));
