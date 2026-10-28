// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/core/Types.h"
#include "elogico/engine/EventList.h"

namespace elogico
{

/// Stato del trasporto passato a ogni nodo, come da spec del motore.
struct ProcessContext
{
    SamplePos samplePos = 0;          // posizione timeline in campioni
    double ppqPos = 0.0;              // posizione musicale (960 tick = 1.0)
    double bpm = 120.0;
    double sampleRate = 48000.0;
    int numFrames = 0;                // dimensione del blocco
    bool playing = false;
    const EventList* events = nullptr; // MIDI e automazione, a campione
};

} // namespace elogico
