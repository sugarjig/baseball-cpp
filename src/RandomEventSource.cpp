#include "RandomEventSource.hpp"
#include "EventSource.hpp"
#include "IGameState.hpp"
#include "Records.hpp"
#include <optional>

RandomEventSource::RandomEventSource(unsigned int seed) : rng(seed), dist(0.0, 1.0) {}

auto RandomEventSource::Next(const IGameState& state) -> std::optional<Record> {
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

    int const visitorScore = state.GetScore(0);
    int const homeScore = state.GetScore(1);

    // If it's the bottom of the 9th inning or later, and the home team is ahead, the game is over.
    if (inning >= numInningsInGame && team == 1 && homeScore > visitorScore) {
        return std::nullopt;
    }

    // If it's the end of the 9th or later (about to start top of next inning), and there is not a tie, the game is
    // over.
    if (inning > numInningsInGame && team == 0 && visitorScore != homeScore) {
        return std::nullopt;
    }

    PlayInfo play;
    play.inning = inning;
    play.team = team;
    play.batter = state.GetNextBatter(team);
    play.pitchCount = "";
    play.pitchSequence = "";

    double const roll = dist(rng);
    constexpr double chanceOfStrikeout = 0.9;
    if (roll < chanceOfStrikeout) {
        play.text = "K";
    } else {
        play.text = "HR";
    }

    Record record;
    record.type = RecordType::Play;
    record.data = play;
    return record;
}
