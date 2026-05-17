#ifndef BASEBALL_CPP_SCOREBOOK_HPP
#define BASEBALL_CPP_SCOREBOOK_HPP

#include "Game.hpp"
#include <filesystem>

struct cw_scorebook_struct;
typedef struct cw_scorebook_struct CWScorebook;

namespace chadwick {

class Scorebook {
public:
    Scorebook();
    ~Scorebook();

    // Disable copying
    Scorebook(const Scorebook&) = delete;
    Scorebook& operator=(const Scorebook&) = delete;

    void AddGame(Game&& game);
    /**
     * Reads retrosheet data from a file into the scorebook.
     * @param path The path to the retrosheet file.
     * @return Number of games read, or -1 on error.
     */
    int Read(const std::filesystem::path& path);
    bool Write(const std::filesystem::path& path);

private:
    CWScorebook* scorebook;
};

} // namespace chadwick

#endif // BASEBALL_CPP_SCOREBOOK_HPP
