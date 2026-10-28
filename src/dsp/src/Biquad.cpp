// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/dsp/Biquad.h"

#include <cmath>
#include <numbers>

namespace elogico::dsp
{

void Biquad::setParameters(Type type, double sampleRate, double frequency, double q, double gainDb) noexcept
{
    const double a = std::pow(10.0, gainDb / 40.0);
    const double w0 = 2.0 * std::numbers::pi * frequency / sampleRate;
    const double cosw = std::cos(w0);
    const double alpha = std::sin(w0) / (2.0 * q);

    double b0 = 1, b1 = 0, b2 = 0, a0 = 1, a1 = 0, a2 = 0;
    switch (type)
    {
    case Type::LowPass:
        b0 = (1 - cosw) / 2; b1 = 1 - cosw; b2 = (1 - cosw) / 2;
        a0 = 1 + alpha; a1 = -2 * cosw; a2 = 1 - alpha;
        break;
    case Type::HighPass:
        b0 = (1 + cosw) / 2; b1 = -(1 + cosw); b2 = (1 + cosw) / 2;
        a0 = 1 + alpha; a1 = -2 * cosw; a2 = 1 - alpha;
        break;
    case Type::Peak:
        b0 = 1 + alpha * a; b1 = -2 * cosw; b2 = 1 - alpha * a;
        a0 = 1 + alpha / a; a1 = -2 * cosw; a2 = 1 - alpha / a;
        break;
    case Type::LowShelf:
    {
        const double s = 2 * std::sqrt(a) * alpha;
        b0 = a * ((a + 1) - (a - 1) * cosw + s);
        b1 = 2 * a * ((a - 1) - (a + 1) * cosw);
        b2 = a * ((a + 1) - (a - 1) * cosw - s);
        a0 = (a + 1) + (a - 1) * cosw + s;
        a1 = -2 * ((a - 1) + (a + 1) * cosw);
        a2 = (a + 1) + (a - 1) * cosw - s;
        break;
    }
    case Type::HighShelf:
    {
        const double s = 2 * std::sqrt(a) * alpha;
        b0 = a * ((a + 1) + (a - 1) * cosw + s);
        b1 = -2 * a * ((a - 1) + (a + 1) * cosw);
        b2 = a * ((a + 1) + (a - 1) * cosw - s);
        a0 = (a + 1) - (a - 1) * cosw + s;
        a1 = 2 * ((a - 1) - (a + 1) * cosw);
        a2 = (a + 1) - (a - 1) * cosw - s;
        break;
    }
    }

    b0_ = b0 / a0;
    b1_ = b1 / a0;
    b2_ = b2 / a0;
    a1_ = a1 / a0;
    a2_ = a2 / a0;
}

} // namespace elogico::dsp
