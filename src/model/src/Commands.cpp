// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/model/Commands.h"

#include <algorithm>

namespace elogico
{

void AddTrackCommand::apply(Project& project)
{
    if (trackId_ == kInvalidId)
        trackId_ = project.allocateId(); // il redo riusa lo stesso id
    Track track;
    track.id = trackId_;
    track.type = type_;
    track.name = name_;
    project.tracks.push_back(std::move(track));
}

void AddTrackCommand::undo(Project& project)
{
    auto& tracks = project.tracks;
    tracks.erase(std::remove_if(tracks.begin(), tracks.end(), [this](const Track& t) { return t.id == trackId_; }),
                 tracks.end());
}

void RemoveTrackCommand::apply(Project& project)
{
    auto& tracks = project.tracks;
    auto it = std::find_if(tracks.begin(), tracks.end(), [this](const Track& t) { return t.id == trackId_; });
    if (it == tracks.end())
        return;
    index_ = std::size_t(it - tracks.begin());
    removed_ = std::move(*it);
    tracks.erase(it);
}

void RemoveTrackCommand::undo(Project& project)
{
    if (!removed_)
        return;
    auto& tracks = project.tracks;
    tracks.insert(tracks.begin() + std::ptrdiff_t(std::min(index_, tracks.size())), std::move(*removed_));
    removed_.reset();
}

void SetTrackVolumeCommand::apply(Project& project)
{
    if (auto* t = project.findTrack(trackId_))
    {
        oldDb_ = t->volumeDb;
        t->volumeDb = newDb_;
    }
}

void SetTrackVolumeCommand::undo(Project& project)
{
    if (auto* t = project.findTrack(trackId_))
        t->volumeDb = oldDb_;
}

bool SetTrackVolumeCommand::mergeWith(const Command& next)
{
    const auto* other = dynamic_cast<const SetTrackVolumeCommand*>(&next);
    if (other == nullptr || other->trackId_ != trackId_)
        return false;
    newDb_ = other->newDb_; // oldDb_ resta quello di inizio gesto
    return true;
}

void SetTrackMuteCommand::apply(Project& project)
{
    if (auto* t = project.findTrack(trackId_))
    {
        previous_ = t->muted;
        t->muted = muted_;
    }
}

void SetTrackMuteCommand::undo(Project& project)
{
    if (auto* t = project.findTrack(trackId_))
        t->muted = previous_;
}

void MoveRegionCommand::apply(Project& project)
{
    if (auto* r = project.findRegion(regionId_))
    {
        oldStart_ = r->start;
        r->start = std::max<Ticks>(0, newStart_);
    }
}

void MoveRegionCommand::undo(Project& project)
{
    if (auto* r = project.findRegion(regionId_))
        r->start = oldStart_;
}

} // namespace elogico
