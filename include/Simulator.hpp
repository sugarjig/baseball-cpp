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
    Simulator(const Simulator&) = delete;
    auto operator=(const Simulator&) -> Simulator& = delete;
    Simulator(Simulator&&) = default;
    auto operator=(Simulator&&) -> Simulator& = default;

    void SimulateGame(IGame& game);
};

#endif // BASEBALL_CPP_SIMULATOR_HPP
