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
    for (size_t i = 0; i < events.size(); ++i) {
        // If this is the first event, append it and then initialize the iterator.
        if (i == 0) {
            cw_game_event_append(game, events[i].inning, events[i].team, (char*)events[i].batter, (char*)"", (char*)"", (char*)events[i].text);
            iter = cw_gameiter_create(game);
        }
        
        // To allow cw_gameiter_next to naturally advance to the next event without manual parsing 
        // logic, we append the NEXT event (if any) before calling next for the CURRENT event.
        if (i + 1 < events.size()) {
            const auto& next_e = events[i+1];
            cw_game_event_append(game, next_e.inning, next_e.team, (char*)next_e.batter, (char*)"", (char*)"", (char*)next_e.text);
        }

        // Call the iterator's next function to process the current event.
        // This will process gameiter->event and then move gameiter->event to the next event (if it exists).
        cw_gameiter_next(iter);

        std::cout << "Processed event: " << events[i].batter << " - " << events[i].text 
                  << " (State: Out=" << iter->state->outs 
                  << ", Score=" << iter->state->score[0] << "-" << iter->state->score[1] << ")\n";
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
    cw_gameiter_cleanup(iter);
    free(iter);
    cw_game_cleanup(game);
    free(game);

    return 0;
}