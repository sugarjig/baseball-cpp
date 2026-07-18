#include "EventSource.hpp" // IWYU pragma: keep
#include "Records.hpp"     // IWYU pragma: keep
#include "Simulator.hpp"
#include "StaticEventSource.hpp"
#include "chadwick/Game.hpp"
#include "chadwick/Scorebook.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream> // IWYU pragma: keep
#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <vector> // IWYU pragma: keep

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

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void NormalizeRetrosheetFile(const std::string& inputPath, const std::string& outputPath) {
    chadwick::Scorebook const scorebook;
    int const gamesRead = scorebook.Read(inputPath);
    ASSERT_GE(gamesRead, 0) << "Failed to read scorebook from " << inputPath;
    ASSERT_TRUE(scorebook.Write(outputPath)) << "Failed to write scorebook to " << outputPath;
}

struct GameData {
    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    std::string gameId;
    std::string version;
    std::vector<InfoRecord> infoRecords;
    std::vector<StarterInfo> starters;
    std::vector<Record> events;
    std::vector<DataRecord> dataRecords;
    // NOLINTEND(misc-non-private-member-variables-in-classes)

    void Clear() {
        gameId.clear();
        version.clear();
        infoRecords.clear();
        starters.clear();
        events.clear();
        dataRecords.clear();
    }
};

void ProcessGame(const GameData& data, const chadwick::Scorebook& scorebook) {
    if (data.gameId.empty()) {
        return;
    }
    chadwick::Game game(data.gameId, data.version, data.infoRecords, data.starters);
    StaticEventSource eventSource(data.events);
    Simulator const simulator(&eventSource);
    simulator.SimulateGame(game);

    for (const auto& dataRecord : data.dataRecords) {
        game.AddData(dataRecord);
    }
    scorebook.AddGame(std::move(game));
}

void ParseRecord(const std::vector<std::string>& fields, GameData& data, const chadwick::Scorebook& scorebook) {
    const std::string& type = fields.at(0);
    if (type == "id") {
        ProcessGame(data, scorebook);
        data.Clear();
        data.gameId = fields.at(1);
    } else if (type == "version") {
        data.version = fields.at(1);
    } else if (type == "info") {
        data.infoRecords.push_back({.key = fields.at(1), .value = fields.at(2)});
    } else if (type == "start") {
        data.starters.push_back({
            .id = fields.at(1),
            .name = fields.at(2),
            .isHome = fields.at(3) == "1",
            .battingOrder = std::stoi(fields.at(4)),
            .position =
                std::stoi(fields.at(5)), // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        });
    } else if (type == "play") {
        data.events.push_back({
            .type = RecordType::Play,
            .data =
                PlayInfo{
                    .inning = std::stoi(fields.at(1)),
                    .team = std::stoi(fields.at(2)),
                    .batter = fields.at(3),
                    .pitchCount = fields.at(4),
                    .pitchSequence =
                        fields.at(5),     // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
                    .text = fields.at(6), // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
                },
        });
    } else if (type == "sub") {
        data.events.push_back({
            .type = RecordType::Substitution,
            .data =
                SubstitutionInfo{
                    .playerId = fields.at(1),
                    .name = fields.at(2),
                    .team = std::stoi(fields.at(3)),
                    .slot = std::stoi(fields.at(4)),
                    .pos = std::stoi(
                        fields.at(5)), // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
                },
        });
    } else if (type == "com") {
        data.events.push_back({.type = RecordType::Comment, .data = fields.at(1)});
    } else if (type == "radj") {
        data.events.push_back({
            .type = RecordType::RunnerAdjustment,
            .data = RunnerAdjustmentInfo{.playerId = fields.at(1), .base = std::stoi(fields.at(2))},
        });
    } else if (type == "badj") {
        data.events.push_back({
            .type = RecordType::BatterAdjustment,
            .data = BatterAdjustmentInfo{.playerId = fields.at(1), .hand = fields.at(2).at(0)},
        });
    } else if (type == "padj") {
        data.events.push_back({
            .type = RecordType::PitcherAdjustment,
            .data = PitcherAdjustmentInfo{.playerId = fields.at(1), .hand = fields.at(2).at(0)},
        });
    } else if (type == "data") {
        data.dataRecords.push_back({.fields = std::vector<std::string>(fields.begin() + 1, fields.end())});
    }
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
    GameData data;
    const chadwick::Scorebook scorebook;

    while (std::getline(inputFile, line)) {
        auto fields = ParseCsvLine(line);
        if (fields.empty()) {
            continue;
        }
        ParseRecord(fields, data, scorebook);
    }
    ProcessGame(data, scorebook);
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
