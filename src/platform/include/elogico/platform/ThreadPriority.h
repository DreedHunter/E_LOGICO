// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

namespace elogico
{

/// Registra il thread corrente in MMCSS come "Pro Audio" finché l'oggetto vive.
class ScopedProAudioThread
{
public:
    ScopedProAudioThread();
    ~ScopedProAudioThread();

    ScopedProAudioThread(const ScopedProAudioThread&) = delete;
    ScopedProAudioThread& operator=(const ScopedProAudioThread&) = delete;

    bool active() const noexcept { return handle_ != nullptr; }

private:
    void* handle_ = nullptr;
};

/// Disattiva il power throttling di Windows per il processo (EcoQoS), utile sulle CPU ibride.
/// TODO(MVP): legare i thread RT ai P-core con le CPU Sets.
void disableProcessPowerThrottling();

} // namespace elogico
