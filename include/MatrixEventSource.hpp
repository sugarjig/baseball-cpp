#ifndef BASEBALL_CPP_MATRIXEVENTSOURCE_HPP
#define BASEBALL_CPP_MATRIXEVENTSOURCE_HPP

#include "EventSource.hpp"
#include "IGameState.hpp"
#include <filesystem>
#include <map>
#include <random>
#include <string>
#include <vector>

/**
 * @brief An outcome sampled from a transition matrix.
 */
struct MatrixOutcome {
    std::string play{};
    std::string type{};
    std::string bnt{};
    std::string fl{};
    std::string loc{};
    std::string batterBase{};
    std::string r1Base{};
    std::string r2Base{};
    std::string r3Base{};
    int weight = 0;
    int newOuts = 0;
    int runs = 0;
};

/**
 * @brief An EventSource that generates baseball events based on transition matrices.
 *
 * This implementation uses aggregate league data from 2025 to sample outcomes
 * based on the current game state (outs and base runners).
 */
class MatrixEventSource : public EventSource {
public:
    /**
     * @brief Constructs a MatrixEventSource.
     * @param dataDir The directory containing the matrix CSV files.
     * @param seed The seed for the random number generator.
     */
    explicit MatrixEventSource(const std::filesystem::path& dataDir, unsigned int seed = std::random_device{}());
    ~MatrixEventSource() override = default;

    // Disable copying
    MatrixEventSource(const MatrixEventSource&) = delete;
    auto operator=(const MatrixEventSource&) -> MatrixEventSource& = delete;

    // Support move semantics
    MatrixEventSource(MatrixEventSource&&) noexcept = default;
    auto operator=(MatrixEventSource&&) noexcept -> MatrixEventSource& = default;

    /// @inheritdoc
    auto Next(const IGameState& state) -> std::optional<Record> override;

private:
    void LoadMatrices(const std::filesystem::path& dataDir);
    [[nodiscard]] auto GetMatrixKey(const IGameState& state) const -> std::string;
    [[nodiscard]] auto TranslateBaseAction(const MatrixOutcome& outcome) const -> std::string;
    [[nodiscard]] auto TranslateAdvancement(int base, const std::string& endBaseName) const -> std::string;
    [[nodiscard]] auto TranslateAdvancements(const MatrixOutcome& outcome, const IGameState& state) const
        -> std::string;
    [[nodiscard]] auto GenerateRetrosheetText(const MatrixOutcome& outcome, const IGameState& state) const
        -> std::string;

    std::map<std::string, std::vector<MatrixOutcome>> matrices;
    std::map<std::string, std::discrete_distribution<size_t>> distributions;
    std::mt19937 rng{};
};

#endif // BASEBALL_CPP_MATRIXEVENTSOURCE_HPP
