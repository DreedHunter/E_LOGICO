// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/platform/AudioDevice.h"

namespace elogico
{

/// Backend WASAPI. MVP attuale: uscita in modalità condivisa, event-driven, float 32 bit.
/// TODO(MVP): ingressi, modalità esclusiva con IAudioClient3 (periodi piccoli), recupero dalla perdita del device.
class WasapiBackend final : public AudioBackend
{
public:
    std::string name() const override { return "WASAPI"; }
    std::vector<AudioDeviceInfo> listDevices() override;
    std::unique_ptr<AudioDevice> open(const AudioDeviceConfig& config, std::string& errorOut) override;
};

} // namespace elogico
