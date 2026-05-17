#include <iostream>
#include "chadwick/Scorebook.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    const char* inputPath = argv[1];
    const char* outputPath = argv[2];

    chadwick::Scorebook scorebook;
    int gamesRead = scorebook.Read(inputPath);

    if (gamesRead < 0) {
        std::cerr << "Error: Failed to read scorebook from " << inputPath << std::endl;
        return 1;
    }

    std::cout << "Successfully read " << gamesRead << " games from " << inputPath << std::endl;

    if (!scorebook.Write(outputPath)) {
        std::cerr << "Error: Could not open output file: " << outputPath << std::endl;
        return 1;
    }

    std::cout << "Successfully wrote normalized scorebook to " << outputPath << std::endl;

    return 0;
}
