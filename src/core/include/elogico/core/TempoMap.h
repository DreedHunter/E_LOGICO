// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/core/Types.h"

#include <vector>

namespace elogico
{

/// Mappa del tempo: cambi di BPM a gradino posizionati in tick.
/// Converte tick <-> campioni; la tabella dei segmenti è ricalcolata a ogni modifica (non RT).
/// TODO(V1): rampe di tempo e cambi di metro.
class TempoMap
{
public:
    struct Change
    {
        Ticks at = 0;
        double bpm = 120.0;
    };

    explicit TempoMap(double sampleRate = 48000.0, double initialBpm = 120.0);

    void setSampleRate(double sampleRate);
    double sampleRate() const noexcept { return sampleRate_; }

    /// Inserisce o sostituisce un cambio di tempo alla posizione indicata.
    void setTempo(Ticks at, double bpm);
    double tempoAt(Ticks at) const noexcept;

    SamplePos ticksToSamples(Ticks ticks) const noexcept;
    Ticks samplesToTicks(SamplePos samples) const noexcept;

    /// Posizione musicale in semiminime (960 tick = 1.0), usata da ProcessContext::ppqPos.
    double samplesToPpq(SamplePos samples) const noexcept;

    const std::vector<Change>& changes() const noexcept { return changes_; }

private:
    struct Segment
    {
        Ticks startTicks;
        double startSamples;
        double samplesPerTick;
    };

    void rebuild();

    double sampleRate_;
    std::vector<Change> changes_;
    std::vector<Segment> segments_;
};

} // namespace elogico
