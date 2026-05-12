#include <iostream>

extern "C" {
#include "chadwick.h"
}

int main() {
    std::cout << "Baseball C++ Application starting...\n";

    // Initialize a Chadwick game object
    CWGame *game = cw_game_create((char*)"TEST01");

    if (game) {
        std::cout << "Successfully initialized Chadwick library.\n";
        std::cout << "Game ID: " << game->game_id << "\n";

        // Add some info
        cw_game_info_append(game, (char*)"vvis", (char*)"NYA");
        cw_game_info_append(game, (char*)"vhome", (char*)"BOS");

        std::cout << "Visitor: " << cw_game_info_lookup(game, (char*)"vvis") << "\n";
        std::cout << "Home: " << cw_game_info_lookup(game, (char*)"vhome") << "\n";

        // Cleanup
        cw_game_cleanup(game);
        free(game);
    } else {
        std::cerr << "Failed to create Chadwick game object.\n";
        return 1;
    }

    return 0;
}