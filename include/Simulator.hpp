//
// Created by Samuel Jones on 5/12/26.
//

#ifndef BASEBALL_CPP_SIMULATOR_HPP
#define BASEBALL_CPP_SIMULATOR_HPP

class EventSource;
class SimulatorObserver;
class IGame;
class IGameState;

class Simulator {
    EventSource* eventSource;
    SimulatorObserver* observer;

public:
    explicit Simulator(EventSource* eventSource, SimulatorObserver* observer = nullptr);

    ~Simulator() = default;

    void SimulateGame(IGame& game);
};

#endif // BASEBALL_CPP_SIMULATOR_HPP
