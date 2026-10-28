// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/platform/ThreadPriority.h"

#if defined(_WIN32)
#include <windows.h>
#include <avrt.h>
#endif

namespace elogico
{

ScopedProAudioThread::ScopedProAudioThread()
{
#if defined(_WIN32)
    DWORD taskIndex = 0;
    handle_ = AvSetMmThreadCharacteristicsW(L"Pro Audio", &taskIndex);
    if (handle_ != nullptr)
        AvSetMmThreadPriority(handle_, AVRT_PRIORITY_HIGH);
#endif
}

ScopedProAudioThread::~ScopedProAudioThread()
{
#if defined(_WIN32)
    if (handle_ != nullptr)
        AvRevertMmThreadCharacteristics(handle_);
#endif
}

void disableProcessPowerThrottling()
{
#if defined(_WIN32) && defined(PROCESS_POWER_THROTTLING_CURRENT_VERSION)
    PROCESS_POWER_THROTTLING_STATE state{};
    state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
    state.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
    state.StateMask = 0; // 0 = throttling disattivato
    SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling, &state, sizeof(state));
#endif
}

} // namespace elogico
