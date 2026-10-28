// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/gui/ControlBar.h"
#include "elogico/gui/ELogicoLookAndFeel.h"
#include "elogico/gui/MixerPanel.h"
#include "elogico/gui/TracksArea.h"

#include <juce_gui_basics/juce_gui_basics.h>

namespace elogico::gui
{

/// Finestra principale: barra di controllo in alto, area tracce al centro, mixer in basso.
/// TODO(MVP): libreria (Y), inspector (I), browser (F), piano roll (P), smart controls (B).
class MainComponent final : public juce::Component
{
public:
    MainComponent(ProjectDocument& document, Engine& engine);
    ~MainComponent() override;

    void resized() override;
    bool keyPressed(const juce::KeyPress&) override;

private:
    ProjectDocument& document_;
    Engine& engine_;
    ELogicoLookAndFeel lookAndFeel_;
    ControlBar controlBar_;
    TracksArea tracks_;
    MixerPanel mixer_;
    bool mixerVisible_ = true;
};

} // namespace elogico::gui
