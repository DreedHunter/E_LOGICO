// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/model/Command.h"

#include <vector>

namespace elogico
{

class UndoManager
{
public:
    explicit UndoManager(std::size_t maxSteps = 500) : maxSteps_(maxSteps) {}

    /// Applica il comando e lo registra. I redo pendenti vengono scartati.
    void perform(CommandPtr command, Project& project);
    bool undo(Project& project);
    bool redo(Project& project);

    bool canUndo() const noexcept { return !done_.empty(); }
    bool canRedo() const noexcept { return !undone_.empty(); }
    std::string undoDescription() const { return canUndo() ? done_.back()->description() : std::string{}; }
    std::string redoDescription() const { return canRedo() ? undone_.back()->description() : std::string{}; }
    const Command* lastPerformed() const noexcept { return done_.empty() ? nullptr : done_.back().get(); }

    /// Impedisce che il prossimo comando si unisca al precedente (es. fine del gesto sul fader).
    void beginNewTransaction() noexcept { allowMerge_ = false; }
    void clear();

private:
    std::vector<CommandPtr> done_;
    std::vector<CommandPtr> undone_;
    std::size_t maxSteps_;
    bool allowMerge_ = false;
};

} // namespace elogico
