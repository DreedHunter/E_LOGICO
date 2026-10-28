// SPDX-License-Identifier: AGPL-3.0-or-later
#include "AsioDevice.h"

#include <asiodrivers.h>

namespace elogico
{

std::vector<AudioDeviceInfo> AsioBackend::listDevices()
{
    std::vector<AudioDeviceInfo> result;
    AsioDrivers drivers;
    char names[32][32] = {};
    char* pointers[32];
    for (int i = 0; i < 32; ++i)
        pointers[i] = names[i];
    const long count = drivers.getDriverNames(pointers, 32);
    for (long i = 0; i < count; ++i)
    {
        AudioDeviceInfo info;
        info.id = names[i];
        info.name = names[i];
        result.push_back(std::move(info));
    }
    return result;
}

std::unique_ptr<AudioDevice> AsioBackend::open(const AudioDeviceConfig&, std::string& errorOut)
{
    errorOut = "backend ASIO: apertura del driver non ancora implementata";
    return nullptr;
}

} // namespace elogico
