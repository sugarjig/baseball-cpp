#include "MatrixEventSource.hpp"
#include "EventSource.hpp"
#include "IGameState.hpp"
#include "Records.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace {
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
auto Split(const std::string& inputStr, char delimiter) -> std::vector<std::string> {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(inputStr);
    while (std::getline(tokenStream, token, delimiter)) {
        token.erase(0, token.find_first_not_of(" \t\r\n") == std::string::npos ? token.length()
                                                                               : token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") == std::string::npos ? 0 : token.find_last_not_of(" \t\r\n") + 1);
        tokens.push_back(token);
    }
    if (!inputStr.empty() && inputStr.back() == delimiter) {
        tokens.push_back("");
    }
    return tokens;
}
} // namespace

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
MatrixEventSource::MatrixEventSource(const std::filesystem::path& dataDir, unsigned int seed)
    : rng(static_cast<std::mt19937::result_type>(seed)) {
    LoadMatrices(dataDir);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void MatrixEventSource::LoadMatrices(const std::filesystem::path& dataDir) {
    const std::vector<std::string> baseConfigs = {"Empty",       "1st",         "2nd",         "3rd",
                                                  "1st_and_2nd", "1st_and_3rd", "2nd_and_3rd", "Loaded"};

    for (int currentOuts = 0; currentOuts <= 2; ++currentOuts) {
        for (const auto& config : baseConfigs) {
            std::string const key = std::to_string(currentOuts) + "_out_" + config;
            std::filesystem::path const filePath = dataDir / (key + ".csv");

            std::ifstream file(filePath);
            if (!file.is_open()) {
                continue;
            }

            std::string line;
            if (!std::getline(file, line)) {
                continue;
            }

            auto headers = Split(line, ',');
            std::map<std::string, int> headerMap;
            for (int i = 0; i < static_cast<int>(headers.size()); ++i) {
                headerMap[headers[i]] = i;
            }

            std::vector<MatrixOutcome> outcomes;
            std::vector<double> weights;

            while (std::getline(file, line)) {
                if (line.empty()) {
                    continue;
                }
                auto values = Split(line, ',');
                if (values.size() < headers.size()) {
                    continue;
                }

                MatrixOutcome outcome;
                if (headerMap.count("Play") != 0) {
                    outcome.play = values[headerMap["Play"]];
                }
                if (headerMap.count("Type") != 0) {
                    outcome.type = values[headerMap["Type"]];
                }
                if (headerMap.count("Bnt") != 0) {
                    outcome.bnt = values[headerMap["Bnt"]];
                }
                if (headerMap.count("Fl") != 0) {
                    outcome.fl = values[headerMap["Fl"]];
                }
                if (headerMap.count("Loc") != 0) {
                    outcome.loc = values[headerMap["Loc"]];
                }
                if (headerMap.count("B") != 0) {
                    outcome.batterBase = values[headerMap["B"]];
                }
                if (headerMap.count("R1") != 0) {
                    outcome.r1Base = values[headerMap["R1"]];
                }
                if (headerMap.count("R2") != 0) {
                    outcome.r2Base = values[headerMap["R2"]];
                }
                if (headerMap.count("R3") != 0) {
                    outcome.r3Base = values[headerMap["R3"]];
                }

                if (headerMap.count("Count") != 0) {
                    try {
                        outcome.weight = std::stoi(values[headerMap["Count"]]);
                    } catch (...) {
                        outcome.weight = 0;
                    }
                }
                if (headerMap.count("New Outs") != 0) {
                    try {
                        outcome.newOuts = std::stoi(values[headerMap["New Outs"]]);
                    } catch (...) {
                        outcome.newOuts = 0;
                    }
                }
                if (headerMap.count("Runs") != 0) {
                    try {
                        outcome.runs = std::stoi(values[headerMap["Runs"]]);
                    } catch (...) {
                        outcome.runs = 0;
                    }
                }

                if (outcome.weight > 0) {
                    outcomes.push_back(outcome);
                    weights.push_back(static_cast<double>(outcome.weight));
                }
            }

            if (!outcomes.empty()) {
                matrices[key] = std::move(outcomes);
                distributions[key] = std::discrete_distribution<size_t>(weights.begin(), weights.end());
            }
        }
    }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto MatrixEventSource::GetMatrixKey(const IGameState& state) const -> std::string {
    int currentOuts = state.GetOuts();
    bool const occupied1 = state.IsBaseOccupied(1);
    bool const occupied2 = state.IsBaseOccupied(2);
    bool const occupied3 = state.IsBaseOccupied(3);

    if (currentOuts >= 3) {
        currentOuts = 0;
        return "0_out_Empty";
    }

    std::string key = std::to_string(currentOuts) + "_out_";
    if (!occupied1 && !occupied2 && !occupied3) {
        key += "Empty";
    } else if (occupied1 && !occupied2 && !occupied3) {
        key += "1st";
    } else if (!occupied1 && occupied2 && !occupied3) {
        key += "2nd";
    } else if (!occupied1 && !occupied2 && occupied3) {
        key += "3rd";
    } else if (occupied1 && occupied2 && !occupied3) {
        key += "1st_and_2nd";
    } else if (occupied1 && !occupied2 && occupied3) {
        key += "1st_and_3rd";
    } else if (!occupied1 && occupied2 && occupied3) {
        key += "2nd_and_3rd";
    } else {
        key += "Loaded";
    }

    return key;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto MatrixEventSource::Next(const IGameState& state) -> std::optional<Record> {
    if (!state.KeepPlaying()) {
        return std::nullopt;
    }

    int currentInning = state.GetInning();
    int currentTeam = state.GetBattingTeam();
    int const currentOuts = state.GetOuts();

    if (currentOuts >= 3) {
        if (currentTeam == 0) {
            currentTeam = 1;
        } else {
            currentTeam = 0;
            currentInning++;
        }
    }

    std::string const key = GetMatrixKey(state);
    auto const itMatrix = matrices.find(key);
    if (itMatrix == matrices.end()) {
        return std::nullopt;
    }

    size_t const sampledIndex = distributions.at(key)(rng);
    const auto& outcome = itMatrix->second[sampledIndex];

    PlayInfo play;
    play.inning = currentInning;
    play.team = currentTeam;
    play.batter = state.GetNextBatter(currentTeam);
    play.pitchCount = "";
    play.pitchSequence = "";
    play.text = GenerateRetrosheetText(outcome, state);

    Record nextRecord;
    nextRecord.type = RecordType::Play;
    nextRecord.data = play;
    return nextRecord;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto MatrixEventSource::TranslateBaseAction(const MatrixOutcome& outcome) const -> std::string {
    static const std::map<std::string, std::string> simplePlays = {
        {"Strikeout", "K"}, {"Walk", "W"},          {"Single", "S"},      {"Double", "D"},       {"Triple", "T"},
        {"Home run", "HR"}, {"Hit by pitch", "HP"}, {"Wild pitch", "WP"}, {"Passed ball", "PB"}, {"Balk", "BK"}};

    auto itPlay = simplePlays.find(outcome.play);
    if (itPlay != simplePlays.end()) {
        return itPlay->second;
    }

    if (outcome.play == "Generic out") {
        std::string fielders;
        for (char c : outcome.loc) {
            if (std::isdigit(c) != 0) {
                fielders += c;
            } else if (fielders.empty()) {
                continue;
            } else {
                break;
            }
        }
        return fielders.empty() ? outcome.loc : fielders;
    }
    if (outcome.play == "Stolen base") {
        if (outcome.r3Base == "Score") {
            return "SBH";
        }
        if (outcome.r2Base == "3rd") {
            return "SB3";
        }
        if (outcome.r1Base == "2nd") {
            return "SB2";
        }
        return "SB";
    }
    if (outcome.play == "Caught stealing") {
        if (outcome.r3Base == "Out") {
            return "CSH";
        }
        if (outcome.r2Base == "Out") {
            return "CS3";
        }
        if (outcome.r1Base == "Out") {
            return "CS2";
        }
        return "CS";
    }
    if (outcome.play == "Pickoff") {
        if (outcome.r3Base == "Out") {
            return "PO3";
        }
        if (outcome.r2Base == "Out") {
            return "PO2";
        }
        if (outcome.r1Base == "Out") {
            return "PO1";
        }
        return "PO";
    }
    return outcome.play;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static,bugprone-easily-swappable-parameters)
auto MatrixEventSource::TranslateAdvancement(int base, const std::string& endBaseName) const -> std::string {
    if (endBaseName == "Out") {
        return std::to_string(base) + "X" + (base == 3 ? "H" : std::to_string(base + 1));
    }
    std::string endBase;
    if (endBaseName == "1st") {
        endBase = "1";
    } else if (endBaseName == "2nd") {
        endBase = "2";
    } else if (endBaseName == "3rd") {
        endBase = "3";
    } else if (endBaseName == "Score") {
        endBase = "H";
    }

    if (!endBase.empty() && endBase != std::to_string(base) && endBaseName != "Stays") {
        return std::to_string(base) + "-" + endBase;
    }
    return "";
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto MatrixEventSource::TranslateAdvancements(const MatrixOutcome& outcome, const IGameState& state) const
    -> std::string {
    std::vector<std::string> ads;

    if (state.GetOuts() < 3) {
        if (state.IsBaseOccupied(1)) {
            std::string adv = TranslateAdvancement(1, outcome.r1Base);
            if (!adv.empty()) {
                ads.push_back(adv);
            }
        }
        if (state.IsBaseOccupied(2)) {
            std::string adv = TranslateAdvancement(2, outcome.r2Base);
            if (!adv.empty()) {
                ads.push_back(adv);
            }
        }
        if (state.IsBaseOccupied(3)) {
            std::string adv = TranslateAdvancement(3, outcome.r3Base);
            if (!adv.empty()) {
                ads.push_back(adv);
            }
        }
    }

    std::string resultStr;
    if (!ads.empty()) {
        resultStr += ".";
        for (size_t i = 0; i < ads.size(); ++i) {
            resultStr += ads[i] + (i == ads.size() - 1 ? "" : ";");
        }
    }
    return resultStr;
}

auto MatrixEventSource::GenerateRetrosheetText(const MatrixOutcome& outcome, const IGameState& state) const
    -> std::string {
    return TranslateBaseAction(outcome) + TranslateAdvancements(outcome, state);
}
