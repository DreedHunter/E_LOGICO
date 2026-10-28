// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/engine/Engine.h"
#include "elogico/model/ProjectDocument.h"

#include <juce_gui_basics/juce_gui_basics.h>

namespace elogico::gui
{

/// Barra di controllo: trasporto, display LCD (posizione, tempo, BPM, metro, tonalità), pulsanti dei pannelli.
/// Legge il motore solo dalle atomiche di EngineFeedback, con un timer a 30 Hz.
class ControlBar final : public juce::Component, private juce::Timer
{
public:
    ControlBar(ProjectDocument& document, Engine& engine);

    std::function<void()> onToggleMixer;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void paintLcd(juce::Graphics&, juce::Rectangle<int> area);

    ProjectDocument& document_;
    Engine& engine_;

    juce::TextButton rewind_{"|<"}, stop_{"[]"}, play_{">"}, record_{"o"}, cycle_{"Ciclo"}, mixer_{"Mixer"};
    juce::Rectangle<int> lcdArea_;
    SamplePos shownPosition_ = -1;
};

} // namespace elogico::gui
