#include "RandomEventSource.hpp"
#include "EventSource.hpp" // IWYU pragma: keep
#include "IGameState.hpp"
#include "Records.hpp"
#include <optional>
#include <string>

namespace {
constexpr int runnerOn1st = 1;
constexpr int runnerOn2nd = 2;
constexpr int runnersOn1st2nd = 3;
constexpr int runnerOn3rd = 4;
constexpr int runnersOn1st3rd = 5;
constexpr int runnersOn2nd3rd = 6;
constexpr int basesLoaded = 7;
} // namespace

// NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
RandomEventSource::RandomEventSource(const unsigned int seed) : rng(static_cast<std::mt19937::result_type>(seed)) {}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto RandomEventSource::GetOutcome(int /*stateId*/) -> std::string { return outcomes.at(playDist(rng)); }

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void RandomEventSource::ApplyAdvancements(PlayInfo& play, unsigned int bases) {
    if (play.text == "K") {
        return;
    }
    std::string advancements;
    bool const isSingle = play.text == "S";

    if ((bases & 1U) != 0U) {
        advancements += (isSingle ? "1-2;" : "1-H;");
    }
    if ((bases & 2U) != 0U) {
        advancements += "2-H;";
    }
    if ((bases & 4U) != 0U) {
        advancements += "3-H;";
    }

    if (!advancements.empty()) {
        advancements.pop_back(); // Remove trailing ';'
        play.text += "." + advancements;
    }
}

auto RandomEventSource::Next(const IGameState& state) -> std::optional<Record> {
    if (outcomes.empty() || !state.KeepPlaying()) {
        return std::nullopt;
    }

    int inning = state.GetInning();
    int team = state.GetBattingTeam();

    // Check if half-inning is over
    if (state.GetOuts() >= 3) {
        if (team == 0) {
            team = 1;
        } else {
            team = 0;
            inning++;
        }
    }

    int const outs = state.GetOuts();
    unsigned int const bases =
        (state.IsBaseOccupied(1) ? 1U : 0U) | (state.IsBaseOccupied(2) ? 2U : 0U) | (state.IsBaseOccupied(3) ? 4U : 0U);

    std::string text;
    // Branch based on 24 possible states (3 out counts * 8 base configurations)
    // This structure allows for easy specialization of each state in the future.
    // NOLINTNEXTLINE(bugprone-branch-clone)
    switch (outs) {
    case 0:
        switch (bases) {
        case 0:
            text = GetOutcome(0);
            break; // 0 outs, bases empty
        case runnerOn1st:
            text = GetOutcome(1);
            break;
        case runnerOn2nd:
            text = GetOutcome(2);
            break;
        case runnersOn1st2nd:
            text = GetOutcome(3);
            break;
        case runnerOn3rd:
            text = GetOutcome(4);
            break;
        case runnersOn1st3rd:
            text = GetOutcome(5); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case runnersOn2nd3rd:
            text = GetOutcome(6); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case basesLoaded:
            text = GetOutcome(7); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        default:
            text = GetOutcome(-1);
            break;
        }
        break;
    case 1:
        switch (bases) {
        case 0:
            text = GetOutcome(8); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case runnerOn1st:
            text = GetOutcome(9); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case runnerOn2nd:
            text = GetOutcome(10); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case runnersOn1st2nd:
            text = GetOutcome(11); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case runnerOn3rd:
            text = GetOutcome(12); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case runnersOn1st3rd:
            text = GetOutcome(13); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case runnersOn2nd3rd:
            text = GetOutcome(14); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case basesLoaded:
            text = GetOutcome(15); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        default:
            text = GetOutcome(-2);
            break;
        }
        break;
    case 2:
        switch (bases) {
        case 0:
            text = GetOutcome(16); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case runnerOn1st:
            text = GetOutcome(17); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case runnerOn2nd:
            text = GetOutcome(18); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case runnersOn1st2nd:
            text = GetOutcome(19); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case runnerOn3rd:
            text = GetOutcome(20); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case runnersOn1st3rd:
            text = GetOutcome(21); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case runnersOn2nd3rd:
            text = GetOutcome(22); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        case basesLoaded:
            text = GetOutcome(23); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
            break;
        default:
            text = GetOutcome(-3);
            break;
        }
        break;
    default:
        text = GetOutcome(-4);
        break;
    }

    PlayInfo play{
        .inning = inning,
        .team = team,
        .batter = state.GetNextBatter(team),
        .pitchCount = "",
        .pitchSequence = "",
        .text = text,
    };

    if (bases != 0U) {
        ApplyAdvancements(play, bases);
    }

    return Record{.type = RecordType::Play, .data = play};
}
