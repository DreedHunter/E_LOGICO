// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/engine/Engine.h"
#include "elogico/model/ProjectDocument.h"

#include <juce_gui_basics/juce_gui_basics.h>

namespace elogico::gui
{

/// Area tracce: righello, intestazioni (colore, numero, nome, M/S), corsie con regioni e playhead.
/// Ogni modifica passa da un comando del ProjectDocument, quindi è annullabile.
/// TODO(MVP): scroll e zoom con Viewport, forme d'onda, miniature MIDI, strumenti puntatore/matita/forbici.
class TracksArea final : public juce::Component, private juce::Timer
{
public:
    TracksArea(ProjectDocument& document, Engine& engine);

    void paint(juce::Graphics&) override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;

    void projectChanged() { repaint(); }

private:
    static constexpr int kHeaderWidth = 240;
    static constexpr int kRulerHeight = 34;
    static constexpr int kTrackHeight = 60;

    void timerCallback() override;
    float pixelsPerTick() const noexcept;
    Ticks ticksPerBar() const noexcept;
    float playheadX() const;

    ProjectDocument& document_;
    Engine& engine_;
    float pixelsPerBar_ = 58.0f;

    ObjectId selectedTrack_ = kInvalidId;
    ObjectId draggedRegion_ = kInvalidId;
    Ticks dragStartTicks_ = 0;
    Ticks dragPreviewTicks_ = 0;
    float lastPlayheadX_ = -1.0f;
};

} // namespace elogico::gui
