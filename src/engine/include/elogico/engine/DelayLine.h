// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/engine/AudioBuffer.h"

#include <algorithm>
#include <vector>

namespace elogico
{

/// Linea di ritardo multicanale a lunghezza fissa, usata dalla PDC sui rami più corti.
class DelayLine
{
public:
    void prepare(int numChannels, int delaySamples, int maxBlock)
    {
        delay_ = delaySamples;
        size_ = delaySamples + maxBlock;
        writePos_ = 0;
        data_.assign(std::size_t(numChannels), std::vector<float>(std::size_t(size_), 0.0f));
    }

    int delay() const noexcept { return delay_; }

    /// Scrive src nella linea e somma in dst l'uscita ritardata di delay() campioni.
    void processAdd(const AudioBuffer& src, const AudioBuffer& dst) noexcept
    {
        const int frames = src.numFrames();
        const int channels = std::min(std::min(src.numChannels(), dst.numChannels()), int(data_.size()));
        for (int c = 0; c < channels; ++c)
        {
            auto& line = data_[std::size_t(c)];
            const float* in = src.channel(c);
            float* out = dst.channel(c);
            int w = writePos_;
            int r = (writePos_ - delay_ + size_) % size_;
            for (int i = 0; i < frames; ++i)
            {
                line[std::size_t(w)] = in[i];
                out[i] += line[std::size_t(r)];
                if (++w == size_) w = 0;
                if (++r == size_) r = 0;
            }
        }
        writePos_ = (writePos_ + frames) % size_;
    }

private:
    std::vector<std::vector<float>> data_;
    int delay_ = 0;
    int size_ = 1;
    int writePos_ = 0;
};

} // namespace elogico
