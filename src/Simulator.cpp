//
// Created by Samuel Jones on 5/12/26.
//

#include "Simulator.hpp"
#include "EventSource.hpp"
#include "IGame.hpp"
#include "SimulatorObserver.hpp"

Simulator::Simulator(EventSource* eventSource, SimulatorObserver* observer)
    : eventSource(eventSource), observer(observer) {}

void Simulator::SimulateGame(IGame& game) const {
    while (true) {
        const bool keepPlaying = game.GetGameState().KeepPlaying();
        const auto event = eventSource->Next(game.GetGameState());
        if (!event) {
            break;
        }

        if (!keepPlaying && event->type == EventType::Play) {
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
