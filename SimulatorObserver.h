#ifndef BASEBALL_CPP_SIMULATOROBSERVER_H
#define BASEBALL_CPP_SIMULATOROBSERVER_H

#include <iostream>
#include "EventSource.h"
extern "C" {
#include "chadwick.h"
}

class SimulatorObserver {
public:
    virtual ~SimulatorObserver() = default;

    virtual void OnPreEvent(CWGameState* state) {
        std::cout << "Before: "
                  << " (State: Out=" << state->outs
                  << ", Inning=" << state->inning
                  << ", Batting team=" << state->batting_team
                  << ", Score=" << state->score[0] << "-" << state->score[1] << ")\n";
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

    virtual void OnPostEvent(CWGameState* state) {
        std::cout << "After: "
                  << " (State: Out=" << state->outs
                  << ", Inning=" << state->inning
                  << ", Batting team=" << state->batting_team
                  << ", Score=" << state->score[0] << "-" << state->score[1] << ")\n\n";
    }
};

#endif //BASEBALL_CPP_SIMULATOROBSERVER_H
