#include <iostream>
#include <unistd.h>
#include "Simulator.hpp"
#include "StaticEventSource.hpp"
#include "SimulatorObserver.hpp"
#include "Game.hpp"

int main() {
    std::cout << "Starting Chadwick example...\n";

    auto* eventSource = new StaticEventSource();
    auto* observer = new SimulatorObserver();
    auto* simulator = new Simulator(eventSource, observer);

    Game game("BAL202503310", "2025/03/31");
    if (!game) {
        std::cerr << "Failed to create game\n";
        return 1;
    }

    game.AddInfo("hometeam", "BAL");
    game.AddInfo("visteam", "BOS");

    // Starter records
    game.AddStarter({"duraj001", "Jarren Duran", 0, 1, 7});
    game.AddStarter({"dever001", "Rafael Devers", 0, 2, 10});
    game.AddStarter({"brega001", "Alex Bregman", 0, 3, 5});
    game.AddStarter({"refsr001", "Rob Refsnyder", 0, 4, 9});
    game.AddStarter({"stort001", "Trevor Story", 0, 5, 6});
    game.AddStarter({"campk002", "Kristian Campbell", 0, 6, 4});
    game.AddStarter({"gonzr003", "Romy Gonzalez", 0, 7, 3});
    game.AddStarter({"wongc001", "Connor Wong", 0, 8, 2});
    game.AddStarter({"rafac001", "Ceddanne Rafaela", 0, 9, 8});
    game.AddStarter({"newcs001", "Sean Newcomb", 0, 0, 1});
    game.AddStarter({"westj002", "Jordan Westburg", 1, 1, 4});
    game.AddStarter({"rutsa001", "Adley Rutschman", 1, 2, 2});
    game.AddStarter({"oneit001", "Tyler O'Neill", 1, 3, 7});
    game.AddStarter({"mounr001", "Ryan Mountcastle", 1, 4, 3});
    game.AddStarter({"kjerh001", "Heston Kjerstad", 1, 5, 10});
    game.AddStarter({"laurr001", "Ramon Laureano", 1, 6, 9});
    game.AddStarter({"mullc002", "Cedric Mullins", 1, 7, 8});
    game.AddStarter({"uriar001", "Ramon Urias", 1, 8, 5});
    game.AddStarter({"hollj003", "Jackson Holliday", 1, 9, 6});
    game.AddStarter({"povic001", "Cade Povich", 1, 0, 1});

    simulator->SimulateGame(game);

    FILE* outfile = fopen("game.evn", "w");
    if (outfile) {
        game.Write(outfile);
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

    delete simulator;
    delete observer;
    delete eventSource;

    return 0;
}