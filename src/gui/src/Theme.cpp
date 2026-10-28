// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/gui/Theme.h"

namespace elogico::gui
{

const Theme& Theme::get()
{
    static const Theme theme;
    return theme;
}

juce::Font Theme::uiFont(float height, bool bold)
{
    return juce::Font(juce::FontOptions("Segoe UI", height, bold ? juce::Font::bold : juce::Font::plain));
}

juce::Font Theme::monoFont(float height, bool bold)
{
    return juce::Font(juce::FontOptions("Consolas", height, bold ? juce::Font::bold : juce::Font::plain));
}

} // namespace elogico::gui
