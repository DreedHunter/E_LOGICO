// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <memory>
#include <string>

namespace elogico
{

struct Project;

/// Ogni modifica al progetto è un comando: così undo/redo, autosalvataggio e import
/// funzionano allo stesso modo per qualsiasi operazione.
class Command
{
public:
    virtual ~Command() = default;

    virtual void apply(Project& project) = 0;
    virtual void undo(Project& project) = 0;
    /// Testo per il menu Modifica ("Annulla Sposta regione").
    virtual std::string description() const = 0;

    /// Unisce un comando successivo dello stesso tipo (es. trascinamento di un fader).
    virtual bool mergeWith(const Command&) { return false; }

    /// true se il comando cambia il grafo audio (tracce aggiunte, routing) e non solo parametri.
    virtual bool changesGraph() const { return false; }
};

using CommandPtr = std::unique_ptr<Command>;

} // namespace elogico
