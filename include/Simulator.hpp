//
// Created by Samuel Jones on 5/12/26.
//

#ifndef BASEBALL_CPP_SIMULATOR_HPP
#define BASEBALL_CPP_SIMULATOR_HPP

class EventSource;
class SimulatorObserver;
class IGame;
class IGameState;

/**
 * @brief The central engine for simulating a baseball game.
 *
 * The Simulator pulls records from an EventSource and applies them to an IGame object,
 * optionally notifying a SimulatorObserver of progress.
 */
class Simulator {
    EventSource* eventSource;
    SimulatorObserver* observer;

public:
    /**
     * @brief Constructs a Simulator with an event source and an optional observer.
     * @param eventSource The source of baseball events to simulate.
     * @param observer An optional observer to hook into simulation events.
     */
    explicit Simulator(EventSource* eventSource, SimulatorObserver* observer = nullptr);

    ~Simulator() = default;
    Simulator(const Simulator&) = delete;
    auto operator=(const Simulator&) -> Simulator& = delete;
    Simulator(Simulator&&) = default;
    auto operator=(Simulator&&) -> Simulator& = default;

    /**
     * @brief Simulates a game by processing all events from the event source.
     * @param game The game object to which events will be applied.
     */
    void SimulateGame(IGame& game) const;
};

#endif // BASEBALL_CPP_SIMULATOR_HPP
