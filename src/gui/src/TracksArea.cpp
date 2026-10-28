// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/gui/TracksArea.h"

#include "elogico/gui/Theme.h"
#include "elogico/model/Commands.h"

namespace elogico::gui
{

TracksArea::TracksArea(ProjectDocument& document, Engine& engine) : document_(document), engine_(engine)
{
    startTimerHz(60);
}

Ticks TracksArea::ticksPerBar() const noexcept
{
    const auto& sig = document_.project().timeSignature;
    return kTicksPerQuarter * 4 * sig.numerator / sig.denominator;
}

float TracksArea::pixelsPerTick() const noexcept
{
    return pixelsPerBar_ / float(ticksPerBar());
}

float TracksArea::playheadX() const
{
    const double quarters = engine_.tempoMap().samplesToPpq(engine_.feedback().position.load());
    return float(kHeaderWidth) + float(quarters * kTicksPerQuarter) * pixelsPerTick();
}

void TracksArea::paint(juce::Graphics& g)
{
    const auto& t = Theme::get();
    const auto& project = document_.project();
    const bool anySolo = project.anySoloed();
    g.fillAll(t.bg);

    // Righello con numeri di battuta.
    g.setColour(t.panel2);
    g.fillRect(0, 0, getWidth(), kRulerHeight);
    g.setFont(Theme::monoFont(10.0f, true));
    for (int bar = 0; float(kHeaderWidth) + float(bar) * pixelsPerBar_ < float(getWidth()); ++bar)
    {
        const float x = float(kHeaderWidth) + float(bar) * pixelsPerBar_;
        g.setColour(t.fgFaint);
        g.drawVerticalLine(int(x), 16.0f, float(kRulerHeight));
        g.setColour(t.fgDim);
        g.drawText(juce::String(bar + 1), int(x) + 3, 16, 40, 14, juce::Justification::centredLeft);
    }

    int y = kRulerHeight;
    int index = 0;
    for (const auto& track : project.tracks)
    {
        const bool silenced = track.muted || (anySolo && !track.soloed);
        const juce::Colour colour(track.colour);
        auto header = juce::Rectangle<int>(0, y, kHeaderWidth, kTrackHeight);

        // Intestazione traccia.
        g.setColour(track.id == selectedTrack_ ? juce::Colour(0xff253246) : t.panel);
        g.fillRect(header);
        g.setColour(colour);
        g.fillRect(header.removeFromLeft(6));
        g.setColour(t.fgFaint);
        g.setFont(Theme::monoFont(10.0f));
        g.drawText(juce::String(index + 1), header.removeFromLeft(26), juce::Justification::centred);
        auto buttons = header.removeFromRight(52).withSizeKeepingCentre(44, 16);
        g.setColour(silenced ? t.fg.withAlpha(0.45f) : t.fg);
        g.setFont(Theme::uiFont(13.0f, true));
        g.drawText(juce::String::fromUTF8(track.name.c_str()), header.reduced(4, 0), juce::Justification::centredLeft);

        auto m = buttons.removeFromLeft(20), s = buttons.removeFromRight(20);
        g.setColour(track.muted ? t.mute : t.panel3);
        g.fillRoundedRectangle(m.toFloat(), 3.0f);
        g.setColour(track.soloed ? t.solo : t.panel3);
        g.fillRoundedRectangle(s.toFloat(), 3.0f);
        g.setColour(t.fg);
        g.setFont(Theme::uiFont(10.0f, true));
        g.drawText("M", m, juce::Justification::centred);
        g.drawText("S", s, juce::Justification::centred);

        // Corsia con griglia e regioni.
        const auto lane = juce::Rectangle<int>(kHeaderWidth, y, getWidth() - kHeaderWidth, kTrackHeight);
        g.setColour(index % 2 ? juce::Colours::white.withAlpha(0.012f) : juce::Colours::transparentBlack);
        g.fillRect(lane);
        g.setColour(t.grid);
        for (float x = float(kHeaderWidth); x < float(getWidth()); x += pixelsPerBar_)
            g.drawVerticalLine(int(x), float(y), float(y + kTrackHeight));

        for (const auto& region : track.regions)
        {
            const Ticks start = region.id == draggedRegion_ ? dragPreviewTicks_ : region.start;
            auto r = juce::Rectangle<float>(float(kHeaderWidth) + float(start) * pixelsPerTick(), float(y + 3),
                                            float(region.length) * pixelsPerTick() - 1.0f, float(kTrackHeight - 6));
            const auto c = silenced ? colour.withSaturation(0.15f).darker(0.4f) : colour;
            g.setColour(c.withAlpha(0.38f));
            g.fillRoundedRectangle(r, 4.0f);
            g.setColour(c);
            g.fillRoundedRectangle(r.removeFromTop(14.0f), 4.0f);
            g.setColour(juce::Colour(0xff111111));
            g.setFont(Theme::uiFont(10.5f, true));
            g.drawText(juce::String::fromUTF8(region.name.c_str()), r.withY(r.getY() - 14.0f).withHeight(14.0f).reduced(5.0f, 0.0f),
                       juce::Justification::centredLeft);
        }

        g.setColour(t.line);
        g.drawHorizontalLine(y + kTrackHeight - 1, 0.0f, float(getWidth()));
        y += kTrackHeight;
        ++index;
    }

    g.setColour(t.line);
    g.drawVerticalLine(kHeaderWidth - 1, 0.0f, float(getHeight()));

    const float px = playheadX();
    g.setColour(juce::Colours::white);
    g.drawVerticalLine(int(px), 0.0f, float(getHeight()));
}

void TracksArea::mouseDown(const juce::MouseEvent& e)
{
    const auto& project = document_.project();
    if (e.y < kRulerHeight)
    {
        // Clic sul righello: sposta il playhead.
        const float ticks = float(e.x - kHeaderWidth) / pixelsPerTick();
        engine_.locate(engine_.tempoMap().ticksToSamples(Ticks(juce::jmax(0.0f, ticks))));
        return;
    }

    const int row = (e.y - kRulerHeight) / kTrackHeight;
    if (row < 0 || row >= int(project.tracks.size()))
        return;
    const auto& track = project.tracks[std::size_t(row)];
    selectedTrack_ = track.id;

    if (e.x < kHeaderWidth)
    {
        // Pulsante M nell'intestazione.
        const int buttonsLeft = kHeaderWidth - 52 + 4;
        if (e.x >= buttonsLeft && e.x < buttonsLeft + 20)
            document_.perform(std::make_unique<SetTrackMuteCommand>(track.id, !track.muted));
        // TODO(MVP): comando solo.
        repaint();
        return;
    }

    const Ticks clicked = Ticks(float(e.x - kHeaderWidth) / pixelsPerTick());
    for (const auto& region : track.regions)
        if (clicked >= region.start && clicked < region.start + region.length)
        {
            draggedRegion_ = region.id;
            dragStartTicks_ = dragPreviewTicks_ = region.start;
            break;
        }
    repaint();
}

void TracksArea::mouseDrag(const juce::MouseEvent& e)
{
    if (draggedRegion_ == kInvalidId)
        return;
    // Aggancio alla battuta, come la griglia "Battuta" del mockup.
    const Ticks delta = Ticks(float(e.getDistanceFromDragStartX()) / pixelsPerTick());
    const Ticks bar = ticksPerBar();
    dragPreviewTicks_ = juce::jmax<Ticks>(0, (dragStartTicks_ + delta + bar / 2) / bar * bar);
    repaint();
}

void TracksArea::mouseUp(const juce::MouseEvent&)
{
    if (draggedRegion_ != kInvalidId && dragPreviewTicks_ != dragStartTicks_)
        document_.perform(std::make_unique<MoveRegionCommand>(draggedRegion_, dragPreviewTicks_));
    draggedRegion_ = kInvalidId;
    repaint();
}

void TracksArea::timerCallback()
{
    const float px = playheadX();
    if (px != lastPlayheadX_)
    {
        repaint(juce::Rectangle<int>(int(lastPlayheadX_) - 1, 0, 3, getHeight()));
        repaint(juce::Rectangle<int>(int(px) - 1, 0, 3, getHeight()));
        lastPlayheadX_ = px;
    }
}

} // namespace elogico::gui
