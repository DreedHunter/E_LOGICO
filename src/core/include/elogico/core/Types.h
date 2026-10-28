// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <cstdint>

namespace elogico
{

/// Posizione sulla timeline in campioni, come nel motore (spec: int64 + tick a 960 PPQN).
using SamplePos = std::int64_t;

/// Tick musicali: 960 per semiminima, la stessa risoluzione di Logic.
using Ticks = std::int64_t;
inline constexpr Ticks kTicksPerQuarter = 960;

/// Identificatore stabile di un oggetto del modello (traccia, regione, bus...).
using ObjectId = std::uint64_t;
inline constexpr ObjectId kInvalidId = 0;

/// Allineamento dei buffer audio: una linea di cache.
inline constexpr std::size_t kBufferAlignment = 64;

} // namespace elogico
