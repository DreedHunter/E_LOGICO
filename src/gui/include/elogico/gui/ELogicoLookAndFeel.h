// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace elogico::gui
{

/// Aspetto originale di E_LOGICO: stessa organizzazione dello schermo di una DAW classica,
/// grafica e icone nostre (nessun asset Apple).
class ELogicoLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    ELogicoLookAndFeel();

    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& background,
                              bool highlighted, bool down) override;
    void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height, float sliderPos, float minPos,
                          float maxPos, juce::Slider::SliderStyle, juce::Slider&) override;
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height, float sliderPos,
                          float startAngle, float endAngle, juce::Slider&) override;
};

} // namespace elogico::gui
