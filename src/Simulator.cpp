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
    while (const auto record = eventSource->Next(game.GetGameState())) {
        if (observer != nullptr) {
            observer->OnPreEvent(game.GetGameState());
            observer->OnEvent(*record);
        }

        switch (record->type) {
        case RecordType::Play: {
            const auto& play = std::get<PlayInfo>(record->data);
            game.AddPlay(play);
            break;
        }
        case RecordType::Substitution: {
            const auto& sub = std::get<SubstitutionInfo>(record->data);
            game.AddSubstitution(sub);
            break;
        }
        case RecordType::Comment: {
            const auto& comment = std::get<std::string>(record->data);
            game.AddComment(comment);
            break;
        }
        case RecordType::RunnerAdjustment: {
            const auto& radj = std::get<RunnerAdjustmentInfo>(record->data);
            game.AddRunnerAdjustment(radj);
            break;
        }
        case RecordType::BatterAdjustment: {
            const auto& badj = std::get<BatterAdjustmentInfo>(record->data);
            game.AddBatterAdjustment(badj);
            break;
        }
        case RecordType::PitcherAdjustment: {
            const auto& padj = std::get<PitcherAdjustmentInfo>(record->data);
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
