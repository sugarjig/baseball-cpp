//
// Created by Samuel Jones on 5/12/26.
//

#include "Records.hpp"
#include "Simulator.hpp"
#include "EventSource.hpp"
#include "IGame.hpp"
#include "SimulatorObserver.hpp"

Simulator::Simulator(EventSource* eventSource, SimulatorObserver* observer)
    : eventSource(eventSource), observer(observer) {}

void Simulator::SimulateGame(IGame& game) const {
    while (auto record = eventSource->Next()) {
        if (observer != nullptr) {
            observer->OnPreEvent(game.GetGameState());
        }

        switch (record->type) {
        case RecordType::Play: {
            const auto& play = std::get<PlayInfo>(record->data);
            game.AddEvent(play);
            if (observer != nullptr) {
                observer->OnEvent(play);
            }
            break;
        }
        case RecordType::Substitution: {
            const auto& sub = std::get<SubstitutionInfo>(record->data);
            game.AddSubstitution(sub);
            if (observer != nullptr) {
                observer->OnSubstitution(sub);
            }
            break;
        }
        case RecordType::Comment: {
            const auto& comment = std::get<std::string>(record->data);
            game.AddComment(comment);
            if (observer != nullptr) {
                observer->OnComment(comment);
            }
            break;
        }
        case RecordType::RunnerAdjustment: {
            const auto& radj = std::get<RunnerAdjustmentInfo>(record->data);
            game.AddRunnerAdjustment(radj);
            if (observer != nullptr) {
                observer->OnRunnerAdjustment(radj);
            }
            break;
        }
        case RecordType::BatterAdjustment: {
            const auto& badj = std::get<BatterAdjustmentInfo>(record->data);
            game.AddBatterAdjustment(badj);
            if (observer != nullptr) {
                observer->OnBatterAdjustment(badj);
            }
            break;
        }
        case RecordType::PitcherAdjustment: {
            const auto& padj = std::get<PitcherAdjustmentInfo>(record->data);
            game.AddPitcherAdjustment(padj);
            if (observer != nullptr) {
                observer->OnPitcherAdjustment(padj);
            }
            break;
        }
        default:
            break;
        }

        game.UpdateState();

        if (observer != nullptr) {
            observer->OnPostEvent(game.GetGameState());
        }
    }
}
