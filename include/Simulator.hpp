//
// Created by Samuel Jones on 5/12/26.
//

#ifndef BASEBALL_CPP_SIMULATOR_HPP
#define BASEBALL_CPP_SIMULATOR_HPP

class EventSource;
class SimulatorObserver;
class Game;

class Simulator {
    EventSource* eventSource;
    SimulatorObserver* observer;
public:
    Simulator(EventSource* eventSource, SimulatorObserver* observer = nullptr);

    ~Simulator();

    void SimulateGame(Game& game);
};


#endif //BASEBALL_CPP_SIMULATOR_HPP
