#include "MatrixLoader.hpp"
#include "MatrixData.hpp" // IWYU pragma: keep
#include <filesystem>
#include <fstream>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <vector>

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

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto MatrixLoader::LoadMatrices(const std::filesystem::path& dataDir) -> MatrixData {
    MatrixData data;
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
                data.matrices[key] = std::move(outcomes);
                data.distributions[key] = std::discrete_distribution<size_t>(weights.begin(), weights.end());
            }
        }
    }
    return data;
}
