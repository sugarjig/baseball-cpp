#include "MatrixLoader.hpp"
#include "MatrixData.hpp"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {
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
        tokens.emplace_back("");
    }
    return tokens;
}

auto ParseOutcome(const std::vector<std::string>& values, const std::map<std::string, size_t>& headerMap)
    -> MatrixOutcome {
    MatrixOutcome outcome;
    if (headerMap.contains("Play")) {
        outcome.play = values.at(headerMap.at("Play"));
    }
    if (headerMap.contains("Type")) {
        outcome.type = values.at(headerMap.at("Type"));
    }
    if (headerMap.contains("Bnt")) {
        outcome.bnt = values.at(headerMap.at("Bnt"));
    }
    if (headerMap.contains("Fl")) {
        outcome.fl = values.at(headerMap.at("Fl"));
    }
    if (headerMap.contains("Loc")) {
        outcome.loc = values.at(headerMap.at("Loc"));
    }
    if (headerMap.contains("B")) {
        outcome.batterBase = values.at(headerMap.at("B"));
    }
    if (headerMap.contains("R1")) {
        outcome.r1Base = values.at(headerMap.at("R1"));
    }
    if (headerMap.contains("R2")) {
        outcome.r2Base = values.at(headerMap.at("R2"));
    }
    if (headerMap.contains("R3")) {
        outcome.r3Base = values.at(headerMap.at("R3"));
    }

    if (headerMap.contains("Count")) {
        try {
            outcome.weight = std::stoi(values.at(headerMap.at("Count")));
        } catch (...) {
            outcome.weight = 0;
        }
    }
    if (headerMap.contains("New Outs")) {
        try {
            outcome.newOuts = std::stoi(values.at(headerMap.at("New Outs")));
        } catch (...) {
            outcome.newOuts = 0;
        }
    }
    if (headerMap.contains("Runs")) {
        try {
            outcome.runs = std::stoi(values.at(headerMap.at("Runs")));
        } catch (...) {
            outcome.runs = 0;
        }
    }
    return outcome;
}

void LoadMatrixFile(const std::filesystem::path& filePath, std::vector<MatrixOutcome>& outcomes,
                    std::vector<double>& weights) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    if (!std::getline(file, line)) {
        return;
    }

    auto headers = Split(line, ',');
    std::map<std::string, size_t> headerMap;
    for (size_t i = 0; i < headers.size(); ++i) {
        headerMap[headers.at(i)] = i;
    }

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        auto values = Split(line, ',');
        if (values.size() < headers.size()) {
            continue;
        }

        MatrixOutcome const outcome = ParseOutcome(values, headerMap);

        if (outcome.weight > 0) {
            outcomes.push_back(outcome);
            weights.push_back(static_cast<double>(outcome.weight));
        }
    }
}
} // namespace

auto MatrixLoader::LoadMatrices(const std::filesystem::path& dataDir) -> MatrixData {
    MatrixData data;
    const std::vector<std::string> baseConfigs = {
        "Empty", "1st", "2nd", "3rd", "1st_and_2nd", "1st_and_3rd", "2nd_and_3rd", "Loaded",
    };

    for (int currentOuts = 0; currentOuts <= 2; ++currentOuts) {
        for (const auto& config : baseConfigs) {
            std::string const key = std::to_string(currentOuts) + "_out_" + config;
            std::filesystem::path const filePath = dataDir / (key + ".csv");

            std::vector<MatrixOutcome> outcomes;
            std::vector<double> weights;

            LoadMatrixFile(filePath, outcomes, weights);

            if (!outcomes.empty()) {
                data.matrices[key] = std::move(outcomes);
                data.distributions[key] = std::discrete_distribution<size_t>(weights.begin(), weights.end());
            }
        }
    }
    return data;
}
