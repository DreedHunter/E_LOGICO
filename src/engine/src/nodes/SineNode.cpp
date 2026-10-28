// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/engine/nodes/SineNode.h"

#include <cmath>
#include <numbers>

namespace elogico
{

void SineNode::prepare(double sampleRate, int)
{
    sampleRate_ = sampleRate;
    phase_ = 0.0;
}

void SineNode::process(const AudioBuffer& io, const ProcessContext& context) noexcept
{
    if (onlyWhenPlaying_ && !context.playing)
        return; // il buffer arriva già azzerato dal grafo

    const double increment = 2.0 * std::numbers::pi * frequency_.load(std::memory_order_relaxed) / sampleRate_;
    for (int i = 0; i < io.numFrames(); ++i)
    {
        const float s = amplitude_ * float(std::sin(phase_));
        for (int c = 0; c < io.numChannels(); ++c)
            io.channel(c)[i] += s;
        phase_ += increment;
        if (phase_ >= 2.0 * std::numbers::pi)
            phase_ -= 2.0 * std::numbers::pi;
    }
}

} // namespace elogico
