// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/core/Types.h"

namespace elogico
{

/// Stato del trasporto, posseduto e modificato solo dal thread audio.
struct Transport
{
    bool playing = false;
    bool looping = false;
    SamplePos position = 0;
    SamplePos loopStart = 0;
    SamplePos loopEnd = 0;

    /// Avanza di numFrames, gestendo il ciclo. Ritorna la posizione di inizio blocco.
    /// TODO(MVP): spezzare il blocco al punto di loop invece di saltare a fine blocco.
    SamplePos advance(int numFrames) noexcept
    {
        const SamplePos start = position;
        if (!playing)
            return start;
        position += numFrames;
        if (looping && loopEnd > loopStart && position >= loopEnd)
            position = loopStart + (position - loopEnd);
        return start;
    }
};

} // namespace elogico
