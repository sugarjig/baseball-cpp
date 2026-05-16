#include <iostream>
#include <cstdio>
#include <cstdlib>

extern "C" {
#include "chadwick.h"
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    const char* inputPath = argv[1];
    const char* outputPath = argv[2];

    FILE* inputFile = std::fopen(inputPath, "r");
    if (!inputFile) {
        std::cerr << "Error: Could not open input file: " << inputPath << std::endl;
        return 1;
    }

    CWScorebook* scorebook = cw_scorebook_create();
    if (!scorebook) {
        std::cerr << "Error: Could not create CWScorebook" << std::endl;
        std::fclose(inputFile);
        return 1;
    }

    int gamesRead = cw_scorebook_read(scorebook, inputFile);
    std::fclose(inputFile);

    if (gamesRead < 0) {
        std::cerr << "Error: Failed to read scorebook from " << inputPath << std::endl;
        cw_scorebook_cleanup(scorebook);
        std::free(scorebook);
        return 1;
    }

    std::cout << "Successfully read " << gamesRead << " games from " << inputPath << std::endl;

    FILE* outputFile = std::fopen(outputPath, "w");
    if (!outputFile) {
        std::cerr << "Error: Could not open output file: " << outputPath << std::endl;
        cw_scorebook_cleanup(scorebook);
        std::free(scorebook);
        return 1;
    }

    cw_scorebook_write(scorebook, outputFile);
    std::fclose(outputFile);

    std::cout << "Successfully wrote normalized scorebook to " << outputPath << std::endl;

    cw_scorebook_cleanup(scorebook);
    std::free(scorebook);

    return 0;
}
