#include <iostream>
#include "Simulator.h"
#include "EventSource.h"

int main() {
    std::cout << "Starting Chadwick example...\n";

    auto* eventSource = new EventSource();
    auto* simulator = new Simulator(eventSource);

    simulator->SimulateGame();

    delete simulator;
    delete eventSource;

    return 0;
}