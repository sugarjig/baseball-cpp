#include "chadwick/GameIterator.hpp"
#include "IGameState.hpp"
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

GameIterator::GameIterator(CWGame* cwGame)
    : cwGameIterator((cwGame != nullptr) ? cw_gameiter_create(cwGame) : nullptr),
      gameState((cwGameIterator != nullptr) ? cwGameIterator->state : nullptr) {}

GameIterator::~GameIterator() {
    if (cwGameIterator != nullptr) {
        cw_gameiter_cleanup(cwGameIterator);
        free(cwGameIterator); // NOLINT(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory)
    }
}

GameIterator::GameIterator(GameIterator&& other) noexcept
    : cwGameIterator(other.cwGameIterator), gameState(std::move(other.gameState)) {
    other.cwGameIterator = nullptr;
}

auto GameIterator::operator=(GameIterator&& other) noexcept -> GameIterator& {
    if (this != &other) {
        if (cwGameIterator != nullptr) {
            cw_gameiter_cleanup(cwGameIterator);
            free(cwGameIterator); // NOLINT(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory)
        }
        cwGameIterator = other.cwGameIterator;
        gameState = std::move(other.gameState);
        other.cwGameIterator = nullptr;
    }
    return *this;
}

void GameIterator::UpdateState() {
    if (cwGameIterator != nullptr) {
        cw_gameiter_reset(cwGameIterator);
        while (cwGameIterator->event != nullptr) {
            CWEvent const* cwEventCurrent = cwGameIterator->event;

            // Save "suspended" comments that Chadwick's cw_gameiter_process_comments might mangle with strtok
            // May be able to remove in versions of Chadwick higher than 0.10.0
            struct SavedComment {
                CWComment* cwComment;
                std::string originalText;
            };
            std::vector<SavedComment> saved;

            for (CWComment* cwComment = cwEventCurrent->first_comment; cwComment != nullptr;
                 cwComment = cwComment->next) {
                if (cwComment->text != nullptr && strncmp(cwComment->text, "suspended,", suspendedTextSize) == 0) {
                    saved.push_back({.cwComment = cwComment, .originalText = cwComment->text});
                }
            }

            cw_gameiter_next(cwGameIterator);

            // Restore any mangled comments
            for (auto& [cwComment, originalText] : saved) {
                strcpy(cwComment->text, originalText.c_str());
            }
        }
    }
}

auto GameIterator::GetGameState() const -> const IGameState& { return gameState; }

} // namespace chadwick
