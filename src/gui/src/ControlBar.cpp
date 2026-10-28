// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/gui/ControlBar.h"

#include "elogico/gui/Theme.h"

#include <cmath>

namespace elogico::gui
{

ControlBar::ControlBar(ProjectDocument& document, Engine& engine) : document_(document), engine_(engine)
{
    for (auto* b : {&rewind_, &stop_, &play_, &record_, &cycle_, &mixer_})
        addAndMakeVisible(b);

    play_.setClickingTogglesState(false);
    cycle_.setClickingTogglesState(true);
    mixer_.setClickingTogglesState(true);
    play_.setColour(juce::TextButton::buttonOnColourId, Theme::get().play.withAlpha(0.35f));
    record_.setColour(juce::TextButton::buttonOnColourId, Theme::get().rec);
    cycle_.setColour(juce::TextButton::buttonOnColourId, Theme::get().cycle);

    rewind_.onClick = [this] { engine_.locate(0); };
    stop_.onClick = [this] {
        // Secondo stop a trasporto fermo: torna all'inizio, come nelle DAW classiche.
        if (!engine_.feedback().playing.load())
            engine_.locate(0);
        engine_.stop();
    };
    play_.onClick = [this] { engine_.feedback().playing.load() ? engine_.stop() : engine_.play(); };
    record_.onClick = [] { /* TODO(MVP): registrazione sulle tracce armate */ };
    cycle_.onClick = [this] {
        engine_.post({EngineCommand::Type::SetLooping, cycle_.getToggleState() ? 1 : 0});
    };
    mixer_.onClick = [this] {
        if (onToggleMixer)
            onToggleMixer();
    };

    startTimerHz(30);
}

void ControlBar::paint(juce::Graphics& g)
{
    const auto& t = Theme::get();
    g.setGradientFill(juce::ColourGradient::vertical(juce::Colour(0xff2a2e36), 0.0f, juce::Colour(0xff22252c),
                                                     float(getHeight())));
    g.fillAll();
    g.setColour(t.line);
    g.fillRect(0, getHeight() - 1, getWidth(), 1);
    paintLcd(g, lcdArea_);
}

void ControlBar::paintLcd(juce::Graphics& g, juce::Rectangle<int> area)
{
    const auto& t = Theme::get();
    const auto& project = document_.project();
    g.setColour(t.lcdBg);
    g.fillRoundedRectangle(area.toFloat(), 6.0f);
    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(area.toFloat(), 6.0f, 1.0f);

    const SamplePos pos = engine_.feedback().position.load();
    const double sr = engine_.sampleRate();
    const double quarters = engine_.tempoMap().samplesToPpq(pos);
    const int beatsPerBar = project.timeSignature.numerator;
    const int bar = int(quarters) / beatsPerBar + 1;
    const int beat = int(quarters) % beatsPerBar + 1;
    const double seconds = double(pos) / sr;

    struct Cell
    {
        juce::String value, label;
        int width;
    };
    const Cell cells[] = {
        {juce::String::formatted("%03d %d", bar, beat), "BATTUTA", 96},
        {juce::String::formatted("%02d:%02d:%04.1f", int(seconds) / 3600, int(seconds) / 60 % 60,
                                 std::fmod(seconds, 60.0)),
         "TEMPO", 120},
        {juce::String(project.bpm, 0), "BPM", 56},
        {juce::String(project.timeSignature.numerator) + "/" + juce::String(project.timeSignature.denominator),
         "METRO", 52},
        {juce::String(project.key), "TONALITÀ", 64},
    };

    auto r = area.reduced(4, 3);
    for (const auto& c : cells)
    {
        auto cell = r.removeFromLeft(c.width);
        g.setColour(t.lcdFg);
        g.setFont(Theme::monoFont(16.0f, true));
        g.drawText(c.value, cell.removeFromTop(cell.getHeight() * 2 / 3), juce::Justification::centredLeft);
        g.setColour(juce::Colour(0xff6f8aa6));
        g.setFont(Theme::uiFont(9.0f));
        g.drawText(c.label, cell, juce::Justification::centredLeft);
    }
}

void ControlBar::resized()
{
    auto r = getLocalBounds().reduced(10, 12);
    mixer_.setBounds(r.removeFromRight(60));
    for (auto* b : {&rewind_, &stop_, &play_, &record_})
    {
        b->setBounds(r.removeFromLeft(36));
        r.removeFromLeft(3);
    }
    cycle_.setBounds(r.removeFromLeft(54));
    r.removeFromLeft(14);
    lcdArea_ = r.removeFromLeft(400).withSizeKeepingCentre(400, 40);
}

void ControlBar::timerCallback()
{
    play_.setToggleState(engine_.feedback().playing.load(), juce::dontSendNotification);
    const SamplePos pos = engine_.feedback().position.load();
    if (pos != shownPosition_)
    {
        shownPosition_ = pos;
        repaint(lcdArea_);
    }
}

} // namespace elogico::gui
