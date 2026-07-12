#ifndef BASEBALL_CPP_MATRIXLOADER_HPP
#define BASEBALL_CPP_MATRIXLOADER_HPP

#include "MatrixData.hpp"
#include <filesystem>

/**
 * @brief Utility class for loading baseball transition matrices from CSV files.
 */
class MatrixLoader {
public:
    /**
     * @brief Loads matrices from the specified directory.
     * @param dataDir The directory containing the matrix CSV files.
     * @return A MatrixData object containing the loaded matrices and distributions.
     */
    static auto LoadMatrices(const std::filesystem::path& dataDir) -> MatrixData;
};

#endif // BASEBALL_CPP_MATRIXLOADER_HPP
