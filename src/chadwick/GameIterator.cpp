#include "chadwick/GameIterator.hpp"
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

extern "C" {
// clang-format off
// ReSharper disable once CppUnusedIncludeDirective
#include "parse.h" // NOLINT(misc-include-cleaner)
// clang-format on
#include "game.h"
#include "gameiter.h"
}

namespace chadwick {

GameIterator::GameIterator(CWGame* game) : iter((game != nullptr) ? cw_gameiter_create(game) : nullptr) {
    gameState.state = (iter != nullptr) ? iter->state : nullptr;
}

GameIterator::~GameIterator() {
    if (iter != nullptr) {
        cw_gameiter_cleanup(iter);
        free(iter); // NOLINT(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory)
    }
}

GameIterator::GameIterator(GameIterator&& other) noexcept : iter(other.iter), gameState(std::move(other.gameState)) {
    other.iter = nullptr;
}

auto GameIterator::operator=(GameIterator&& other) noexcept -> GameIterator& {
    if (this != &other) {
        if (iter != nullptr) {
            cw_gameiter_cleanup(iter);
            free(iter); // NOLINT(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory)
        }
        iter = other.iter;
        gameState = std::move(other.gameState);
        other.iter = nullptr;
    }
    return *this;
}

void GameIterator::UpdateState() {
    if (iter != nullptr) {
        cw_gameiter_reset(iter);
        while (iter->event != nullptr) {
            CWEvent const* currentEvent = iter->event;

            // Save "suspended" comments that Chadwick's cw_gameiter_process_comments might mangle with strtok
            // May be able to remove in versions of Chadwick higher than 0.10.0
            struct SavedComment {
                CWComment* comment;
                std::string originalText;
            };
            std::vector<SavedComment> saved;

            for (CWComment* comment = currentEvent->first_comment; comment != nullptr; comment = comment->next) {
                if (comment->text != nullptr && strncmp(comment->text, "suspended,", suspendedTextSize) == 0) {
                    saved.push_back({.comment = comment, .originalText = comment->text});
                }
            }

            cw_gameiter_next(iter);

            // Restore any mangled comments
            for (auto& [comment, originalText] : saved) {
                strcpy(comment->text, originalText.c_str());
            }
        }
    }
}

auto GameIterator::GetGameState() const -> const IGameState& { return gameState; }

} // namespace chadwick
