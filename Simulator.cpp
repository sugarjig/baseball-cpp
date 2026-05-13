//
// Created by Samuel Jones on 5/12/26.
//

#include "Simulator.h"
#include "EventSource.h"
#include "SimulatorObserver.h"

Simulator::Simulator(EventSource* eventSource, SimulatorObserver* observer) : eventSource(eventSource), observer(observer) {
}

Simulator::~Simulator() = default;

void Simulator::SimulateGame(CWGame* game) {
    CWGameIterator* iter = cw_gameiter_create(game);

    while (auto record = eventSource->Next()) {
        if (observer) observer->OnPreEvent(iter->state);

        switch (record->type) {
            case RecordType::Play: {
                const auto& play = std::get<PlayInfo>(record->data);
                cw_game_event_append(game, play.inning, play.team, 
                                     const_cast<char *>(play.batter.c_str()), 
                                     const_cast<char *>(play.pitchCount.c_str()), 
                                     const_cast<char *>(play.pitchSequence.c_str()), 
                                     const_cast<char *>(play.text.c_str()));
                if (observer) observer->OnEvent(play);
                break;
            }
            case RecordType::Substitution: {
                const auto& sub = std::get<SubstitutionInfo>(record->data);
                cw_game_substitute_append(game, const_cast<char *>(sub.playerID.c_str()), const_cast<char *>(sub.name.c_str()), sub.team, sub.slot, sub.pos);
                break;
            }
            case RecordType::Comment: {
                const auto& comment = std::get<std::string>(record->data);
                cw_game_comment_append(game, const_cast<char *>(comment.c_str()));
                break;
            }
        }

        cw_gameiter_reset(iter);

        // Process all events up to the one just added.
        while (iter->event != nullptr) {
            cw_gameiter_next(iter);
        }

        if (observer) observer->OnPostEvent(iter->state);
    }

    cw_gameiter_cleanup(iter);
    free(iter);
}
