#ifndef BASEBALL_CPP_SCOREBOOK_HPP
#define BASEBALL_CPP_SCOREBOOK_HPP

#include "Game.hpp"
#include <filesystem>

struct cw_scorebook_struct;
using CWScorebook = struct cw_scorebook_struct;

namespace chadwick {

class Scorebook {
public:
    Scorebook();
    ~Scorebook();

    // Disable copying
    Scorebook(const Scorebook&) = delete;
    auto operator=(const Scorebook&) -> Scorebook& = delete;

    // Support move semantics
    Scorebook(Scorebook&&) noexcept;
    auto operator=(Scorebook&&) noexcept -> Scorebook&;

    void AddGame(Game&& game);
    /**
     * Reads retrosheet data from a file into the scorebook.
     * @param path The path to the retrosheet file.
     * @return Number of games read, or -1 on error.
     */
    auto Read(const std::filesystem::path& path) -> int;
    auto Write(const std::filesystem::path& path) -> bool;

private:
    CWScorebook* scorebook;
};

} // namespace chadwick

#endif // BASEBALL_CPP_SCOREBOOK_HPP
