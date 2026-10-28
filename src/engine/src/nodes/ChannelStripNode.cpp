// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/engine/nodes/ChannelStripNode.h"

#include "elogico/dsp/Decibels.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace elogico
{

namespace
{
// Legge −3 dB compensata: al centro ogni canale vale 1/sqrt(2), agli estremi 1 e 0.
void panGains(float pan, float& left, float& right) noexcept
{
    const float angle = (std::clamp(pan, -1.0f, 1.0f) + 1.0f) * float(std::numbers::pi) * 0.25f;
    left = std::cos(angle) * float(std::numbers::sqrt2);
    right = std::sin(angle) * float(std::numbers::sqrt2);
}
} // namespace

void ChannelStripNode::prepare(double sampleRate, int)
{
    gainL_.reset(sampleRate, 0.02);
    gainR_.reset(sampleRate, 0.02);
    float l, r;
    panGains(pan_.load(), l, r);
    const float g = dsp::dbToGain(volumeDb_.load());
    gainL_.setImmediate(g * l);
    gainR_.setImmediate(g * r);
}

void ChannelStripNode::process(const AudioBuffer& io, const ProcessContext&) noexcept
{
    float l, r;
    panGains(pan_.load(std::memory_order_relaxed), l, r);
    const float g = muted_.load(std::memory_order_relaxed) ? 0.0f
                                                           : dsp::dbToGain(volumeDb_.load(std::memory_order_relaxed));
    gainL_.setTarget(g * l);
    gainR_.setTarget(g * r);

    const int frames = io.numFrames();
    if (io.numChannels() >= 2)
    {
        float* left = io.channel(0);
        float* right = io.channel(1);
        for (int i = 0; i < frames; ++i)
        {
            left[i] *= gainL_.next();
            right[i] *= gainR_.next();
        }
    }
    else if (io.numChannels() == 1)
    {
        float* mono = io.channel(0);
        for (int i = 0; i < frames; ++i)
        {
            mono[i] *= gainL_.next();
            gainR_.next();
        }
    }

    for (int c = 0; c < std::min(io.numChannels(), 2); ++c)
        peak_[c].store(io.peak(c), std::memory_order_relaxed);
}

} // namespace elogico
