// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/model/EngineGraphBuilder.h"

namespace elogico
{

namespace
{
// Gli id dei nodi derivano dagli id del modello; 1 è riservato al master.
NodeId nodeIdFor(ObjectId trackId) noexcept
{
    return NodeId(trackId + 1);
}
} // namespace

std::shared_ptr<ChannelStripNode> EngineGraphBuilder::stripFor(ObjectId trackId)
{
    auto& strip = strips_[trackId];
    if (!strip)
        strip = std::make_shared<ChannelStripNode>("Track");
    return strip;
}

GraphDescription EngineGraphBuilder::build(const Project& project)
{
    GraphDescription graph;
    graph.addNode(kMasterNodeId, master_);
    graph.output = kMasterNodeId;

    std::unordered_map<ObjectId, std::shared_ptr<ChannelStripNode>> alive;
    for (const auto& track : project.tracks)
    {
        if (track.type == TrackType::Output)
            continue;
        auto strip = stripFor(track.id);
        alive[track.id] = strip;
        graph.addNode(nodeIdFor(track.id), strip);
        graph.connect(nodeIdFor(track.id), kMasterNodeId);
    }
    strips_ = std::move(alive);

    syncParameters(project);
    return graph;
}

void EngineGraphBuilder::syncParameters(const Project& project)
{
    const bool anySolo = project.anySoloed();
    for (const auto& track : project.tracks)
    {
        auto it = strips_.find(track.id);
        if (it == strips_.end())
            continue;
        it->second->setVolumeDb(track.volumeDb);
        it->second->setPan(track.pan);
        it->second->setMuted(track.muted || (anySolo && !track.soloed));
    }
    master_->setVolumeDb(project.masterVolumeDb);
}

} // namespace elogico
