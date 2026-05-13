#include <iostream>
#include <filesystem>
#include "Simulator.hpp"
#include "StaticEventSource.hpp"
#include "SimulatorObserver.hpp"
#include "Game.hpp"

int main() {
    std::cout << "Starting Chadwick example...\n";

    StaticEventSource eventSource;
    SimulatorObserver observer;
    Simulator simulator(&eventSource, &observer);

    Game game("BAL202503310", "2025/03/31");
    if (!game) {
        std::cerr << "Failed to create game\n";
        return 1;
    }

    game.AddInfo("hometeam", "BAL");
    game.AddInfo("visteam", "BOS");

    // Starter records
    game.AddStarter({"duraj001", "Jarren Duran", false, 1, 7});
    game.AddStarter({"dever001", "Rafael Devers", false, 2, 10});
    game.AddStarter({"brega001", "Alex Bregman", false, 3, 5});
    game.AddStarter({"refsr001", "Rob Refsnyder", false, 4, 9});
    game.AddStarter({"stort001", "Trevor Story", false, 5, 6});
    game.AddStarter({"campk002", "Kristian Campbell", false, 6, 4});
    game.AddStarter({"gonzr003", "Romy Gonzalez", false, 7, 3});
    game.AddStarter({"wongc001", "Connor Wong", false, 8, 2});
    game.AddStarter({"rafac001", "Ceddanne Rafaela", false, 9, 8});
    game.AddStarter({"newcs001", "Sean Newcomb", false, 0, 1});
    game.AddStarter({"westj002", "Jordan Westburg", true, 1, 4});
    game.AddStarter({"rutsa001", "Adley Rutschman", true, 2, 2});
    game.AddStarter({"oneit001", "Tyler O'Neill", true, 3, 7});
    game.AddStarter({"mounr001", "Ryan Mountcastle", true, 4, 3});
    game.AddStarter({"kjerh001", "Heston Kjerstad", true, 5, 10});
    game.AddStarter({"laurr001", "Ramon Laureano", true, 6, 9});
    game.AddStarter({"mullc002", "Cedric Mullins", true, 7, 8});
    game.AddStarter({"uriar001", "Ramon Urias", true, 8, 5});
    game.AddStarter({"hollj003", "Jackson Holliday", true, 9, 6});
    game.AddStarter({"povic001", "Cade Povich", true, 0, 1});

    simulator.SimulateGame(game);

    if (game.Write("game.evn")) {
        std::cout << "Game written to " << std::filesystem::current_path() << "/game.evn\n";
    } else {
        std::cerr << "Failed to open game.evn for writing\n";
    }

    return 0;
}