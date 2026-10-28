// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace elogico::gui
{

/// Token del tema scuro, gli stessi del mockup GUI.
/// TODO: leggerli da un file JSON così si possono ritoccare senza ricompilare.
struct Theme
{
    juce::Colour bg{0xff15171b};
    juce::Colour panel{0xff1d2026};
    juce::Colour panel2{0xff252931};
    juce::Colour panel3{0xff2e333d};
    juce::Colour line{0xff0d0e11};
    juce::Colour lineSoft{0xff343a45};
    juce::Colour fg{0xffd9dde5};
    juce::Colour fgDim{0xff8a92a1};
    juce::Colour fgFaint{0xff5c6370};
    juce::Colour accent{0xff5aa9ff};
    juce::Colour rec{0xffff4d4d};
    juce::Colour play{0xff3ddc84};
    juce::Colour cycle{0xfff2c14e};
    juce::Colour mute{0xff4ab0ff};
    juce::Colour solo{0xfff2c14e};
    juce::Colour lcdBg{0xff0b1016};
    juce::Colour lcdFg{0xffcfe6ff};
    juce::Colour grid{0xff22262d};

    static const Theme& get();

    /// Barlow / JetBrains Mono quando saranno incorporati nelle risorse (OFL); per ora font di sistema.
    static juce::Font uiFont(float height, bool bold = false);
    static juce::Font monoFont(float height, bool bold = false);
};

} // namespace elogico::gui
