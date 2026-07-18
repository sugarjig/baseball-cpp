#include "chadwick/GameState.hpp"
#include <gtest/gtest.h>
#include <string>

extern "C" {
// clang-format off
 // ReSharper disable once CppUnusedIncludeDirective
#include "parse.h" // NOLINT(misc-include-cleaner)
#include "gameiter.h"
// clang-format on
}

namespace {

struct KeepPlayingParams {
    std::string label;
    int inning;
    int battingTeam;
    int outs;
    int visitorScore;
    int homeScore;
    bool expected;
};

class GameStateKeepPlayingTest : public ::testing::TestWithParam<KeepPlayingParams> {
protected:
    CWGameState state{}; // NOLINT(*-non-private-member-variables-in-classes)

    void SetUp() override { cw_gamestate_initialize(&state); }

    void TearDown() override { cw_gamestate_cleanup(&state); }
};

TEST_P(GameStateKeepPlayingTest, KeepPlaying) {
    const auto& params = GetParam();
    state.inning = params.inning;
    state.batting_team = params.battingTeam;
    state.outs = params.outs;
    state.score[0] = params.visitorScore;
    state.score[1] = params.homeScore;

    chadwick::GameState const gameState(&state);
    EXPECT_EQ(gameState.KeepPlaying(), params.expected) << "Failed for: " << params.label;
}

INSTANTIATE_TEST_SUITE_P(GameStateTests, GameStateKeepPlayingTest,
                         ::testing::Values(KeepPlayingParams{"BeginningOfTopOf1st", 1, 0, 0, 0, 0, true},
                                           KeepPlayingParams{"MiddleOfTop1stAwayLeading", 1, 0, 0, 2, 1, true},
                                           KeepPlayingParams{"EndOfTop1stHomeLeading", 1, 0, 3, 0, 1, true},
                                           KeepPlayingParams{"EndOfTop1stVisitorLeading", 1, 0, 3, 2, 1, true},
                                           KeepPlayingParams{"EndOfTop1stTied", 1, 0, 3, 1, 1, true},
                                           KeepPlayingParams{"MiddleOfBottom1stHomeLeading", 1, 1, 0, 1, 2, true},
                                           KeepPlayingParams{"EndOfBottom1stVisitorLeading", 1, 1, 3, 3, 1, true},
                                           KeepPlayingParams{"EndOfBottom1stTied", 1, 1, 3, 1, 1, true},

                                           KeepPlayingParams{"BeginningOfTop8thTied", 8, 0, 0, 1, 1, true},
                                           KeepPlayingParams{"MiddleOfTop8thAwayLeading", 8, 0, 0, 2, 1, true},
                                           KeepPlayingParams{"EndOfTop8thHomeLeading", 8, 0, 3, 0, 1, true},
                                           KeepPlayingParams{"EndOfTop8thVisitorLeading", 8, 0, 3, 2, 1, true},
                                           KeepPlayingParams{"EndOfTop8thTied", 8, 0, 3, 1, 1, true},
                                           KeepPlayingParams{"MiddleOfBottom8thHomeLeading", 8, 1, 0, 1, 2, true},
                                           KeepPlayingParams{"EndOfBottom8thVisitorLeading", 8, 1, 3, 3, 1, true},
                                           KeepPlayingParams{"EndOfBottom8thTied", 8, 1, 3, 1, 1, true},

                                           KeepPlayingParams{"BeginningOfTop9thTied", 9, 0, 0, 1, 1, true},
                                           KeepPlayingParams{"MiddleOfTop9thAwayLeading", 9, 0, 0, 2, 1, true},
                                           KeepPlayingParams{"EndOfTop9thHomeLeading", 9, 0, 3, 0, 1, false},
                                           KeepPlayingParams{"EndOfTop9thVisitorLeading", 9, 0, 3, 2, 1, true},
                                           KeepPlayingParams{"EndOfTop9thTied", 9, 0, 3, 1, 1, true},
                                           KeepPlayingParams{"MiddleOfBottom9thHomeLeading", 9, 1, 0, 1, 2, false},
                                           KeepPlayingParams{"EndOfBottom9thVisitorLeading", 9, 1, 3, 3, 1, false},
                                           KeepPlayingParams{"EndOfBottom9thTied", 9, 1, 3, 1, 1, true},

                                           KeepPlayingParams{"BeginningOfTop10thTied", 10, 0, 0, 1, 1, true},
                                           KeepPlayingParams{"MiddleOfTop10thAwayLeading", 10, 0, 0, 2, 1, true},
                                           KeepPlayingParams{"EndOfTop10thHomeLeading", 10, 0, 3, 0, 1, false},
                                           KeepPlayingParams{"EndOfTop10thVisitorLeading", 10, 0, 3, 2, 1, true},
                                           KeepPlayingParams{"EndOfTop10thTied", 10, 0, 3, 1, 1, true},
                                           KeepPlayingParams{"MiddleOfBottom10thHomeLeading", 10, 1, 0, 1, 2, false},
                                           KeepPlayingParams{"EndOfBottom10thVisitorLeading", 10, 1, 3, 3, 1, false},
                                           KeepPlayingParams{"EndOfBottom10thTied", 10, 1, 3, 1, 1, true}),
                         [](const ::testing::TestParamInfo<KeepPlayingParams>& info) -> std::string {
                             return info.param.label;
                         });

} // namespace
