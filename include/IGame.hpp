#ifndef BASEBALL_CPP_IGAME_HPP
#define BASEBALL_CPP_IGAME_HPP

#include "IGameState.hpp"
#include "Records.hpp"
#include <string_view>

class IGame {
public:
    virtual ~IGame() = default;
    virtual void AddEvent(const PlayInfo& play) = 0;
    virtual void AddSubstitution(const SubstitutionInfo& sub) = 0;
    virtual void AddComment(std::string_view comment) = 0;
    virtual void AddData(const DataRecord& data) = 0;
    virtual void AddRunnerAdjustment(const RunnerAdjustmentInfo& radj) = 0;
    virtual void AddBatterAdjustment(const BatterAdjustmentInfo& badj) = 0;
    virtual void AddPitcherAdjustment(const PitcherAdjustmentInfo& padj) = 0;
    virtual void UpdateState() = 0;
    [[nodiscard]] virtual auto GetGameState() const -> const IGameState& = 0;
};

#endif // BASEBALL_CPP_IGAME_HPP
