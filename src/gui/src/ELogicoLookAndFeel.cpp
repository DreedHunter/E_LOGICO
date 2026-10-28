// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/gui/ELogicoLookAndFeel.h"

#include "elogico/gui/Theme.h"

namespace elogico::gui
{

ELogicoLookAndFeel::ELogicoLookAndFeel()
{
    const auto& t = Theme::get();
    setColour(juce::ResizableWindow::backgroundColourId, t.bg);
    setColour(juce::DocumentWindow::backgroundColourId, t.bg);
    setColour(juce::TextButton::buttonColourId, t.panel3);
    setColour(juce::TextButton::buttonOnColourId, t.accent.withAlpha(0.35f));
    setColour(juce::TextButton::textColourOffId, t.fgDim);
    setColour(juce::TextButton::textColourOnId, t.fg);
    setColour(juce::Label::textColourId, t.fg);
    setColour(juce::Slider::thumbColourId, t.panel3);
    setColour(juce::Slider::trackColourId, juce::Colours::black);
    setColour(juce::Slider::rotarySliderFillColourId, t.accent);
    setColour(juce::ScrollBar::thumbColourId, t.lineSoft);
    setColour(juce::PopupMenu::backgroundColourId, t.panel2);
    setColour(juce::PopupMenu::textColourId, t.fg);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, t.accent.withAlpha(0.4f));
}

void ELogicoLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& background,
                                              bool highlighted, bool down)
{
    const auto& t = Theme::get();
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);
    auto fill = button.getToggleState() ? button.findColour(juce::TextButton::buttonOnColourId) : background;
    if (down)
        fill = fill.brighter(0.15f);
    else if (highlighted)
        fill = fill.brighter(0.07f);
    g.setColour(fill);
    g.fillRoundedRectangle(bounds, 5.0f);
    g.setColour(t.line);
    g.drawRoundedRectangle(bounds, 5.0f, 1.0f);
}

void ELogicoLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                          float, float, juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (style != juce::Slider::LinearVertical)
    {
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, 0, 0, style, slider);
        return;
    }

    // Fader: binario nero sottile e cappuccio rigato, come nel mockup.
    const float cx = float(x) + float(width) * 0.5f;
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(cx - 1.0f, float(y), 2.0f, float(height), 1.0f);

    const juce::Rectangle<float> cap(cx - 9.0f, sliderPos - 14.0f, 18.0f, 28.0f);
    g.setGradientFill(juce::ColourGradient::vertical(juce::Colour(0xff5a606b), cap.getY(), juce::Colour(0xff3a3f48),
                                                     cap.getBottom()));
    g.fillRoundedRectangle(cap, 3.0f);
    g.setColour(juce::Colour(0xff202328));
    g.fillRect(cap.getX() + 2.0f, cap.getCentreY() - 0.5f, cap.getWidth() - 4.0f, 1.0f);
    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(cap, 3.0f, 1.0f);
}

void ELogicoLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                          float startAngle, float endAngle, juce::Slider&)
{
    const auto& t = Theme::get();
    const auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(2.0f);
    const float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto centre = bounds.getCentre();
    const float angle = startAngle + sliderPos * (endAngle - startAngle);

    juce::Path track, value;
    track.addCentredArc(centre.x, centre.y, radius, radius, 0.0f, startAngle, endAngle, true);
    value.addCentredArc(centre.x, centre.y, radius, radius, 0.0f, startAngle, angle, true);
    g.setColour(juce::Colour(0xff11141a));
    g.strokePath(track, juce::PathStrokeType(3.0f));
    g.setColour(t.accent);
    g.strokePath(value, juce::PathStrokeType(3.0f));

    g.setColour(t.panel3);
    g.fillEllipse(juce::Rectangle<float>(radius * 1.4f, radius * 1.4f).withCentre(centre));
    g.setColour(t.fg);
    g.drawLine(juce::Line<float>(centre, centre.getPointOnCircumference(radius * 0.6f, angle)), 1.5f);
}

} // namespace elogico::gui
