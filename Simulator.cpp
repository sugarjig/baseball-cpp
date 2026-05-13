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

    while (auto event = eventSource->Next()) {
        if (observer) observer->OnPreEvent(iter->state);

        cw_game_event_append(game, event->inning, event->team, const_cast<char *>(event->batter.c_str()), (char*)"", (char*)"", const_cast<char *>(event->text.c_str()));

        if (observer) observer->OnEvent(*event);

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
