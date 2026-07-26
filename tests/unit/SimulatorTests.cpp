#include "EventSource.hpp"
#include "IGame.hpp"
#include "IGameState.hpp"
#include "Records.hpp"
#include "Simulator.hpp"
#include "SimulatorObserver.hpp"
#include "chadwick/GameState.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>
#include <string>

using ::testing::_; // NOLINT(bugprone-reserved-identifier)
using ::testing::InSequence;
using ::testing::Return;

namespace {
class MockEventSource : public EventSource {
public:
    MOCK_METHOD(std::optional<Event>, Next, (const IGameState& state), // NOLINT(modernize-use-trailing-return-type)
                (override));
};

class MockSimulatorObserver : public SimulatorObserver {
public:
    MOCK_METHOD(void, OnPreEvent, (const IGameState& state), (override));
    MOCK_METHOD(void, OnEvent, (const Event& event), (override));
    MOCK_METHOD(void, OnPostEvent, (const IGameState& state), (override));
};

class MockGame : public IGame {
public:
    MOCK_METHOD(void, AddEvent, (const Event& event), (override));
    MOCK_METHOD(void, AddData, (const DataRecord& data), (override));
    MOCK_METHOD(const IGameState&, GetGameState, (), (const, override)); // NOLINT(modernize-use-trailing-return-type)
};
} // namespace

TEST(SimulatorTest, ProcessesEvent) {
    MockEventSource mockSource;
    MockSimulatorObserver mockObserver;
    MockGame mockGame;
    Simulator const simulator(&mockSource, &mockObserver);

    PlayInfo play;
    play.inning = 1;
    play.team = 0;
    play.batter = "p1";
    play.text = "K";

    Event event;
    event.type = EventType::Play;
    event.data = play;

    {
        chadwick::GameState dummyState;
        InSequence seq;

        // First iteration
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockSource, Next(_)).WillOnce(Return(event));
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPreEvent(_)).Times(1);
        EXPECT_CALL(mockObserver, OnEvent(_)).Times(1);
        EXPECT_CALL(mockGame, AddEvent(_)).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPostEvent(_)).Times(1);

        // Second iteration (End of stream)
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockSource, Next(_)).WillOnce(Return(std::nullopt));
    }

    simulator.SimulateGame(mockGame);
}