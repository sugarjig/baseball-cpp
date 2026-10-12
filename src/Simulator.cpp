//
// Created by Samuel Jones on 5/12/26.
//

#include "Simulator.hpp"
#include "EventSource.hpp"
#include "IGame.hpp"
#include "SimulatorObserver.hpp"

Simulator::Simulator(EventSource* eventSource, SimulatorObserver* observer, int numInningsInGame)
    : eventSource(eventSource), observer(observer), numInningsInGame(numInningsInGame) {}

void Simulator::SimulateGame(IGame& game) const {
    while (true) {
        const auto event = eventSource->Next(game.GetGameState());
        if (!event.has_value()) {
            break;
        }

        if (observer != nullptr) {
            observer->OnPreEvent(game.GetGameState());
            observer->OnEvent(*event);
        }

        game.AddEvent(*event);

        if (observer != nullptr) {
            observer->OnPostEvent(game.GetGameState());
        }
    }
}

