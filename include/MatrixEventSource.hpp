#ifndef BASEBALL_CPP_MATRIXEVENTSOURCE_HPP
#define BASEBALL_CPP_MATRIXEVENTSOURCE_HPP

#include "EventSource.hpp"
#include "IGameState.hpp"
#include "MatrixData.hpp"
#include <optional>
#include <random>
#include <string>
#include <string_view>

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
     * @param data The pre-loaded matrix data and distributions.
     * @param seed The seed for the random number generator.
     */
    explicit MatrixEventSource(MatrixData data, unsigned int seed = std::random_device{}());
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
    [[nodiscard]] static auto GetMatrixKey(const IGameState& state) -> std::string;
    [[nodiscard]] static auto TranslateBaseAction(const MatrixOutcome& outcome) -> std::string;
    [[nodiscard]] static auto TranslateGenericOut(const std::string& loc) -> std::string;
    [[nodiscard]] static auto TranslateStolenBase(const MatrixOutcome& outcome) -> std::string_view;
    [[nodiscard]] static auto TranslateCaughtStealing(const MatrixOutcome& outcome) -> std::string_view;
    [[nodiscard]] static auto TranslatePickoff(const MatrixOutcome& outcome) -> std::string_view;
    [[nodiscard]] static auto TranslateAdvancement(int base, const std::string& endBaseName) -> std::string;
    [[nodiscard]] static auto TranslateAdvancements(const MatrixOutcome& outcome, const IGameState& state)
        -> std::string;
    [[nodiscard]] static auto GenerateRetrosheetText(const MatrixOutcome& outcome, const IGameState& state)
        -> std::string;

    MatrixData data;
    std::mt19937 rng;
};

#endif // BASEBALL_CPP_MATRIXEVENTSOURCE_HPP
