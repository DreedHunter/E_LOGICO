// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

namespace elogico::dsp
{

/// Rampa lineare a campione per fader e manopole: evita lo zipper noise.
class SmoothedValue
{
public:
    void reset(double sampleRate, double rampSeconds) noexcept
    {
        rampLength_ = rampSeconds > 0.0 ? int(sampleRate * rampSeconds) : 0;
        current_ = target_;
        remaining_ = 0;
    }

    void setTarget(float target) noexcept
    {
        if (target == target_)
            return;
        target_ = target;
        if (rampLength_ <= 0)
        {
            current_ = target;
            remaining_ = 0;
            return;
        }
        remaining_ = rampLength_;
        step_ = (target_ - current_) / float(rampLength_);
    }

    void setImmediate(float value) noexcept
    {
        target_ = current_ = value;
        remaining_ = 0;
    }

    float next() noexcept
    {
        if (remaining_ <= 0)
            return target_;
        --remaining_;
        current_ += step_;
        if (remaining_ == 0)
            current_ = target_;
        return current_;
    }

    bool isSmoothing() const noexcept { return remaining_ > 0; }
    float target() const noexcept { return target_; }

private:
    float current_ = 0.0f;
    float target_ = 0.0f;
    float step_ = 0.0f;
    int remaining_ = 0;
    int rampLength_ = 0;
};

} // namespace elogico::dsp
