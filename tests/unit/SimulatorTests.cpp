#include "EventSource.hpp"
#include "IGame.hpp"
#include "IGameState.hpp" // IWYU pragma: keep
#include "Records.hpp"
#include "Simulator.hpp"
#include "SimulatorObserver.hpp"
#include "chadwick/GameState.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <string_view>

using ::testing::_; // NOLINT(bugprone-reserved-identifier)
using ::testing::InSequence;
using ::testing::Return;

namespace {
class MockEventSource : public EventSource {
public:
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(std::optional<Record>, Next, (const IGameState& state), (override));
};

class MockSimulatorObserver : public SimulatorObserver {
public:
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, OnPreEvent, (const IGameState& state), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, OnEvent, (const PlayInfo& event), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, OnSubstitution, (const SubstitutionInfo& sub), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, OnComment, (const std::string& comment), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, OnRunnerAdjustment, (const RunnerAdjustmentInfo& radj), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, OnBatterAdjustment, (const BatterAdjustmentInfo& badj), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, OnPitcherAdjustment, (const PitcherAdjustmentInfo& padj), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, OnPostEvent, (const IGameState& state), (override));
};

class MockGame : public IGame {
public:
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, AddEvent, (const PlayInfo& play), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, AddSubstitution, (const SubstitutionInfo& sub), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, AddComment, (std::string_view comment), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, AddData, (const DataRecord& data), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, AddRunnerAdjustment, (const RunnerAdjustmentInfo& radj), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, AddBatterAdjustment, (const BatterAdjustmentInfo& badj), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, AddPitcherAdjustment, (const PitcherAdjustmentInfo& padj), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(void, UpdateState, (), (override));
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MOCK_METHOD(const IGameState&, GetGameState, (), (const, override));
};
} // namespace

TEST(SimulatorTest, ProcessesPlayEvent) {
    MockEventSource mockSource;
    MockSimulatorObserver mockObserver;
    MockGame mockGame; // NOLINT(misc-const-correctness)
    Simulator const simulator(&mockSource, &mockObserver);

    PlayInfo play;
    play.inning = 1;
    play.team = 0;
    play.batter = "p1";
    play.text = "K";

    Record record;
    record.type = RecordType::Play;
    record.data = play;

    {
        const chadwick::GameState dummyState;
        InSequence seq;
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockSource, Next(_)).WillOnce(Return(record));
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPreEvent(_)).Times(1);
        EXPECT_CALL(mockGame, AddEvent(_)).Times(1);
        EXPECT_CALL(mockObserver, OnEvent(_)).Times(1);
        EXPECT_CALL(mockGame, UpdateState()).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPostEvent(_)).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockSource, Next(_)).WillOnce(Return(std::nullopt));
    }

    simulator.SimulateGame(mockGame);
}

TEST(SimulatorTest, ProcessesSubstitutionEvent) {
    MockEventSource mockSource;
    MockSimulatorObserver mockObserver;
    MockGame mockGame; // NOLINT(misc-const-correctness)
    Simulator const simulator(&mockSource, &mockObserver);

    SubstitutionInfo sub;
    sub.playerId = "testp002";
    sub.name = "Sub Player";
    sub.team = 0;
    sub.pos = 1;

    Record record;
    record.type = RecordType::Substitution;
    record.data = sub;

    {
        const chadwick::GameState dummyState;
        InSequence seq;
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockSource, Next(_)).WillOnce(Return(record));
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPreEvent(_)).Times(1);
        EXPECT_CALL(mockGame, AddSubstitution(_)).Times(1);
        EXPECT_CALL(mockObserver, OnSubstitution(_)).Times(1);
        EXPECT_CALL(mockGame, UpdateState()).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPostEvent(_)).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockSource, Next(_)).WillOnce(Return(std::nullopt));
    }

    simulator.SimulateGame(mockGame);
}

