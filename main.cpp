#include <iostream>
#include <unistd.h>
#include "Simulator.h"
#include "StaticEventSource.h"
#include "SimulatorObserver.h"

int main() {
    std::cout << "Starting Chadwick example...\n";

    auto* eventSource = new StaticEventSource();
    auto* observer = new SimulatorObserver();
    auto* simulator = new Simulator(eventSource, observer);

    CWGame* game = cw_game_create((char*)"BAL202503310");
    if (!game) {
        std::cerr << "Failed to create game\n";
        return 1;
    }

    // date record must be set before iterator is created
    cw_game_info_append(game, (char*)"date", (char*)"2025/03/31");
    cw_game_info_append(game, (char*)"hometeam", (char*)"BAL");
    cw_game_info_append(game, (char*)"visteam", (char*)"BOS");

    // Starter records
    cw_game_starter_append(game, (char*)"duraj001", (char*)"Jarren Duran", 0, 1, 7);
    cw_game_starter_append(game, (char*)"dever001", (char*)"Rafael Devers", 0, 2, 10);
    cw_game_starter_append(game, (char*)"brega001", (char*)"Alex Bregman", 0, 3, 5);
    cw_game_starter_append(game, (char*)"refsr001", (char*)"Rob Refsnyder", 0, 4, 9);
    cw_game_starter_append(game, (char*)"stort001", (char*)"Trevor Story", 0, 5, 6);
    cw_game_starter_append(game, (char*)"campk002", (char*)"Kristian Campbell", 0, 6, 4);
    cw_game_starter_append(game, (char*)"gonzr003", (char*)"Romy Gonzalez", 0, 7, 3);
    cw_game_starter_append(game, (char*)"wongc001", (char*)"Connor Wong", 0, 8, 2);
    cw_game_starter_append(game, (char*)"rafac001", (char*)"Ceddanne Rafaela", 0, 9, 8);
    cw_game_starter_append(game, (char*)"newcs001", (char*)"Sean Newcomb", 0, 0, 1);
    cw_game_starter_append(game, (char*)"westj002", (char*)"Jordan Westburg", 1, 1, 4);
    cw_game_starter_append(game, (char*)"rutsa001", (char*)"Adley Rutschman", 1, 2, 2);
    cw_game_starter_append(game, (char*)"oneit001", (char*)"Tyler O'Neill", 1, 3, 7);
    cw_game_starter_append(game, (char*)"mounr001", (char*)"Ryan Mountcastle", 1, 4, 3);
    cw_game_starter_append(game, (char*)"kjerh001", (char*)"Heston Kjerstad", 1, 5, 10);
    cw_game_starter_append(game, (char*)"laurr001", (char*)"Ramon Laureano", 1, 6, 9);
    cw_game_starter_append(game, (char*)"mullc002", (char*)"Cedric Mullins", 1, 7, 8);
    cw_game_starter_append(game, (char*)"uriar001", (char*)"Ramon Urias", 1, 8, 5);
    cw_game_starter_append(game, (char*)"hollj003", (char*)"Jackson Holliday", 1, 9, 6);
    cw_game_starter_append(game, (char*)"povic001", (char*)"Cade Povich", 1, 0, 1);

    simulator->SimulateGame(game);

    FILE* outfile = fopen("game.evn", "w");
    if (outfile) {
        cw_game_write(game, outfile);
        fclose(outfile);
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            std::cout << "Game written to " << cwd << "/game.evn\n";
        } else {
            std::cout << "Game written to game.evn\n";
        }
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