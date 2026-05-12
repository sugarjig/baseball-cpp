#include <iostream>
#include <cstdio>
#include <vector>

extern "C" {
#include "chadwick.h"
}

int main() {
    std::cout << "Starting Chadwick example...\n";

    // 1. Initialize a game.
    CWGame* game = cw_game_create((char*)"TEST01");
    if (!game) {
        std::cerr << "Failed to create game\n";
        return 1;
    }

    // Set info records - essential for iterator
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

    // 3 & 4. Append events and advance iterator.
    struct EventInfo {
        int inning;
        int team;
        const char* batter;
        const char* text;
    };

    std::vector<EventInfo> events = {
        {1, 0, "AWAY01", "K"},
        {1, 0, "AWAY02", "K"},
        {1, 0, "AWAY03", "K"},
        {1, 1, "HOME01", "HR"},
        {1, 1, "HOME02", "K"},
        {1, 1, "HOME03", "K"},
        {1, 1, "HOME04", "K"}
    };

    CWGameIterator* iter = nullptr;
    for (auto & event : events) {
        cw_game_event_append(game, event.inning, event.team, const_cast<char *>(event.batter), (char*)"", (char*)"", const_cast<char *>(event.text));

        // Real-time iterator advancement on a growing game requires re-processing from 
        // the start to correctly handle side changes between events. This is O(N^2) 
        // but ensures the game state is always accurate as events are added.
        if (iter == nullptr) {
            iter = cw_gameiter_create(game);
        } else {
            cw_gameiter_reset(iter);
        }

        // Process all events up to the one just added.
        while (iter && iter->event != nullptr) {
            cw_gameiter_next(iter);
        }

        if (iter) {
            std::cout << "Processed event: " << event.batter << " - " << event.text
                      << " (State: Out=" << iter->state->outs
                      << ", Score=" << iter->state->score[0] << "-" << iter->state->score[1] << ")\n";
        }
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

    // Cleanup
    if (iter) {
        cw_gameiter_cleanup(iter);
        free(iter);
    }
    if (game) {
        cw_game_cleanup(game);
        free(game);
    }

    return 0;
}