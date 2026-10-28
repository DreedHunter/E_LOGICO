// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/engine/Node.h"

#include <atomic>

namespace elogico
{

/// Generatore sinusoidale: sorgente per test di null, PDC e verifica dei driver.
class SineNode final : public Node
{
public:
    explicit SineNode(float frequency = 440.0f, float amplitude = 0.25f, bool onlyWhenPlaying = false)
        : frequency_(frequency), amplitude_(amplitude), onlyWhenPlaying_(onlyWhenPlaying)
    {
    }

    void setFrequency(float hz) noexcept { frequency_.store(hz, std::memory_order_relaxed); }

    void prepare(double sampleRate, int maxBlock) override;
    void process(const AudioBuffer& io, const ProcessContext& context) noexcept override;
    const char* name() const noexcept override { return "Sine"; }

private:
    std::atomic<float> frequency_;
    float amplitude_;
    bool onlyWhenPlaying_;
    double phase_ = 0.0;
    double sampleRate_ = 48000.0;
};

} // namespace elogico
