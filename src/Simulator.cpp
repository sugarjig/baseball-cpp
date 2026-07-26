//
// Created by Samuel Jones on 5/12/26.
//

#include "Simulator.hpp"
#include "EventSource.hpp"
#include "IGame.hpp"
#include "Records.hpp"
#include "SimulatorObserver.hpp"
#include <string>

Simulator::Simulator(EventSource* eventSource, SimulatorObserver* observer)
    : eventSource(eventSource), observer(observer) {}

void Simulator::SimulateGame(IGame& game) const {
    while (const auto event = eventSource->Next(game.GetGameState())) {
        if (observer != nullptr) {
            observer->OnPreEvent(game.GetGameState());
            observer->OnEvent(*event);
        }

        switch (event->type) {
        case EventType::Play: {
            const auto& play = std::get<PlayInfo>(event->data);
            game.AddPlay(play);
            break;
        }
        case EventType::Substitution: {
            const auto& sub = std::get<SubstitutionInfo>(event->data);
            game.AddSubstitution(sub);
            break;
        }
        case EventType::Comment: {
            const auto& comment = std::get<std::string>(event->data);
            game.AddComment(comment);
            break;
        }
        case EventType::RunnerAdjustment: {
            const auto& radj = std::get<RunnerAdjustmentInfo>(event->data);
            game.AddRunnerAdjustment(radj);
            break;
        }
        case EventType::BatterAdjustment: {
            const auto& badj = std::get<BatterAdjustmentInfo>(event->data);
            game.AddBatterAdjustment(badj);
            break;
        }
        case EventType::PitcherAdjustment: {
            const auto& padj = std::get<PitcherAdjustmentInfo>(event->data);
            game.AddPitcherAdjustment(padj);
            break;
        }
        default:
            break;
        }

        if (observer != nullptr) {
            observer->OnPostEvent(game.GetGameState());
        }
    }
}
