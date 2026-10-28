// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/dsp/SmoothedValue.h"
#include "elogico/engine/Node.h"

#include <atomic>

namespace elogico
{

/// Strip di canale minima: volume, pan (legge −3 dB compensata, default di Logic), mute.
/// I parametri si scrivono da qualsiasi thread; il thread audio li legge a inizio blocco.
/// TODO(MVP): inserti, send pre/post fader, meter peak/RMS per la GUI.
class ChannelStripNode final : public Node
{
public:
    explicit ChannelStripNode(const char* name = "Channel") : name_(name) {}

    void setVolumeDb(float db) noexcept { volumeDb_.store(db, std::memory_order_relaxed); }
    void setPan(float pan) noexcept { pan_.store(pan, std::memory_order_relaxed); } // -1 .. +1
    void setMuted(bool muted) noexcept { muted_.store(muted, std::memory_order_relaxed); }

    float peak(int channel) const noexcept { return peak_[channel].load(std::memory_order_relaxed); }

    void prepare(double sampleRate, int maxBlock) override;
    void process(const AudioBuffer& io, const ProcessContext& context) noexcept override;
    const char* name() const noexcept override { return name_; }

private:
    const char* name_;
    std::atomic<float> volumeDb_{0.0f};
    std::atomic<float> pan_{0.0f};
    std::atomic<bool> muted_{false};
    std::atomic<float> peak_[2]{};
    dsp::SmoothedValue gainL_, gainR_;
};

} // namespace elogico