TEST(SimulatorTest, ProcessesCommentEvent) {
    MockEventSource mockSource;
    MockSimulatorObserver mockObserver;
    MockGame mockGame; // NOLINT(misc-const-correctness)
    Simulator const simulator(&mockSource, &mockObserver);

    const std::string comment = "Test Comment";

    Record record;
    record.type = RecordType::Comment;
    record.data = comment;

    {
        const chadwick::GameState dummyState;
        InSequence seq;
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockSource, Next(_)).WillOnce(Return(record));
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPreEvent(_)).Times(1);
        EXPECT_CALL(mockGame, AddComment(_)).Times(1);
        EXPECT_CALL(mockObserver, OnComment(comment)).Times(1);
        EXPECT_CALL(mockGame, UpdateState()).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPostEvent(_)).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockSource, Next(_)).WillOnce(Return(std::nullopt));
    }

    simulator.SimulateGame(mockGame);
}

TEST(SimulatorTest, ProcessesRunnerAdjustmentEvent) {
    MockEventSource mockSource;
    MockSimulatorObserver mockObserver;
    MockGame mockGame; // NOLINT(misc-const-correctness)
    Simulator const simulator(&mockSource, &mockObserver);

    RunnerAdjustmentInfo radj;
    radj.playerId = "testp001";
    radj.base = 2;

    Record record;
    record.type = RecordType::RunnerAdjustment;
    record.data = radj;

    {
        const chadwick::GameState dummyState;
        InSequence seq;
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockSource, Next(_)).WillOnce(Return(record));
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPreEvent(_)).Times(1);
        EXPECT_CALL(mockGame, AddRunnerAdjustment(_)).Times(1);
        EXPECT_CALL(mockObserver, OnRunnerAdjustment(_)).Times(1);
        EXPECT_CALL(mockGame, UpdateState()).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPostEvent(_)).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockSource, Next(_)).WillOnce(Return(std::nullopt));
    }

    simulator.SimulateGame(mockGame);
}

TEST(SimulatorTest, ProcessesBatterAdjustmentEvent) {
    MockEventSource mockSource;
    MockSimulatorObserver mockObserver;
    MockGame mockGame; // NOLINT(misc-const-correctness)
    Simulator const simulator(&mockSource, &mockObserver);

    BatterAdjustmentInfo badj;
    badj.playerId = "testp001";
    badj.hand = 'R';

    Record record;
    record.type = RecordType::BatterAdjustment;
    record.data = badj;

    {
        const chadwick::GameState dummyState;
        InSequence seq;
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockSource, Next(_)).WillOnce(Return(record));
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPreEvent(_)).Times(1);
        EXPECT_CALL(mockGame, AddBatterAdjustment(_)).Times(1);
        EXPECT_CALL(mockObserver, OnBatterAdjustment(_)).Times(1);
        EXPECT_CALL(mockGame, UpdateState()).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPostEvent(_)).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockSource, Next(_)).WillOnce(Return(std::nullopt));
    }

    simulator.SimulateGame(mockGame);
}

TEST(SimulatorTest, ProcessesPitcherAdjustmentEvent) {
    MockEventSource mockSource;
    MockSimulatorObserver mockObserver;
    MockGame mockGame; // NOLINT(misc-const-correctness)
    Simulator const simulator(&mockSource, &mockObserver);

    PitcherAdjustmentInfo padj;
    padj.playerId = "testp001";
    padj.hand = 'L';

    Record record;
    record.type = RecordType::PitcherAdjustment;
    record.data = padj;

    {
        const chadwick::GameState dummyState;
        InSequence seq;
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockSource, Next(_)).WillOnce(Return(record));
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPreEvent(_)).Times(1);
        EXPECT_CALL(mockGame, AddPitcherAdjustment(_)).Times(1);
        EXPECT_CALL(mockObserver, OnPitcherAdjustment(_)).Times(1);
        EXPECT_CALL(mockGame, UpdateState()).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockObserver, OnPostEvent(_)).Times(1);
        EXPECT_CALL(mockGame, GetGameState()).WillOnce(::testing::ReturnRef(dummyState));
        EXPECT_CALL(mockSource, Next(_)).WillOnce(Return(std::nullopt));
    }

    simulator.SimulateGame(mockGame);
}