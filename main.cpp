#include <iostream>
#include "Simulator.h"
#include "StaticEventSource.h"
#include "SimulatorObserver.h"

int main() {
    std::cout << "Starting Chadwick example...\n";

    auto* eventSource = new StaticEventSource();
    auto* observer = new SimulatorObserver();
    auto* simulator = new Simulator(eventSource, observer);

    CWGame* game = cw_game_create((char*)"TEST01");
    if (!game) {
        std::cerr << "Failed to create game\n";
        return 1;
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

    simulator->SimulateGame(game);

    FILE* outfile = fopen("game.evn", "w");
    if (outfile) {
        cw_game_write(game, outfile);
        fclose(outfile);
        std::cout << "Game written to game.evn\n";
    } else {
        std::cerr << "Failed to open game.evn for writing\n";
    }

    cw_game_cleanup(game);
    free(game);

    delete simulator;
    delete observer;
    delete eventSource;

    return 0;
}