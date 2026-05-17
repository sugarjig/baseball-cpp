#include "EventSource.hpp"
#include "IGame.hpp"
#include "IGameState.hpp"
#include "Simulator.hpp"
#include "SimulatorObserver.hpp"
#include "chadwick/Game.hpp"
#include "chadwick/GameState.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Exactly;
using ::testing::InSequence;
using ::testing::Return;

class MockEventSource : public EventSource {
public:
    MOCK_METHOD(std::optional<Record>, Next, (), (override));
};

class MockSimulatorObserver : public SimulatorObserver {
public:
    MOCK_METHOD(void, OnPreEvent, (const IGameState& state), (override));
    MOCK_METHOD(void, OnEvent, (const PlayInfo& event), (override));
    MOCK_METHOD(void, OnSubstitution, (const SubstitutionInfo& sub), (override));
    MOCK_METHOD(void, OnComment, (const std::string& comment), (override));
    MOCK_METHOD(void, OnRunnerAdjustment, (const RunnerAdjustmentInfo& radj), (override));
    MOCK_METHOD(void, OnBatterAdjustment, (const BatterAdjustmentInfo& badj), (override));
    MOCK_METHOD(void, OnPitcherAdjustment, (const PitcherAdjustmentInfo& padj), (override));
    MOCK_METHOD(void, OnPostEvent, (const IGameState& state), (override));
};

class MockGame : public IGame {
public:
    MOCK_METHOD(void, AddEvent, (const PlayInfo& play), (override));
    MOCK_METHOD(void, AddSubstitution, (const SubstitutionInfo& sub), (override));
    MOCK_METHOD(void, AddComment, (std::string_view comment), (override));
    MOCK_METHOD(void, AddData, (const DataRecord& data), (override));
    MOCK_METHOD(void, AddRunnerAdjustment, (const RunnerAdjustmentInfo& radj), (override));
    MOCK_METHOD(void, AddBatterAdjustment, (const BatterAdjustmentInfo& badj), (override));
    MOCK_METHOD(void, AddPitcherAdjustment, (const PitcherAdjustmentInfo& padj), (override));
    MOCK_METHOD(void, UpdateState, (), (override));
    MOCK_METHOD(const IGameState&, GetGameState, (), (const, override));
};

TEST(SimulatorTest, ProcessesPlayEvent) {
    MockEventSource mockSource;
    MockSimulatorObserver mockObserver;
    MockGame mockGame;
    Simulator simulator(&mockSource, &mockObserver);

    chadwick::GameState dummyState;

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

    chadwick::GameState dummyState;

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

    chadwick::GameState dummyState;

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

    chadwick::GameState dummyState;

    RunnerAdjustmentInfo radj;
    radj.playerID = "testp001";
    radj.base = 2;

    Record record;
    record.type = RecordType::RunnerAdjustment;
    record.data = radj;

    simulator.SimulateGame(mockGame);
}

TEST(SimulatorTest, ProcessesBatterAdjustmentEvent) {
    MockEventSource mockSource;
    MockSimulatorObserver mockObserver;
    MockGame mockGame;
    Simulator simulator(&mockSource, &mockObserver);

    chadwick::GameState dummyState;

    BatterAdjustmentInfo badj;
    badj.playerID = "testp001";
    badj.hand = 'R';

    Record record;
    record.type = RecordType::BatterAdjustment;
    record.data = badj;

    {
        InSequence seq;
        EXPECT_CALL(mockSource, Next()).WillOnce(Return(record));
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPreEvent(_)).Times(1);
        EXPECT_CALL(mockGame, AddBatterAdjustment(_)).Times(1);
        EXPECT_CALL(mockObserver, OnBatterAdjustment(_)).Times(1);
        EXPECT_CALL(mockGame, UpdateState()).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPostEvent(_)).Times(1);
        EXPECT_CALL(mockSource, Next()).WillOnce(Return(std::nullopt));
    }

    simulator.SimulateGame(mockGame);
}
