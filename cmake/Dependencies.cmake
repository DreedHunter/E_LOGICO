# SPDX-License-Identifier: AGPL-3.0-or-later
# Dipendenze esterne scaricate al configure con FetchContent.
# Le versioni sono fissate: aggiornarle è una decisione da registrare in docs/adr.

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

if(ELOGICO_BUILD_GUI)
    # JUCE 8 (AGPLv3) — solo GUI e utilità, mai dentro il motore audio.
    FetchContent_Declare(juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        8.0.4
        GIT_SHALLOW    TRUE)
    FetchContent_MakeAvailable(juce)
endif()

if(ELOGICO_WITH_CLAP)
    # Header ufficiali CLAP (MIT).
    FetchContent_Declare(clap
        GIT_REPOSITORY https://github.com/free-audio/clap.git
        GIT_TAG        1.2.2
        GIT_SHALLOW    TRUE)
    FetchContent_MakeAvailable(clap)
endif()

if(ELOGICO_WITH_VST3)
    # VST3 SDK di Steinberg (MIT dalla 3.8). Pesante: per questo è OFF di default.
    FetchContent_Declare(vst3sdk
        GIT_REPOSITORY https://github.com/steinbergmedia/vst3sdk.git
        GIT_TAG        v3.8.0_build_66
        GIT_SHALLOW    TRUE
        GIT_SUBMODULES base pluginterfaces public.sdk)
    set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SMTG_ENABLE_VSTGUI_SUPPORT OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(vst3sdk)
endif()
