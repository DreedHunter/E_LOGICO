// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/engine/Engine.h"
#include "elogico/model/EngineGraphBuilder.h"
#include "elogico/model/ProjectDocument.h"
#include "elogico/platform/DeviceManager.h"

#include <memory>
#include <string>

namespace elogico
{

/// Collega i livelli: documento -> grafo del motore -> driver audio.
/// La GUI riceve solo document() ed engine() (per il trasporto e le atomiche di feedback).
class AppController
{
public:
    AppController();
    ~AppController();

    ProjectDocument& document() noexcept { return document_; }
    Engine& engine() noexcept { return engine_; }

    /// Apre il device WASAPI di default. Ritorna il messaggio di errore, vuoto se riuscito.
    std::string startAudio();
    void stopAudio();

    /// Da chiamare periodicamente sul thread dei messaggi.
    void idle() { engine_.collectGarbage(); }

    /// Importa un progetto (.logicx...) e lo sostituisce a quello aperto.
    std::string importProject(const std::string& path);

private:
    void rebuildGraph();
    static Project demoProject();

    ProjectDocument document_;
    Engine engine_;
    EngineGraphBuilder graphBuilder_;
    DeviceManager devices_;
    std::unique_ptr<AudioDevice> device_;
};

} // namespace elogico
