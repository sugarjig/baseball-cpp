#ifndef BASEBALL_CPP_SCOREBOOK_HPP
#define BASEBALL_CPP_SCOREBOOK_HPP

#include "Game.hpp"
#include <filesystem>

struct cw_scorebook_struct; // NOLINT(readability-identifier-naming)
using CWScorebook = cw_scorebook_struct;

namespace chadwick {

/**
 * @brief A container for multiple baseball games, backed by Chadwick's CWScorebook.
 *
 * The Scorebook class allows reading from and writing to Retrosheet event files,
 * managing the underlying Chadwick scorebook resource.
 */
class Scorebook {
public:
    /**
     * @brief Constructs an empty Scorebook.
     */
    Scorebook();
    /**
     * @brief Destructor that ensures proper cleanup of Chadwick resources.
     */
    ~Scorebook();

    // Disable copying
    Scorebook(const Scorebook&) = delete;
    auto operator=(const Scorebook&) -> Scorebook& = delete;

    /**
     * @brief Move constructor for Scorebook.
     * @param other The Scorebook to move from.
     */
    Scorebook(Scorebook&& other) noexcept;
    /**
     * @brief Move assignment operator for Scorebook.
     * @param other The Scorebook to move from.
     * @return A reference to this Scorebook.
     */
    auto operator=(Scorebook&& other) noexcept -> Scorebook&;

    /**
     * @brief Adds a game to the scorebook.
     * @param game The game to add (transfers ownership).
     */
    void AddGame(Game&& game) const;

    /**
     * @brief Reads Retrosheet data from a file into the scorebook.
     * @param path The path to the Retrosheet file.
     * @return Number of games read, or -1 on error.
     */
    [[nodiscard]] auto Read(const std::filesystem::path& path) const -> int;

    /**
     * @brief Writes all games in the scorebook to a Retrosheet file.
     * @param path The path to the file to write.
     * @return True if successful, false otherwise.
     */
    [[nodiscard]] auto Write(const std::filesystem::path& path) const -> bool;

private:
    CWScorebook* scorebook = nullptr;
};

} // namespace chadwick

#endif // BASEBALL_CPP_SCOREBOOK_HPP
