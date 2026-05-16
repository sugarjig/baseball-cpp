//
// Created by Samuel Jones on 5/12/26.
//

#include "Simulator.hpp"
#include "EventSource.hpp"
#include "SimulatorObserver.hpp"
#include "Game.hpp"

Simulator::Simulator(EventSource* eventSource, SimulatorObserver* observer) : eventSource(eventSource), observer(observer) {
}

Simulator::~Simulator() = default;

void Simulator::SimulateGame(IGame& game) {
    while (auto record = eventSource->Next()) {
        if (observer) observer->OnPreEvent(game.GetGameState());

        switch (record->type) {
            case RecordType::Play: {
                const auto& play = std::get<PlayInfo>(record->data);
                game.AddEvent(play);
                if (observer) observer->OnEvent(play);
                break;
            }
            case RecordType::Substitution: {
                const auto& sub = std::get<SubstitutionInfo>(record->data);
                game.AddSubstitution(sub);
                if (observer) observer->OnSubstitution(sub);
                break;
            }
            case RecordType::Comment: {
                const auto& comment = std::get<std::string>(record->data);
                game.AddComment(comment.c_str());
                if (observer) observer->OnComment(comment);
                break;
            }
            case RecordType::RunnerAdjustment: {
                const auto& radj = std::get<RunnerAdjustmentInfo>(record->data);
                game.AddRunnerAdjustment(radj);
                if (observer) observer->OnRunnerAdjustment(radj);
                break;
            }
            case RecordType::BatterAdjustment: {
                const auto& badj = std::get<BatterAdjustmentInfo>(record->data);
                game.AddBatterAdjustment(badj);
                if (observer) observer->OnBatterAdjustment(badj);
                break;
            }
            case RecordType::PitcherAdjustment: {
                const auto& padj = std::get<PitcherAdjustmentInfo>(record->data);
                game.AddPitcherAdjustment(padj);
                if (observer) observer->OnPitcherAdjustment(padj);
                break;
            }
            default: break;
        }

        game.UpdateState();

        if (observer) observer->OnPostEvent(game.GetGameState());
    }
}
