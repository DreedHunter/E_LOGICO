// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/core/TempoMap.h"

#include <algorithm>
#include <cmath>

namespace elogico
{

TempoMap::TempoMap(double sampleRate, double initialBpm) : sampleRate_(sampleRate)
{
    changes_.push_back({0, initialBpm});
    rebuild();
}

void TempoMap::setSampleRate(double sampleRate)
{
    sampleRate_ = sampleRate;
    rebuild();
}

void TempoMap::setTempo(Ticks at, double bpm)
{
    auto it = std::lower_bound(changes_.begin(), changes_.end(), at,
                               [](const Change& c, Ticks t) { return c.at < t; });
    if (it != changes_.end() && it->at == at)
        it->bpm = bpm;
    else
        changes_.insert(it, {at, bpm});
    rebuild();
}

double TempoMap::tempoAt(Ticks at) const noexcept
{
    auto it = std::upper_bound(changes_.begin(), changes_.end(), at,
                               [](Ticks t, const Change& c) { return t < c.at; });
    return it == changes_.begin() ? changes_.front().bpm : std::prev(it)->bpm;
}

void TempoMap::rebuild()
{
    segments_.clear();
    double samples = 0.0;
    for (std::size_t i = 0; i < changes_.size(); ++i)
    {
        const double samplesPerTick = sampleRate_ * 60.0 / (changes_[i].bpm * kTicksPerQuarter);
        segments_.push_back({changes_[i].at, samples, samplesPerTick});
        if (i + 1 < changes_.size())
            samples += double(changes_[i + 1].at - changes_[i].at) * samplesPerTick;
    }
}

SamplePos TempoMap::ticksToSamples(Ticks ticks) const noexcept
{
    auto it = std::upper_bound(segments_.begin(), segments_.end(), ticks,
                               [](Ticks t, const Segment& s) { return t < s.startTicks; });
    const Segment& seg = it == segments_.begin() ? segments_.front() : *std::prev(it);
    return SamplePos(std::llround(seg.startSamples + double(ticks - seg.startTicks) * seg.samplesPerTick));
}

Ticks TempoMap::samplesToTicks(SamplePos samples) const noexcept
{
    return Ticks(std::floor(samplesToPpq(samples) * kTicksPerQuarter));
}

double TempoMap::samplesToPpq(SamplePos samples) const noexcept
{
    const double s = double(samples);
    auto it = std::upper_bound(segments_.begin(), segments_.end(), s,
                               [](double v, const Segment& seg) { return v < seg.startSamples; });
    const Segment& seg = it == segments_.begin() ? segments_.front() : *std::prev(it);
    const double ticks = double(seg.startTicks) + (s - seg.startSamples) / seg.samplesPerTick;
    return ticks / double(kTicksPerQuarter);
}

} // namespace elogico
