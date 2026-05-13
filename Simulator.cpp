//
// Created by Samuel Jones on 5/12/26.
//

#include "Simulator.h"
#include "EventSource.h"
#include "SimulatorObserver.h"

#include <iostream>
#include <vector>

Simulator::Simulator(EventSource* eventSource, SimulatorObserver* observer) : eventSource(eventSource), observer(observer) {
    game = cw_game_create((char*)"TEST01");
    if (!game) {
        std::cerr << "Failed to create game\n";
        return;
    }

    // date record must be set before iterator is created
    cw_game_info_append(game, (char*)"date", (char*)"2026/05/12");
    cw_game_info_append(game, (char*)"hometeam", (char*)"BOS");
    cw_game_info_append(game, (char*)"visteam", (char*)"NYA");

    // Starter records
    for (int i = 1; i <= 9; ++i) {
        char id[10], name[20];
        snprintf(id, sizeof(id), "AWAY%02d", i);
        snprintf(name, sizeof(name), "Away Player %d", i);
        cw_game_starter_append(game, id, name, 0, i, i);
    }
    for (int i = 1; i <= 9; ++i) {
        char id[10], name[20];
        snprintf(id, sizeof(id), "HOME%02d", i);
        snprintf(name, sizeof(name), "Home Player %d", i);
        cw_game_starter_append(game, id, name, 1, i, i);
    }

    iter = cw_gameiter_create(game);
}

Simulator::~Simulator() {
    cw_gameiter_cleanup(iter);
    free(iter);
    cw_game_cleanup(game);
    free(game);
}

void Simulator::SimulateGame() {
    // 3 & 4. Append events and advance iterator.
    while (auto event = eventSource->Next()) {
        if (observer) observer->OnPreEvent(iter, *event);

        cw_game_event_append(game, event->inning, event->team, const_cast<char *>(event->batter.c_str()), (char*)"", (char*)"", const_cast<char *>(event->text.c_str()));

        cw_gameiter_reset(iter);

        // Process all events up to the one just added.
        while (iter->event != nullptr) {
            cw_gameiter_next(iter);
        }

        if (observer) observer->OnEvent(*event);

        if (observer) observer->OnPostEvent(iter, *event);
    }

    // 5. After processing all the events, write the game to a Retrosheet event file.
    FILE* outfile = fopen("game.evn", "w");
    if (outfile) {
        cw_game_write(game, outfile);
        fclose(outfile);
        std::cout << "Game written to game.evn\n";
    } else {
        std::cerr << "Failed to open game.evn for writing\n";
    }
}
