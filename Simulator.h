//
// Created by Samuel Jones on 5/12/26.
//

#ifndef BASEBALL_CPP_SIMULATOR_H
#define BASEBALL_CPP_SIMULATOR_H

#include <cstdio>

extern "C" {
#include "chadwick.h"
}

class EventSource;
class SimulatorObserver;

class Simulator {
    EventSource* eventSource;
    SimulatorObserver* observer;
public:
    Simulator(EventSource* eventSource, SimulatorObserver* observer = nullptr);

    ~Simulator();

    void SimulateGame(CWGame* game);
};


#endif //BASEBALL_CPP_SIMULATOR_H
