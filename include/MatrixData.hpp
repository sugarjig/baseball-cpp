#ifndef BASEBALL_CPP_MATRIXDATA_HPP
#define BASEBALL_CPP_MATRIXDATA_HPP

#include <cstddef> // IWYU pragma: keep
#include <map>     // IWYU pragma: keep
#include <random>  // IWYU pragma: keep
#include <string>  // IWYU pragma: keep
#include <vector>  // IWYU pragma: keep

/**
 * @brief An outcome sampled from a transition matrix.
 */
struct MatrixOutcome {
    std::string play;
    std::string type;
    std::string bnt;
    std::string fl;
    std::string loc;
    std::string batterBase;
    std::string r1Base;
    std::string r2Base;
    std::string r3Base;
    int weight = 0;
    int newOuts = 0;
    int runs = 0;
};

/**
 * @brief Container for loaded matrix data and distributions.
 */
struct MatrixData {
    std::map<std::string, std::vector<MatrixOutcome>> matrices;
    std::map<std::string, std::discrete_distribution<size_t>> distributions;
};

#endif // BASEBALL_CPP_MATRIXDATA_HPP
