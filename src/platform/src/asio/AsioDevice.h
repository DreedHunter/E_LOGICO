// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/platform/AudioDevice.h"

namespace elogico
{

/// Backend ASIO, compilato solo se ELOGICO_ASIO_SDK_DIR punta all'SDK di Steinberg.
/// TODO(MVP): bufferSwitchTimeInfo, kAsioResetRequest, canali con nome, cambio sample rate.
class AsioBackend final : public AudioBackend
{
public:
    std::string name() const override { return "ASIO"; }
    std::vector<AudioDeviceInfo> listDevices() override;
    std::unique_ptr<AudioDevice> open(const AudioDeviceConfig& config, std::string& errorOut) override;
};

} // namespace elogico
