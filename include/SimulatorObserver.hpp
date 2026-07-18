#ifndef BASEBALL_CPP_SIMULATOROBSERVER_HPP
#define BASEBALL_CPP_SIMULATOROBSERVER_HPP

#include "IGameState.hpp"
#include "Records.hpp"
#include <iostream>
#include <string>

/**
 * @brief Observer interface for monitoring baseball simulation events.
 *
 * Provides default empty or logging implementations for various hooks during simulation.
 */
class SimulatorObserver {
public:
    SimulatorObserver() = default;
    virtual ~SimulatorObserver() = default;
    SimulatorObserver(const SimulatorObserver&) = delete;
    auto operator=(const SimulatorObserver&) -> SimulatorObserver& = delete;
    SimulatorObserver(SimulatorObserver&&) = default;
    auto operator=(SimulatorObserver&&) -> SimulatorObserver& = default;

    /**
     * @brief Called before an event is processed.
     * @param state The current state of the game.
     */
    virtual void OnPreEvent(const IGameState& state) {
        std::cout << "Before: (State: Out=" << state.GetOuts() << ", Inning=" << state.GetInning()
                  << ", Batting team=" << state.GetBattingTeam() << ", Score=" << state.GetScore(0) << "-"
                  << state.GetScore(1) << ", Batter=" << state.GetNextBatter(state.GetBattingTeam())
                  << ", Bases=1:" << (state.IsBaseOccupied(1) ? state.GetRunnerOnBase(1) : "-")
                  << ", 2:" << (state.IsBaseOccupied(2) ? state.GetRunnerOnBase(2) : "-")
                  << ", 3:" << (state.IsBaseOccupied(3) ? state.GetRunnerOnBase(3) : "-") << ")\n";
    }

    /**
     * @brief Called when a play event occurs.
     * @param event The play information.
     */
    virtual void OnEvent(const PlayInfo& event) {
        std::cout << "Play: " << event.batter << " - " << event.text;
        if (auto dot = event.text.find('.'); dot != std::string::npos) {
            std::cout << " [Adv: " << event.text.substr(dot + 1) << "]";
        }
        std::cout << "\n";
    }

    /**
     * @brief Called when a substitution occurs.
     * @param sub The substitution information.
     */
    virtual void OnSubstitution(const SubstitutionInfo& sub) {
        std::cout << "Substitution: " << sub.name << " (" << sub.playerId << ") at pos " << sub.pos << "\n";
    }

    /**
     * @brief Called when a comment record is encountered.
     * @param comment The comment text.
     */
    virtual void OnComment(const std::string& comment) { std::cout << "Comment: " << comment << "\n"; }

    /**
     * @brief Called when a runner adjustment occurs.
     * @param radj The runner adjustment information.
     */
    virtual void OnRunnerAdjustment(const RunnerAdjustmentInfo& radj) {
        std::cout << "Runner Adjustment: " << radj.playerId << " to base " << radj.base << "\n";
    }

    /**
     * @brief Called when a batter adjustment occurs.
     * @param badj The batter adjustment information.
     */
    virtual void OnBatterAdjustment(const BatterAdjustmentInfo& badj) {
        std::cout << "Batter Adjustment: " << badj.playerId << " to hand " << badj.hand << "\n";
    }

    /**
     * @brief Called when a pitcher adjustment occurs.
     * @param padj The pitcher adjustment information.
     */
    virtual void OnPitcherAdjustment(const PitcherAdjustmentInfo& padj) {
        std::cout << "Pitcher Adjustment: " << padj.playerId << " to hand " << padj.hand << "\n";
    }

    /**
     * @brief Called after an event is processed.
     * @param state The new state of the game.
     */
    virtual void OnPostEvent(const IGameState& state) {
        std::cout << "After: (State: Out=" << state.GetOuts() << ", Inning=" << state.GetInning()
                  << ", Batting team=" << state.GetBattingTeam() << ", Score=" << state.GetScore(0) << "-"
                  << state.GetScore(1) << ", Batter=" << state.GetNextBatter(state.GetBattingTeam())
                  << ", Bases=1:" << (state.IsBaseOccupied(1) ? state.GetRunnerOnBase(1) : "-")
                  << ", 2:" << (state.IsBaseOccupied(2) ? state.GetRunnerOnBase(2) : "-")
                  << ", 3:" << (state.IsBaseOccupied(3) ? state.GetRunnerOnBase(3) : "-") << ")\n\n";
    }
};

#endif // BASEBALL_CPP_SIMULATOROBSERVER_HPP
