// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

namespace elogico
{

/// Contratto tra driver (modulo platform) e motore (modulo engine).
/// audioCallback è chiamato dal thread del driver: niente allocazioni, lock o I/O.
class AudioCallback
{
public:
    virtual ~AudioCallback() = default;

    /// Chiamato prima dell'avvio dello stream, fuori dal thread audio.
    virtual void audioDeviceAboutToStart(double sampleRate, int maxBlockSize, int numInputs, int numOutputs) = 0;

    virtual void audioCallback(const float* const* inputs, int numInputs,
                               float* const* outputs, int numOutputs, int numFrames) noexcept = 0;

    virtual void audioDeviceStopped() = 0;
};

} // namespace elogico
