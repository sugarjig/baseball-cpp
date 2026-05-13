#include <iostream>
#include "Simulator.h"
#include "StaticEventSource.h"

int main() {
    std::cout << "Starting Chadwick example...\n";

    auto* eventSource = new StaticEventSource();
    auto* simulator = new Simulator(eventSource);

    simulator->SimulateGame();

    delete simulator;
    delete eventSource;

    return 0;
}