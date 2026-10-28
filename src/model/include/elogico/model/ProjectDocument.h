// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/model/Command.h"
#include "elogico/model/Project.h"
#include "elogico/model/UndoManager.h"

#include <functional>
#include <vector>

namespace elogico
{

/// Progetto + storia delle modifiche + notifiche. È l'oggetto che la GUI tiene in mano:
/// legge project() e chiama perform() per ogni modifica. Solo thread GUI/messaggi.
class ProjectDocument
{
public:
    struct Change
    {
        bool graphChanged = false; // serve ricostruire il grafo audio
    };
    using Listener = std::function<void(const Change&)>;

    ProjectDocument() = default;
    explicit ProjectDocument(Project project) : project_(std::move(project)) {}

    const Project& project() const noexcept { return project_; }

    void perform(CommandPtr command);
    bool undo();
    bool redo();
    UndoManager& undoManager() noexcept { return undo_; }

    /// Sostituisce l'intero progetto (apertura file, import). Azzera la storia.
    void replace(Project project);

    void addListener(Listener listener) { listeners_.push_back(std::move(listener)); }

private:
    void notify(const Change& change);

    Project project_;
    UndoManager undo_;
    std::vector<Listener> listeners_;
};

} // namespace elogico
