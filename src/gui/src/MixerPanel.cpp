// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/gui/MixerPanel.h"

#include "elogico/gui/Theme.h"
#include "elogico/model/Commands.h"

namespace elogico::gui
{

struct MixerPanel::Strip final : public juce::Component
{
    Strip(ProjectDocument& doc, const Track& track) : document(doc), trackId(track.id), colour(track.colour)
    {
        name = juce::String::fromUTF8(track.name.c_str());
        fader.setSliderStyle(juce::Slider::LinearVertical);
        fader.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
        fader.setRange(-60.0, 6.0, 0.1);
        fader.setSkewFactorFromMidPoint(-12.0);
        fader.setValue(track.volumeDb, juce::dontSendNotification);
        fader.setDoubleClickReturnValue(true, 0.0);
        fader.setTextValueSuffix(" dB");
        fader.onDragStart = [this] { document.undoManager().beginNewTransaction(); };
        fader.onValueChange = [this] {
            document.perform(std::make_unique<SetTrackVolumeCommand>(trackId, float(fader.getValue())));
        };
        addAndMakeVisible(fader);
    }

    void refresh(const Track& track)
    {
        fader.setValue(track.volumeDb, juce::dontSendNotification);
        name = juce::String::fromUTF8(track.name.c_str());
        colour = juce::Colour(track.colour);
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        const auto& t = Theme::get();
        g.setColour(t.panel);
        g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 4.0f);
        auto nameArea = getLocalBounds().removeFromBottom(22).reduced(4, 3);
        g.setColour(colour);
        g.fillRoundedRectangle(nameArea.toFloat(), 3.0f);
        g.setColour(juce::Colour(0xff111111));
        g.setFont(Theme::uiFont(10.5f, true));
        g.drawText(name, nameArea, juce::Justification::centred, true);
    }

    void resized() override { fader.setBounds(getLocalBounds().reduced(6).withTrimmedBottom(22)); }

    ProjectDocument& document;
    ObjectId trackId;
    juce::Colour colour;
    juce::String name;
    juce::Slider fader;
};

MixerPanel::MixerPanel(ProjectDocument& document) : document_(document)
{
    projectChanged();
}

void MixerPanel::projectChanged()
{
    const auto& tracks = document_.project().tracks;
    bool sameLayout = strips_.size() == tracks.size();
    for (std::size_t i = 0; sameLayout && i < tracks.size(); ++i)
        sameLayout = strips_[i]->trackId == tracks[i].id;

    if (sameLayout)
    {
        for (std::size_t i = 0; i < tracks.size(); ++i)
            strips_[i]->refresh(tracks[i]);
        return;
    }

    strips_.clear();
    for (const auto& track : tracks)
    {
        strips_.push_back(std::make_unique<Strip>(document_, track));
        addAndMakeVisible(*strips_.back());
    }
    resized();
}

void MixerPanel::paint(juce::Graphics& g)
{
    g.fillAll(Theme::get().bg);
}

void MixerPanel::resized()
{
    auto r = getLocalBounds().reduced(6);
    for (auto& s : strips_)
    {
        s->setBounds(r.removeFromLeft(78));
        r.removeFromLeft(2);
    }
}

} // namespace elogico::gui
