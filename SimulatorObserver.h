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

    virtual void OnPreEvent(CWGameIterator* iter, const EventInfo& event) {
        std::cout << "Before: "
                  << " (State: Out=" << iter->state->outs
                  << ", Inning=" << iter->state->inning
                  << ", Score=" << iter->state->score[0] << "-" << iter->state->score[1] << ")\n";
    }

    virtual void OnEvent(const EventInfo& event) {
        std::cout << "Processed event: " << event.batter << " - " << event.text << "\n";
    }

    virtual void OnPostEvent(CWGameIterator* iter, const EventInfo& event) {
        std::cout << "After: "
                  << " (State: Out=" << iter->state->outs
                  << ", Inning=" << iter->state->inning
                  << ", Score=" << iter->state->score[0] << "-" << iter->state->score[1] << ")\n\n";
    }
};

#endif //BASEBALL_CPP_SIMULATOROBSERVER_H
