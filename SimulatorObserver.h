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
                  << ", Score=" << state->score[0] << "-" << state->score[1] << ")\n";
    }

    virtual void OnEvent(const PlayInfo& event) {
        std::cout << "Processed event: " << event.batter << " - " << event.text << "\n";
    }

    virtual void OnPostEvent(CWGameState* state) {
        std::cout << "After: "
                  << " (State: Out=" << state->outs
                  << ", Inning=" << state->inning
                  << ", Score=" << state->score[0] << "-" << state->score[1] << ")\n\n";
    }
};

#endif //BASEBALL_CPP_SIMULATOROBSERVER_H
