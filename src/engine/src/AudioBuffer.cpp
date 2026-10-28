// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/engine/AudioBuffer.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <new>

namespace elogico
{

void AudioBuffer::clear() const noexcept
{
    for (int c = 0; c < numChannels_; ++c)
        std::memset(channels_[c], 0, sizeof(float) * std::size_t(numFrames_));
}

void AudioBuffer::addFrom(const AudioBuffer& src) const noexcept
{
    const int channels = std::min(numChannels_, src.numChannels());
    const int frames = std::min(numFrames_, src.numFrames());
    for (int c = 0; c < channels; ++c)
    {
        float* dst = channels_[c];
        const float* in = src.channel(c);
        for (int i = 0; i < frames; ++i)
            dst[i] += in[i];
    }
}

void AudioBuffer::copyFrom(const AudioBuffer& src) const noexcept
{
    const int channels = std::min(numChannels_, src.numChannels());
    const int frames = std::min(numFrames_, src.numFrames());
    for (int c = 0; c < channels; ++c)
        std::memcpy(channels_[c], src.channel(c), sizeof(float) * std::size_t(frames));
}

float AudioBuffer::peak(int channel) const noexcept
{
    float p = 0.0f;
    const float* data = channels_[channel];
    for (int i = 0; i < numFrames_; ++i)
        p = std::max(p, std::fabs(data[i]));
    return p;
}

AudioBlock::AudioBlock(int numChannels, int maxFrames) : numChannels_(numChannels), maxFrames_(maxFrames)
{
    // Ogni canale parte su una linea di cache.
    const std::size_t stride = (std::size_t(maxFrames) * sizeof(float) + kBufferAlignment - 1)
                               / kBufferAlignment * kBufferAlignment / sizeof(float);
    const std::size_t bytes = std::max<std::size_t>(stride * std::size_t(numChannels) * sizeof(float), kBufferAlignment);
#if defined(_WIN32)
    auto* raw = static_cast<float*>(_aligned_malloc(bytes, kBufferAlignment));
#else
    auto* raw = static_cast<float*>(std::aligned_alloc(kBufferAlignment, bytes));
#endif
    if (raw == nullptr)
        throw std::bad_alloc();
    std::memset(raw, 0, bytes);
    storage_.reset(raw);
    pointers_.resize(std::size_t(numChannels));
    for (int c = 0; c < numChannels; ++c)
        pointers_[std::size_t(c)] = raw + stride * std::size_t(c);
}

void AudioBlock::AlignedDelete::operator()(float* p) const noexcept
{
#if defined(_WIN32)
    _aligned_free(p);
#else
    std::free(p);
#endif
}

} // namespace elogico
