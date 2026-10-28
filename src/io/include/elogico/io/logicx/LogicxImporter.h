// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/io/Importer.h"

namespace elogico
{

/// Importatore .logicx a livelli (vedi documento di architettura, sezione "Apertura dei progetti .logicx").
/// Oggi: legge la struttura del pacchetto e crea una traccia audio per ogni file in Audio Files,
/// segnalando nel report che tracce, regioni, tempo e mixer di ProjectData non sono ancora decodificati.
/// Il formato di ProjectData si ricava solo da progetti di prova creati apposta (reverse engineering
/// a fini di interoperabilità), mai da codice Apple.
class LogicxImporter final : public Importer
{
public:
    std::string name() const override { return "Logic Pro (.logicx)"; }
    bool canImport(const std::string& path) const override;
    std::optional<Project> import(const std::string& path, ImportReport& report) override;
};

} // namespace elogico
