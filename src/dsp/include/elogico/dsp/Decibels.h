// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <cmath>

namespace elogico::dsp
{

inline constexpr float kMinusInfinityDb = -144.0f;

inline float dbToGain(float db) noexcept
{
    return db <= kMinusInfinityDb ? 0.0f : std::pow(10.0f, db * 0.05f);
}

inline float gainToDb(float gain) noexcept
{
    return gain <= 0.0f ? kMinusInfinityDb : 20.0f * std::log10(gain);
}

} // namespace elogico::dsp
