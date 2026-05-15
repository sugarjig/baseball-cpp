#ifndef BASEBALL_CPP_SCOREBOOK_HPP
#define BASEBALL_CPP_SCOREBOOK_HPP

#include <filesystem>
#include "Game.hpp"

struct cw_scorebook_struct;
typedef struct cw_scorebook_struct CWScorebook;

class Scorebook {
public:
    Scorebook();
    ~Scorebook();

    // Disable copying
    Scorebook(const Scorebook&) = delete;
    Scorebook& operator=(const Scorebook&) = delete;

    void AddGame(Game&& game);
    bool Write(const std::filesystem::path& path);

private:
    CWScorebook* scorebook;
};

#endif //BASEBALL_CPP_SCOREBOOK_HPP
