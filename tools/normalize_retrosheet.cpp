#include "chadwick/Scorebook.hpp"
#include <exception> // IWYU pragma: keep
#include <iostream>  // IWYU pragma: keep

auto main(int argc, char* argv[]) -> int {
    try {
        if (argc != 3) {
            std::cerr << "Usage: " << argv[0] // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                      << " <input_file> <output_file>" << '\n';
            return 1;
        }

        const char* inputPath = argv[1];  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        const char* outputPath = argv[2]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

        chadwick::Scorebook const scorebook;
        int const gamesRead = scorebook.Read(inputPath);

        if (gamesRead < 0) {
            std::cerr << "Error: Failed to read scorebook from " << inputPath << '\n';
            return 1;
        }

        std::cout << "Successfully read " << gamesRead << " games from " << inputPath << '\n';

        if (!scorebook.Write(outputPath)) {
            std::cerr << "Error: Could not open output file: " << outputPath << '\n';
            return 1;
        }

        std::cout << "Successfully wrote normalized scorebook to " << outputPath << '\n';

        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Unexpected error: " << exception.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred\n";
        return 1;
    }
}
