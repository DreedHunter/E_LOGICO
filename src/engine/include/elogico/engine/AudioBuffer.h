// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/core/Types.h"

#include <cstddef>
#include <memory>
#include <vector>

namespace elogico
{

/// Vista non proprietaria su un blocco audio planare (un puntatore per canale).
/// È ciò che i nodi ricevono in process(): leggono e scrivono in place.
class AudioBuffer
{
public:
    AudioBuffer() = default;
    AudioBuffer(float* const* channels, int numChannels, int numFrames) noexcept
        : channels_(channels), numChannels_(numChannels), numFrames_(numFrames)
    {
    }

    float* channel(int index) const noexcept { return channels_[index]; }
    float* const* channels() const noexcept { return channels_; }
    int numChannels() const noexcept { return numChannels_; }
    int numFrames() const noexcept { return numFrames_; }

    void clear() const noexcept;
    /// Somma src in questo buffer (stessi canali e frame, oppure il minimo comune).
    void addFrom(const AudioBuffer& src) const noexcept;
    void copyFrom(const AudioBuffer& src) const noexcept;
    float peak(int channel) const noexcept;

private:
    float* const* channels_ = nullptr;
    int numChannels_ = 0;
    int numFrames_ = 0;
};

/// Memoria audio proprietaria, allineata a 64 byte. Si alloca solo fuori dal thread audio.
class AudioBlock
{
public:
    AudioBlock() = default;
    AudioBlock(int numChannels, int maxFrames);

    AudioBuffer view(int numFrames) const noexcept { return {pointers_.data(), numChannels_, numFrames}; }
    AudioBuffer view() const noexcept { return view(maxFrames_); }
    int numChannels() const noexcept { return numChannels_; }
    int maxFrames() const noexcept { return maxFrames_; }

private:
    struct AlignedDelete
    {
        void operator()(float* p) const noexcept;
    };

    std::unique_ptr<float, AlignedDelete> storage_;
    std::vector<float*> pointers_;
    int numChannels_ = 0;
    int maxFrames_ = 0;
};

} // namespace elogico
