// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/model/Command.h"
#include "elogico/model/Project.h"

#include <optional>

namespace elogico
{

class AddTrackCommand final : public Command
{
public:
    AddTrackCommand(TrackType type, std::string name) : type_(type), name_(std::move(name)) {}

    void apply(Project& project) override;
    void undo(Project& project) override;
    std::string description() const override { return "Nuova traccia"; }
    bool changesGraph() const override { return true; }
    ObjectId trackId() const noexcept { return trackId_; }

private:
    TrackType type_;
    std::string name_;
    ObjectId trackId_ = kInvalidId;
};

class RemoveTrackCommand final : public Command
{
public:
    explicit RemoveTrackCommand(ObjectId trackId) : trackId_(trackId) {}

    void apply(Project& project) override;
    void undo(Project& project) override;
    std::string description() const override { return "Elimina traccia"; }
    bool changesGraph() const override { return true; }

private:
    ObjectId trackId_;
    std::optional<Track> removed_;
    std::size_t index_ = 0;
};

/// Volume della traccia; i movimenti consecutivi dello stesso fader diventano un solo passo di undo.
class SetTrackVolumeCommand final : public Command
{
public:
    SetTrackVolumeCommand(ObjectId trackId, float volumeDb) : trackId_(trackId), newDb_(volumeDb) {}

    void apply(Project& project) override;
    void undo(Project& project) override;
    std::string description() const override { return "Volume"; }
    bool mergeWith(const Command& next) override;

private:
    ObjectId trackId_;
    float newDb_;
    float oldDb_ = 0.0f;
};

class SetTrackMuteCommand final : public Command
{
public:
    SetTrackMuteCommand(ObjectId trackId, bool muted) : trackId_(trackId), muted_(muted) {}

    void apply(Project& project) override;
    void undo(Project& project) override;
    std::string description() const override { return muted_ ? "Muto" : "Togli muto"; }

private:
    ObjectId trackId_;
    bool muted_;
    bool previous_ = false;
};

class MoveRegionCommand final : public Command
{
public:
    MoveRegionCommand(ObjectId regionId, Ticks newStart) : regionId_(regionId), newStart_(newStart) {}

    void apply(Project& project) override;
    void undo(Project& project) override;
    std::string description() const override { return "Sposta regione"; }

private:
    ObjectId regionId_;
    Ticks newStart_;
    Ticks oldStart_ = 0;
};

} // namespace elogico
