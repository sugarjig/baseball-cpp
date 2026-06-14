#ifndef BASEBALL_CPP_SCOREBOOK_HPP
#define BASEBALL_CPP_SCOREBOOK_HPP

#include "Game.hpp"
#include <filesystem>

struct cw_scorebook_struct; // NOLINT(readability-identifier-naming)
using CWScorebook = cw_scorebook_struct;

namespace chadwick {

class Scorebook {
public:
    Scorebook();
    ~Scorebook();

    // Disable copying
    Scorebook(const Scorebook&) = delete;
    auto operator=(const Scorebook&) -> Scorebook& = delete;

    // Support move semantics
    Scorebook(Scorebook&& other) noexcept;
    auto operator=(Scorebook&& other) noexcept -> Scorebook&;

    void AddGame(Game&& game) const;
    /**
     * Reads retrosheet data from a file into the scorebook.
     * @param path The path to the retrosheet file.
     * @return Number of games read, or -1 on error.
     */
    [[nodiscard]] auto Read(const std::filesystem::path& path) const -> int;
    [[nodiscard]] auto Write(const std::filesystem::path& path) const -> bool;

private:
    CWScorebook* scorebook;
};

} // namespace chadwick

#endif // BASEBALL_CPP_SCOREBOOK_HPP
