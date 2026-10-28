// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/gui/MainComponent.h"

namespace elogico::gui
{

MainComponent::MainComponent(ProjectDocument& document, Engine& engine)
    : document_(document), engine_(engine), controlBar_(document, engine), tracks_(document, engine),
      mixer_(document)
{
    setLookAndFeel(&lookAndFeel_);
    addAndMakeVisible(controlBar_);
    addAndMakeVisible(tracks_);
    addAndMakeVisible(mixer_);

    controlBar_.onToggleMixer = [this] {
        mixerVisible_ = !mixerVisible_;
        mixer_.setVisible(mixerVisible_);
        resized();
    };

    document_.addListener([this](const ProjectDocument::Change&) {
        tracks_.projectChanged();
        mixer_.projectChanged();
    });

    setWantsKeyboardFocus(true);
    setSize(1400, 860);
}

MainComponent::~MainComponent()
{
    setLookAndFeel(nullptr);
}

void MainComponent::resized()
{
    auto r = getLocalBounds();
    controlBar_.setBounds(r.removeFromTop(52));
    if (mixerVisible_)
        mixer_.setBounds(r.removeFromBottom(290));
    tracks_.setBounds(r);
}

bool MainComponent::keyPressed(const juce::KeyPress& key)
{
    // Scorciatoie del mockup; TODO: renderle rimappabili.
    if (key == juce::KeyPress::spaceKey)
    {
        engine_.feedback().playing.load() ? engine_.stop() : engine_.play();
        return true;
    }
    if (key == juce::KeyPress('z', juce::ModifierKeys::ctrlModifier, 0))
        return document_.undo();
    if (key == juce::KeyPress('z', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier, 0)
        || key == juce::KeyPress('y', juce::ModifierKeys::ctrlModifier, 0))
        return document_.redo();
    if (key.getTextCharacter() == 'x')
    {
        mixerVisible_ = !mixerVisible_;
        mixer_.setVisible(mixerVisible_);
        resized();
        return true;
    }
    return false;
}

} // namespace elogico::gui
