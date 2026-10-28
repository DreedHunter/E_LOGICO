// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/model/UndoManager.h"

namespace elogico
{

void UndoManager::perform(CommandPtr command, Project& project)
{
    command->apply(project);
    undone_.clear();

    if (allowMerge_ && !done_.empty() && done_.back()->mergeWith(*command))
        return;

    done_.push_back(std::move(command));
    if (done_.size() > maxSteps_)
        done_.erase(done_.begin());
    allowMerge_ = true;
}

bool UndoManager::undo(Project& project)
{
    if (done_.empty())
        return false;
    auto command = std::move(done_.back());
    done_.pop_back();
    command->undo(project);
    undone_.push_back(std::move(command));
    allowMerge_ = false;
    return true;
}

bool UndoManager::redo(Project& project)
{
    if (undone_.empty())
        return false;
    auto command = std::move(undone_.back());
    undone_.pop_back();
    command->apply(project);
    done_.push_back(std::move(command));
    allowMerge_ = false;
    return true;
}

void UndoManager::clear()
{
    done_.clear();
    undone_.clear();
    allowMerge_ = false;
}

} // namespace elogico
