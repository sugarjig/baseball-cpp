#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Simulator.hpp"
#include "EventSource.hpp"
#include "SimulatorObserver.hpp"
#include "Game.hpp"

using ::testing::Return;
using ::testing::_;
using ::testing::Exactly;
using ::testing::InSequence;

class MockEventSource : public EventSource {
public:
    MOCK_METHOD(std::optional<Record>, Next, (), (override));
};

class MockSimulatorObserver : public SimulatorObserver {
public:
    MOCK_METHOD(void, OnPreEvent, (const GameState& state), (override));
    MOCK_METHOD(void, OnEvent, (const PlayInfo& event), (override));
    MOCK_METHOD(void, OnSubstitution, (const SubstitutionInfo& sub), (override));
    MOCK_METHOD(void, OnComment, (const std::string& comment), (override));
    MOCK_METHOD(void, OnRunnerAdjustment, (const RunnerAdjustmentInfo& radj), (override));
    MOCK_METHOD(void, OnPostEvent, (const GameState& state), (override));
};

class MockGame : public IGame {
public:
    MOCK_METHOD(void, AddEvent, (const PlayInfo& play), (override));
    MOCK_METHOD(void, AddSubstitution, (const SubstitutionInfo& sub), (override));
    MOCK_METHOD(void, AddComment, (std::string_view comment), (override));
    MOCK_METHOD(void, AddData, (const DataRecord& data), (override));
    MOCK_METHOD(void, AddRunnerAdjustment, (const RunnerAdjustmentInfo& radj), (override));
    MOCK_METHOD(void, UpdateState, (), (override));
    MOCK_METHOD(const GameState&, GetGameState, (), (const, override));
};

TEST(SimulatorTest, ProcessesPlayEvent) {
    MockEventSource mockSource;
    MockSimulatorObserver mockObserver;
    MockGame mockGame;
    Simulator simulator(&mockSource, &mockObserver);
    
    GameState dummyState;

    PlayInfo play;
    play.inning = 1;
    play.team = 0;
    play.batter = "p1";
    play.text = "K";

    Record record;
    record.type = RecordType::Play;
    record.data = play;

    {
        InSequence seq;
        EXPECT_CALL(mockSource, Next()).WillOnce(Return(record));
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPreEvent(_)).Times(1);
        EXPECT_CALL(mockGame, AddEvent(_)).Times(1);
        EXPECT_CALL(mockObserver, OnEvent(_)).Times(1);
        EXPECT_CALL(mockGame, UpdateState()).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPostEvent(_)).Times(1);
        EXPECT_CALL(mockSource, Next()).WillOnce(Return(std::nullopt));
    }

    simulator.SimulateGame(mockGame);
}

TEST(SimulatorTest, ProcessesSubstitutionEvent) {
    MockEventSource mockSource;
    MockSimulatorObserver mockObserver;
    MockGame mockGame;
    Simulator simulator(&mockSource, &mockObserver);
    
    GameState dummyState;

    SubstitutionInfo sub;
    sub.playerID = "testp002";
    sub.name = "Sub Player";
    sub.team = 0;
    sub.pos = 1;

    Record record;
    record.type = RecordType::Substitution;
    record.data = sub;

    {
        InSequence seq;
        EXPECT_CALL(mockSource, Next()).WillOnce(Return(record));
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPreEvent(_)).Times(1);
        EXPECT_CALL(mockGame, AddSubstitution(_)).Times(1);
        EXPECT_CALL(mockObserver, OnSubstitution(_)).Times(1);
        EXPECT_CALL(mockGame, UpdateState()).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPostEvent(_)).Times(1);
        EXPECT_CALL(mockSource, Next()).WillOnce(Return(std::nullopt));
    }

    simulator.SimulateGame(mockGame);
}

TEST(SimulatorTest, ProcessesCommentEvent) {
    MockEventSource mockSource;
    MockSimulatorObserver mockObserver;
    MockGame mockGame;
    Simulator simulator(&mockSource, &mockObserver);
    
    GameState dummyState;

    std::string comment = "Test Comment";

    Record record;
    record.type = RecordType::Comment;
    record.data = comment;

    {
        InSequence seq;
        EXPECT_CALL(mockSource, Next()).WillOnce(Return(record));
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPreEvent(_)).Times(1);
        EXPECT_CALL(mockGame, AddComment(_)).Times(1);
        EXPECT_CALL(mockObserver, OnComment(comment)).Times(1);
        EXPECT_CALL(mockGame, UpdateState()).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPostEvent(_)).Times(1);
        EXPECT_CALL(mockSource, Next()).WillOnce(Return(std::nullopt));
    }

    simulator.SimulateGame(mockGame);
}

TEST(SimulatorTest, ProcessesRunnerAdjustmentEvent) {
    MockEventSource mockSource;
    MockSimulatorObserver mockObserver;
    MockGame mockGame;
    Simulator simulator(&mockSource, &mockObserver);
    
    GameState dummyState;

    RunnerAdjustmentInfo radj;
    radj.playerID = "testp001";
    radj.base = 2;

    Record record;
    record.type = RecordType::RunnerAdjustment;
    record.data = radj;

    {
        InSequence seq;
        EXPECT_CALL(mockSource, Next()).WillOnce(Return(record));
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPreEvent(_)).Times(1);
        EXPECT_CALL(mockGame, AddRunnerAdjustment(_)).Times(1);
        EXPECT_CALL(mockObserver, OnRunnerAdjustment(_)).Times(1);
        EXPECT_CALL(mockGame, UpdateState()).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPostEvent(_)).Times(1);
        EXPECT_CALL(mockSource, Next()).WillOnce(Return(std::nullopt));
    }

    simulator.SimulateGame(mockGame);
}
