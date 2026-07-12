#include "IGameState.hpp"
#include "MatrixData.hpp" // IWYU pragma: keep
#include "MatrixEventSource.hpp"
#include "MatrixLoader.hpp" // IWYU pragma: keep
#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {
class MockGameState : public IGameState {
public:
    MOCK_METHOD(bool, KeepPlaying, (), (const, override));                  // NOLINT(*-use-trailing-return-type)
    MOCK_METHOD(int, GetInning, (), (const, override));                     // NOLINT(*-use-trailing-return-type)
    MOCK_METHOD(int, GetOuts, (), (const, override));                       // NOLINT(*-use-trailing-return-type)
    MOCK_METHOD(bool, IsBaseOccupied, (int base), (const, override));       // NOLINT(*-use-trailing-return-type)
    MOCK_METHOD(int, GetBattingTeam, (), (const, override));                // NOLINT(*-use-trailing-return-type)
    MOCK_METHOD(std::string, GetNextBatter, (int team), (const, override)); // NOLINT(*-use-trailing-return-type)
    MOCK_METHOD(int, GetScore, (int team), (const, override));              // NOLINT(*-use-trailing-return-type)
};

constexpr unsigned int seed123 = 123;
constexpr unsigned int seed456 = 456;
constexpr unsigned int seed789 = 789;

auto GetTestData() -> MatrixData {
    std::filesystem::path const dataDir = std::filesystem::path(PROJECT_ROOT) / "data" / "matrices_2025";
    return MatrixLoader::LoadMatrices(dataDir);
}
} // namespace

TEST(MatrixLoaderTest, LoadsDataFromDirectory) {
    std::filesystem::path const dataDir = std::filesystem::path(PROJECT_ROOT) / "data" / "matrices_2025";
    auto data = MatrixLoader::LoadMatrices(dataDir);
    EXPECT_FALSE(data.matrices.empty());
    EXPECT_FALSE(data.distributions.empty());
    EXPECT_EQ(data.matrices.size(), data.distributions.size());
}

TEST(MatrixEventSourceTest, LoadsMatricesAndGeneratesRecord) {
    MatrixEventSource source(GetTestData(), seed123); // NOLINT(misc-const-correctness)
    MockGameState const state;

    EXPECT_CALL(state, KeepPlaying()).WillOnce(testing::Return(true));
    EXPECT_CALL(state, GetOuts()).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(state, IsBaseOccupied(testing::_)).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(state, GetInning()).WillRepeatedly(testing::Return(1));
    EXPECT_CALL(state, GetBattingTeam()).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(state, GetNextBatter(0)).WillRepeatedly(testing::Return("PLAYER1"));

    auto record = source.Next(state);
    ASSERT_TRUE(record.has_value());
    EXPECT_EQ(record->type, RecordType::Play);

    auto play = std::get<PlayInfo>(record->data);
    EXPECT_EQ(play.inning, 1);
    EXPECT_EQ(play.team, 0);
    EXPECT_EQ(play.batter, "PLAYER1");
    EXPECT_FALSE(play.text.empty());
}

TEST(MatrixEventSourceTest, HandlesBasesLoaded) {
    MatrixEventSource source(GetTestData(), seed456); // NOLINT(misc-const-correctness)
    MockGameState const state;

    EXPECT_CALL(state, KeepPlaying()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(state, GetOuts()).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(state, IsBaseOccupied(1)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(state, IsBaseOccupied(2)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(state, IsBaseOccupied(3)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(state, GetInning()).WillRepeatedly(testing::Return(1));
    EXPECT_CALL(state, GetBattingTeam()).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(state, GetNextBatter(0)).WillRepeatedly(testing::Return("PLAYER1"));

    auto record = source.Next(state);
    ASSERT_TRUE(record.has_value());
    EXPECT_EQ(record->type, RecordType::Play);
}

TEST(MatrixEventSourceTest, ReturnsNulloptWhenGameEnds) {
    MatrixEventSource source(GetTestData()); // NOLINT(misc-const-correctness)
    MockGameState const state;

    EXPECT_CALL(state, KeepPlaying()).WillOnce(testing::Return(false));

    auto record = source.Next(state);
    EXPECT_FALSE(record.has_value());
}

TEST(MatrixEventSourceTest, HandlesHalfInningTransition) {
    MatrixEventSource source(GetTestData(), seed789); // NOLINT(misc-const-correctness)
    MockGameState const state;

    // Simulate state after 3rd out of top 1st
    EXPECT_CALL(state, KeepPlaying()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(state, GetOuts()).WillRepeatedly(testing::Return(3));
    EXPECT_CALL(state, IsBaseOccupied(testing::_)).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(state, GetInning()).WillRepeatedly(testing::Return(1));
    EXPECT_CALL(state, GetBattingTeam()).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(state, GetNextBatter(1)).WillRepeatedly(testing::Return("HOME_PLAYER1"));

    auto record = source.Next(state);
    ASSERT_TRUE(record.has_value());
    auto play = std::get<PlayInfo>(record->data);

    // Should have flipped to home team (1)
    EXPECT_EQ(play.team, 1);
    EXPECT_EQ(play.inning, 1);
    EXPECT_EQ(play.batter, "HOME_PLAYER1");
}
