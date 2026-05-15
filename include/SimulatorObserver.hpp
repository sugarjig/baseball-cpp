#ifndef BASEBALL_CPP_SIMULATOROBSERVER_HPP
#define BASEBALL_CPP_SIMULATOROBSERVER_HPP

#include <iostream>
#include "EventSource.hpp"
#include "GameState.hpp"

class SimulatorObserver {
public:
    virtual ~SimulatorObserver() = default;

    virtual void OnPreEvent(const GameState& state) {
        std::cout << "Before: "
                  << " (State: Out=" << state.GetOuts()
                  << ", Inning=" << state.GetInning()
                  << ", Batting team=" << state.GetBattingTeam()
                  << ", Score=" << state.GetScore(0) << "-" << state.GetScore(1) << ")\n";
    }

    virtual void OnEvent(const PlayInfo& event) {
        std::cout << "Play: " << event.batter << " - " << event.text << "\n";
    }

    virtual void OnSubstitution(const SubstitutionInfo& sub) {
        std::cout << "Substitution: " << sub.name << " (" << sub.playerID << ") at pos " << sub.pos << "\n";
    }

    virtual void OnComment(const std::string& comment) {
        std::cout << "Comment: " << comment << "\n";
    }

    virtual void OnRunnerAdjustment(const RunnerAdjustmentInfo& radj) {
        std::cout << "Runner Adjustment: " << radj.playerID << " to base " << radj.base << "\n";
    }

    virtual void OnPostEvent(const GameState& state) {
        std::cout << "After: "
                  << " (State: Out=" << state.GetOuts()
                  << ", Inning=" << state.GetInning()
                  << ", Batting team=" << state.GetBattingTeam()
                  << ", Score=" << state.GetScore(0) << "-" << state.GetScore(1) << ")\n\n";
    }
};

#endif //BASEBALL_CPP_SIMULATOROBSERVER_HPP
