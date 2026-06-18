#include "RandomEventSource.hpp"
#include "EventSource.hpp"
#include "IGameState.hpp"
#include "Records.hpp"
#include <optional>

namespace {
constexpr int runnerOn1st = 1;
constexpr int runnerOn2nd = 2;
constexpr int runnersOn1st2nd = 3;
constexpr int runnerOn3rd = 4;
constexpr int runnersOn1st3rd = 5;
constexpr int runnersOn2nd3rd = 6;
constexpr int basesLoaded = 7;
} // namespace

RandomEventSource::RandomEventSource(const unsigned int seed)
    : rng(static_cast<std::mt19937::result_type>(seed)), playDist({probabilityStrikeout, probabilityHomeRun}),
      outcomes({"K", "HR"}) {}

auto RandomEventSource::Next(const IGameState& state)
    -> std::optional<Record> { // NOLINT(readability-convert-member-functions-to-static)
    if (!state.KeepPlaying()) {
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

    PlayInfo play;
    play.inning = inning;
    play.team = team;
    play.batter = state.GetNextBatter(team);
    play.pitchCount = "";
    play.pitchSequence = "";

    int const outs = state.GetOuts();
    int const bases =
        (state.IsBaseOccupied(1) ? 1 : 0) | (state.IsBaseOccupied(2) ? 2 : 0) | (state.IsBaseOccupied(3) ? 4 : 0);

    auto getOutcome = [&](int /*stateId*/) -> std::string { return outcomes.at(playDist(rng)); };

    // Branch based on 24 possible states (3 out counts * 8 base configurations)
    // This structure allows for easy specialization of each state in the future.
    switch (outs) { // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    case 0:
        switch (bases) {
        case 0: play.text = getOutcome(0); break; // 0 outs, bases empty
        case runnerOn1st: play.text = getOutcome(1); break;
        case runnerOn2nd: play.text = getOutcome(2); break;
        case runnersOn1st2nd: play.text = getOutcome(3); break;
        case runnerOn3rd: play.text = getOutcome(4); break;
        case runnersOn1st3rd: play.text = getOutcome(5); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case runnersOn2nd3rd: play.text = getOutcome(6); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case basesLoaded: play.text = getOutcome(7); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        default: play.text = getOutcome(-1); break;
        }
        break;
    case 1:
        switch (bases) {
        case 0: play.text = getOutcome(8); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case runnerOn1st: play.text = getOutcome(9); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case runnerOn2nd: play.text = getOutcome(10); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case runnersOn1st2nd: play.text = getOutcome(11); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case runnerOn3rd: play.text = getOutcome(12); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case runnersOn1st3rd: play.text = getOutcome(13); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case runnersOn2nd3rd: play.text = getOutcome(14); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case basesLoaded: play.text = getOutcome(15); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        default: play.text = getOutcome(-2); break;
        }
        break;
    case 2:
        switch (bases) {
        case 0: play.text = getOutcome(16); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case runnerOn1st: play.text = getOutcome(17); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case runnerOn2nd: play.text = getOutcome(18); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case runnersOn1st2nd: play.text = getOutcome(19); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case runnerOn3rd: play.text = getOutcome(20); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case runnersOn1st3rd: play.text = getOutcome(21); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case runnersOn2nd3rd: play.text = getOutcome(22); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        case basesLoaded: play.text = getOutcome(23); break; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        default: play.text = getOutcome(-3); break;
        }
        break;
    default:
        play.text = getOutcome(-4);
        break;
    }

    Record record;
    record.type = RecordType::Play;
    record.data = play;
    return record;
}
