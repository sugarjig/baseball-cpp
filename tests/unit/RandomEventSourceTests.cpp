#include "EventSource.hpp"
#include "IGameState.hpp"
#include "RandomEventSource.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

namespace {
class MockGameState : public IGameState {
public:
    MOCK_METHOD(bool, KeepPlaying, (), (const, override));                    // NOLINT(*-use-trailing-return-type)
    MOCK_METHOD(int, GetInning, (), (const, override));                       // NOLINT(*-use-trailing-return-type)
    MOCK_METHOD(int, GetOuts, (), (const, override));                         // NOLINT(*-use-trailing-return-type)
    MOCK_METHOD(HalfInning, GetUpcomingHalfInning, (), (const, override));    // NOLINT(*-use-trailing-return-type)
    MOCK_METHOD(bool, IsBaseOccupied, (int base), (const, override));         // NOLINT(*-use-trailing-return-type)
    MOCK_METHOD(int, GetBattingTeam, (), (const, override));                  // NOLINT(*-use-trailing-return-type)
    MOCK_METHOD(std::string, GetNextBatter, (int team), (const, override));   // NOLINT(*-use-trailing-return-type)
    MOCK_METHOD(int, GetScore, (int team), (const, override));                // NOLINT(*-use-trailing-return-type)
    MOCK_METHOD(std::string, GetRunnerOnBase, (int base), (const, override)); // NOLINT(*-use-trailing-return-type)
};
} // namespace

TEST(RandomEventSourceTest, GeneratesEventsUntilStopped) {
    constexpr int seed = 12345;
    RandomEventSource source(seed);
    MockGameState const state;

    EXPECT_CALL(state, KeepPlaying()).WillOnce(testing::Return(true)).WillOnce(testing::Return(false));

    EXPECT_CALL(state, GetUpcomingHalfInning()).WillRepeatedly(testing::Return(HalfInning{.inning = 1, .team = 0}));
    EXPECT_CALL(state, GetOuts()).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(state, IsBaseOccupied(testing::_)).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(state, GetNextBatter(testing::_)).WillRepeatedly(testing::Return("PLAYER1"));

    auto event1 = source.Next(state);
    auto event1HasValue = event1.has_value();
    ASSERT_TRUE(event1HasValue);
    if (event1HasValue) {
        EXPECT_EQ(event1->type, EventType::Play);
    }

    auto event2 = source.Next(state);
    EXPECT_FALSE(event2.has_value());
}

TEST(RandomEventSourceTest, HandlesHalfInningTransition) {
    constexpr int seed = 12345;
    RandomEventSource source(seed);
    MockGameState const state;

    EXPECT_CALL(state, KeepPlaying()).WillOnce(testing::Return(true));
    EXPECT_CALL(state, GetUpcomingHalfInning()).WillRepeatedly(testing::Return(HalfInning{.inning = 1, .team = 1}));
    EXPECT_CALL(state, GetOuts()).WillRepeatedly(testing::Return(3));
    EXPECT_CALL(state, IsBaseOccupied(testing::_)).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(state, GetNextBatter(1)).WillRepeatedly(testing::Return("HOME_PLAYER1"));

    auto event = source.Next(state);
    ASSERT_TRUE(event.has_value());
    if (event.has_value()) {
        auto play = std::get<PlayInfo>(event->data);
        EXPECT_EQ(play.team, 1);
        EXPECT_EQ(play.inning, 1);
        EXPECT_EQ(play.batter, "HOME_PLAYER1");
    }
}
