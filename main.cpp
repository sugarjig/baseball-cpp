#include <iostream>
#include "Simulator.h"

int main() {
    std::cout << "Starting Chadwick example...\n";

    auto* simulator = new Simulator();

    simulator->SimulateGame();

    delete simulator;

    return 0;
}