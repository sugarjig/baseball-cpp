#include <iostream>
#include "Simulator.h"
#include "StaticEventSource.h"
#include "SimulatorObserver.h"

int main() {
    std::cout << "Starting Chadwick example...\n";

    auto* eventSource = new StaticEventSource();
    auto* observer = new SimulatorObserver();
    auto* simulator = new Simulator(eventSource, observer);

    simulator->SimulateGame((char*)"TEST01");

    delete simulator;
    delete observer;
    delete eventSource;

    return 0;
}