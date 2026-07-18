#include "MatrixEventSource.hpp"
#include "MatrixLoader.hpp"
#include "Records.hpp"
#include "Simulator.hpp"
#include "SimulatorObserver.hpp"
#include "chadwick/Game.hpp"
#include <exception>
#include <filesystem>
#include <iostream>
#include <utility>
#include <vector>

auto main() -> int {
    try {
        std::cout << "Starting Chadwick example...\n";

        std::filesystem::path const dataDir = std::filesystem::path(PROJECT_ROOT) / "data" / "matrices_2025";
        auto data = MatrixLoader::LoadMatrices(dataDir);
        MatrixEventSource eventSource{std::move(data)};

        SimulatorObserver observer;
        Simulator const simulator(&eventSource, &observer);

        std::vector<InfoRecord> const infoRecords = {
            {.key = "date", .value = "2025/03/31"},
            {.key = "hometeam", .value = "BAL"},
            {.key = "visteam", .value = "BOS"},
        };

        std::vector<StarterInfo> const starters = {
            {.id = "duraj001", .name = "Jarren Duran", .isHome = false, .battingOrder = 1, .position = 7},
            {.id = "dever001", .name = "Rafael Devers", .isHome = false, .battingOrder = 2, .position = 10},
            {.id = "brega001", .name = "Alex Bregman", .isHome = false, .battingOrder = 3, .position = 5},
            {.id = "refsr001", .name = "Rob Refsnyder", .isHome = false, .battingOrder = 4, .position = 9},
            {.id = "stort001", .name = "Trevor Story", .isHome = false, .battingOrder = 5, .position = 6},
            {.id = "campk002", .name = "Kristian Campbell", .isHome = false, .battingOrder = 6, .position = 4},
            {.id = "gonzr003", .name = "Romy Gonzalez", .isHome = false, .battingOrder = 7, .position = 3},
            {.id = "wongc001", .name = "Connor Wong", .isHome = false, .battingOrder = 8, .position = 2},
            {.id = "rafac001", .name = "Ceddanne Rafaela", .isHome = false, .battingOrder = 9, .position = 8},
            {.id = "newcs001", .name = "Sean Newcomb", .isHome = false, .battingOrder = 0, .position = 1},
            {.id = "westj002", .name = "Jordan Westburg", .isHome = true, .battingOrder = 1, .position = 4},
            {.id = "rutsa001", .name = "Adley Rutschman", .isHome = true, .battingOrder = 2, .position = 2},
            {.id = "oneit001", .name = "Tyler O'Neill", .isHome = true, .battingOrder = 3, .position = 7},
            {.id = "mounr001", .name = "Ryan Mountcastle", .isHome = true, .battingOrder = 4, .position = 3},
            {.id = "kjerh001", .name = "Heston Kjerstad", .isHome = true, .battingOrder = 5, .position = 10},
            {.id = "laurr001", .name = "Ramon Laureano", .isHome = true, .battingOrder = 6, .position = 9},
            {.id = "mullc002", .name = "Cedric Mullins", .isHome = true, .battingOrder = 7, .position = 8},
            {.id = "uriar001", .name = "Ramon Urias", .isHome = true, .battingOrder = 8, .position = 5},
            {.id = "hollj003", .name = "Jackson Holliday", .isHome = true, .battingOrder = 9, .position = 6},
            {.id = "povic001", .name = "Cade Povich", .isHome = true, .battingOrder = 0, .position = 1},
        };

        chadwick::Game game("BAL202503310", "2", infoRecords, starters);
        if (!game) {
            std::cerr << "Failed to create game\n";
            return 1;
        }

        simulator.SimulateGame(game);

        if (game.Write("game.evn")) {
            std::cout << "Game written to " << std::filesystem::current_path() << "/game.evn\n";
        } else {
            std::cerr << "Failed to open game.evn for writing\n";
        }

        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Unexpected error: " << exception.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred\n";
        return 1;
    }
}