#ifndef BASEBALL_CPP_SIMULATOROBSERVER_HPP
#define BASEBALL_CPP_SIMULATOROBSERVER_HPP

#include "EventSource.hpp"
#include "IGameState.hpp"
#include <iostream>

class SimulatorObserver {
public:
    virtual ~SimulatorObserver() = default;

    virtual void OnPreEvent(const IGameState& state) {
        std::cout << "Before: (State: Out=" << state.GetOuts() << ", Inning=" << state.GetInning()
                  << ", Batting team=" << state.GetBattingTeam() << ", Score=" << state.GetScore(0) << "-"
                  << state.GetScore(1) << ")\n";
    }

    virtual void OnEvent(const PlayInfo& event) {
        std::cout << "Play: " << event.batter << " - " << event.text << "\n";
    }

    virtual void OnSubstitution(const SubstitutionInfo& sub) {
        std::cout << "Substitution: " << sub.name << " (" << sub.playerId << ") at pos " << sub.pos << "\n";
    }

    virtual void OnComment(const std::string& comment) { std::cout << "Comment: " << comment << "\n"; }

    virtual void OnRunnerAdjustment(const RunnerAdjustmentInfo& radj) {
        std::cout << "Runner Adjustment: " << radj.playerId << " to base " << radj.base << "\n";
    }

    virtual void OnBatterAdjustment(const BatterAdjustmentInfo& badj) {
        std::cout << "Batter Adjustment: " << badj.playerId << " to hand " << badj.hand << "\n";
    }

    virtual void OnPitcherAdjustment(const PitcherAdjustmentInfo& padj) {
        std::cout << "Pitcher Adjustment: " << padj.playerId << " to hand " << padj.hand << "\n";
    }

    virtual void OnPostEvent(const IGameState& state) {
        std::cout << "After: (State: Out=" << state.GetOuts() << ", Inning=" << state.GetInning()
                  << ", Batting team=" << state.GetBattingTeam() << ", Score=" << state.GetScore(0) << "-"
                  << state.GetScore(1) << ")\n\n";
    }
};

#endif // BASEBALL_CPP_SIMULATOROBSERVER_HPP
