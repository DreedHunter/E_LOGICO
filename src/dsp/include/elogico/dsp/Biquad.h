// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

namespace elogico::dsp
{

/// Filtro biquad (forme dell'Audio EQ Cookbook), base delle bande del Channel EQ.
/// TODO(MVP): sostituire con SVF stile Cytomic per la modulazione veloce dei parametri.
class Biquad
{
public:
    enum class Type
    {
        LowPass,
        HighPass,
        Peak,
        LowShelf,
        HighShelf
    };

    void setParameters(Type type, double sampleRate, double frequency, double q, double gainDb) noexcept;
    void reset() noexcept { z1_ = z2_ = 0.0; }

    float process(float x) noexcept
    {
        const double y = b0_ * x + z1_;
        z1_ = b1_ * x - a1_ * y + z2_;
        z2_ = b2_ * x - a2_ * y;
        return float(y);
    }

    void process(float* samples, int numFrames) noexcept
    {
        for (int i = 0; i < numFrames; ++i)
            samples[i] = process(samples[i]);
    }

private:
    double b0_ = 1.0, b1_ = 0.0, b2_ = 0.0, a1_ = 0.0, a2_ = 0.0;
    double z1_ = 0.0, z2_ = 0.0;
};

} // namespace elogico::dsp
