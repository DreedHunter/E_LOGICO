// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/engine/DelayLine.h"
#include "elogico/engine/Node.h"

namespace elogico
{

/// Ritardo puro che dichiara la propria latenza: simula un plugin con lookahead nei test della PDC.
class DelayNode final : public Node
{
public:
    explicit DelayNode(int latency) : latency_(latency) {}

    void prepare(double, int maxBlock) override
    {
        line_.prepare(2, latency_, maxBlock);
        scratch_ = AudioBlock(2, maxBlock);
    }

    void process(const AudioBuffer& io, const ProcessContext& context) noexcept override
    {
        const AudioBuffer tmp = scratch_.view(context.numFrames);
        tmp.copyFrom(io);
        io.clear();
        line_.processAdd(tmp, io);
    }

    int latencySamples() const noexcept override { return latency_; }
    const char* name() const noexcept override { return "Delay"; }

private:
    int latency_;
    DelayLine line_;
    AudioBlock scratch_;
};

} // namespace elogico
