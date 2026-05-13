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

void Simulator::SimulateGame(Game& game) {
    while (auto record = eventSource->Next()) {
        if (observer) observer->OnPreEvent(game.GetGameState());

        switch (record->type) {
            case RecordType::Play: {
                const auto& play = std::get<PlayInfo>(record->data);
                game.AddEvent(play.inning, play.team, 
                              play.batter.c_str(), 
                              play.pitchCount.c_str(), 
                              play.pitchSequence.c_str(), 
                              play.text.c_str());
                if (observer) observer->OnEvent(play);
                break;
            }
            case RecordType::Substitution: {
                const auto& sub = std::get<SubstitutionInfo>(record->data);
                game.AddSubstitution(sub.playerID.c_str(), sub.name.c_str(), sub.team, sub.slot, sub.pos);
                if (observer) observer->OnSubstitution(sub);
                break;
            }
            case RecordType::Comment: {
                const auto& comment = std::get<std::string>(record->data);
                game.AddComment(comment.c_str());
                if (observer) observer->OnComment(comment);
                break;
            }
        }

        game.UpdateState();

        if (observer) observer->OnPostEvent(game.GetGameState());
    }
}
