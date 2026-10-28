// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/platform/AudioDevice.h"

#include <memory>
#include <vector>

namespace elogico
{

/// Elenco dei backend disponibili su questa build. WASAPI condiviso è il default al primo avvio.
class DeviceManager
{
public:
    DeviceManager();

    const std::vector<std::unique_ptr<AudioBackend>>& backends() const noexcept { return backends_; }
    AudioBackend* findBackend(const std::string& name) const;

private:
    std::vector<std::unique_ptr<AudioBackend>> backends_;
};

} // namespace elogico
