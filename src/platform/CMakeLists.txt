# SPDX-License-Identifier: AGPL-3.0-or-later
# I/O Windows: driver audio (WASAPI, ASIO), MIDI, priorità dei thread.
set(platform_sources
    src/DeviceManager.cpp
    src/ThreadPriority.cpp
    include/elogico/platform/AudioDevice.h
    include/elogico/platform/DeviceManager.h
    include/elogico/platform/ThreadPriority.h)

if(WIN32)
    list(APPEND platform_sources src/wasapi/WasapiDevice.cpp src/wasapi/WasapiDevice.h)
endif()

set(asio_enabled OFF)
if(WIN32 AND ELOGICO_ASIO_SDK_DIR AND EXISTS "${ELOGICO_ASIO_SDK_DIR}/common/asio.h")
    set(asio_enabled ON)
    list(APPEND platform_sources src/asio/AsioDevice.cpp src/asio/AsioDevice.h
        "${ELOGICO_ASIO_SDK_DIR}/common/asio.cpp"
        "${ELOGICO_ASIO_SDK_DIR}/host/asiodrivers.cpp"
        "${ELOGICO_ASIO_SDK_DIR}/host/pc/asiolist.cpp")
endif()

elogico_add_module(platform
    SOURCES ${platform_sources}
    DEPS elogico::core)

if(WIN32)
    target_link_libraries(elogico_platform PRIVATE ole32 avrt winmm ksuser)
endif()

if(asio_enabled)
    target_compile_definitions(elogico_platform PRIVATE ELOGICO_HAS_ASIO=1)
    target_include_directories(elogico_platform PRIVATE
        "${ELOGICO_ASIO_SDK_DIR}/common" "${ELOGICO_ASIO_SDK_DIR}/host" "${ELOGICO_ASIO_SDK_DIR}/host/pc")
    message(STATUS "E_LOGICO: backend ASIO attivo (${ELOGICO_ASIO_SDK_DIR})")
else()
    message(STATUS "E_LOGICO: backend ASIO disattivato (imposta ELOGICO_ASIO_SDK_DIR)")
endif()
