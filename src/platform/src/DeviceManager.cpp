// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/platform/DeviceManager.h"

#if defined(_WIN32)
#include "wasapi/WasapiDevice.h"
#endif
#if ELOGICO_HAS_ASIO
#include "asio/AsioDevice.h"
#endif

namespace elogico
{

DeviceManager::DeviceManager()
{
#if defined(_WIN32)
    backends_.push_back(std::make_unique<WasapiBackend>());
#endif
#if ELOGICO_HAS_ASIO
    backends_.push_back(std::make_unique<AsioBackend>());
#endif
}

AudioBackend* DeviceManager::findBackend(const std::string& name) const
{
    for (const auto& b : backends_)
        if (b->name() == name)
            return b.get();
    return nullptr;
}

} // namespace elogico
