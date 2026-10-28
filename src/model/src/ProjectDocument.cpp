// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/model/ProjectDocument.h"

namespace elogico
{

void ProjectDocument::perform(CommandPtr command)
{
    const bool graph = command->changesGraph();
    undo_.perform(std::move(command), project_);
    notify({graph});
}

bool ProjectDocument::undo()
{
    // Non sappiamo quale comando torna indietro: nel dubbio ricostruiamo il grafo.
    const bool graph = undo_.lastPerformed() == nullptr || undo_.lastPerformed()->changesGraph();
    if (!undo_.undo(project_))
        return false;
    notify({graph});
    return true;
}

bool ProjectDocument::redo()
{
    if (!undo_.redo(project_))
        return false;
    notify({undo_.lastPerformed()->changesGraph()});
    return true;
}

void ProjectDocument::replace(Project project)
{
    project_ = std::move(project);
    undo_.clear();
    notify({true});
}

void ProjectDocument::notify(const Change& change)
{
    for (const auto& l : listeners_)
        l(change);
}

} // namespace elogico
