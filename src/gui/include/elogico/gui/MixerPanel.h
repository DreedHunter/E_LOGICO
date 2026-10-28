// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/model/ProjectDocument.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>
#include <vector>

namespace elogico::gui
{

/// Mixer: una channel strip per traccia. Il fader invia SetTrackVolumeCommand (un solo undo per gesto).
/// TODO(MVP): slot inserti e send, pan, meter dalle atomiche del motore, bus e Stereo Out.
class MixerPanel final : public juce::Component
{
public:
    explicit MixerPanel(ProjectDocument& document);

    void projectChanged();
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    struct Strip;

    ProjectDocument& document_;
    std::vector<std::unique_ptr<Strip>> strips_;
};

} // namespace elogico::gui
