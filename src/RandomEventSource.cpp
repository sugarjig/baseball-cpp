#include "RandomEventSource.hpp"
#include "EventSource.hpp"
#include "IGameState.hpp"
#include "Records.hpp"
#include <optional>

RandomEventSource::RandomEventSource(const unsigned int seed)
    : rng(seed), playDist({probabilityStrikeout, probabilityHomeRun}), outcomes({"K", "HR"}) {}

auto RandomEventSource::Next(const IGameState& state) -> std::optional<Record> {
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

    play.text = outcomes.at(playDist(rng));

    Record record;
    record.type = RecordType::Play;
    record.data = play;
    return record;
}
