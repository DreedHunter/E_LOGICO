// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/engine/GraphDescription.h"
#include "elogico/engine/nodes/ChannelStripNode.h"
#include "elogico/model/Project.h"

#include <memory>
#include <unordered_map>

namespace elogico
{

/// Traduce il modello progetto nel grafo del motore: una ChannelStripNode per traccia, tutte sul master.
/// Mantiene i nodi tra una ricostruzione e l'altra così lo stato DSP non si azzera.
/// TODO(MVP): regioni (lettori audio e MIDI), bus aux, send, inserti plugin.
class EngineGraphBuilder
{
public:
    static constexpr NodeId kMasterNodeId = 1;

    GraphDescription build(const Project& project);

    /// Aggiorna solo i parametri (volume, pan, mute/solo) senza ricompilare il grafo.
    void syncParameters(const Project& project);

private:
    std::shared_ptr<ChannelStripNode> stripFor(ObjectId trackId);

    std::unordered_map<ObjectId, std::shared_ptr<ChannelStripNode>> strips_;
    std::shared_ptr<ChannelStripNode> master_ = std::make_shared<ChannelStripNode>("Stereo Out");
};

} // namespace elogico
