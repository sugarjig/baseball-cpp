#include "MatrixEventSource.hpp"
#include "EventSource.hpp"
#include "IGameState.hpp"
#include "MatrixData.hpp"
#include "Records.hpp"
#include <cctype>
#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

MatrixEventSource::MatrixEventSource(MatrixData data, unsigned int seed)
    : data(std::move(data)), rng(static_cast<std::mt19937::result_type>(seed)) {}

auto MatrixEventSource::GetMatrixKey(const IGameState& state) -> std::string {
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
    auto const itMatrix = data.matrices.find(key);
    if (itMatrix == data.matrices.end()) {
        return std::nullopt;
    }

    size_t const sampledIndex = data.distributions.at(key)(rng);
    const auto& outcome = itMatrix->second.at(sampledIndex);

    PlayInfo const play{
        .inning = currentInning,
        .team = currentTeam,
        .batter = state.GetNextBatter(currentTeam),
        .pitchCount = "",
        .pitchSequence = "",
        .text = GenerateRetrosheetText(outcome, state),
    };

    return Record{.type = RecordType::Play, .data = play};
}

auto MatrixEventSource::TranslateBaseAction(const MatrixOutcome& outcome) -> std::string {
    static const std::map<std::string, std::string> simplePlays = {
        {"Strikeout", "K"}, {"Walk", "W"},          {"Single", "S"},      {"Double", "D"},       {"Triple", "T"},
        {"Home run", "HR"}, {"Hit by pitch", "HP"}, {"Wild pitch", "WP"}, {"Passed ball", "PB"}, {"Balk", "BK"},
    };

    auto itPlay = simplePlays.find(outcome.play);
    if (itPlay != simplePlays.end()) {
        return itPlay->second;
    }

    if (outcome.play == "Generic out") {
        return TranslateGenericOut(outcome.loc);
    }
    if (outcome.play == "Stolen base") {
        return std::string(TranslateStolenBase(outcome));
    }
    if (outcome.play == "Caught stealing") {
        return std::string(TranslateCaughtStealing(outcome));
    }
    if (outcome.play == "Pickoff") {
        return std::string(TranslatePickoff(outcome));
    }
    return outcome.play;
}

auto MatrixEventSource::TranslateGenericOut(const std::string& loc) -> std::string {
    std::string fielders;
    for (char const locationChar : loc) {
        if (std::isdigit(locationChar) != 0) {
            fielders += locationChar;
        } else if (fielders.empty()) {
            continue;
        } else {
            break;
        }
    }
    return fielders.empty() ? loc : fielders;
}

auto MatrixEventSource::TranslateStolenBase(const MatrixOutcome& outcome) -> std::string_view {
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

auto MatrixEventSource::TranslateCaughtStealing(const MatrixOutcome& outcome) -> std::string_view {
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

auto MatrixEventSource::TranslatePickoff(const MatrixOutcome& outcome) -> std::string_view {
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

auto MatrixEventSource::TranslateAdvancement(int base, const std::string& endBaseName) -> std::string {
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

auto MatrixEventSource::TranslateAdvancements(const MatrixOutcome& outcome, const IGameState& state) -> std::string {
    std::vector<std::string> ads;

    if (state.GetOuts() < 3) {
        if (state.IsBaseOccupied(1)) {
            std::string const adv = TranslateAdvancement(1, outcome.r1Base);
            if (!adv.empty()) {
                ads.push_back(adv);
            }
        }
        if (state.IsBaseOccupied(2)) {
            std::string const adv = TranslateAdvancement(2, outcome.r2Base);
            if (!adv.empty()) {
                ads.push_back(adv);
            }
        }
        if (state.IsBaseOccupied(3)) {
            std::string const adv = TranslateAdvancement(3, outcome.r3Base);
            if (!adv.empty()) {
                ads.push_back(adv);
            }
        }
    }

    std::string resultStr;
    if (!ads.empty()) {
        resultStr += '.';
        for (size_t i = 0; i < ads.size(); ++i) {
            resultStr += ads.at(i) + (i == ads.size() - 1 ? "" : ";");
        }
    }
    return resultStr;
}

auto MatrixEventSource::GenerateRetrosheetText(const MatrixOutcome& outcome, const IGameState& state) -> std::string {
    return TranslateBaseAction(outcome) + TranslateAdvancements(outcome, state);
}
