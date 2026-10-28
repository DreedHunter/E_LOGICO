# SPDX-License-Identifier: AGPL-3.0-or-later
# E_LOGICO — DAW open source italiana
cmake_minimum_required(VERSION 3.25)

project(E_LOGICO
    VERSION 0.1.0
    DESCRIPTION "DAW open source italiana per Windows"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT WIN32)
    message(WARNING "E_LOGICO è pensato per Windows: driver e GUI potrebbero non compilare altrove.")
endif()

option(ELOGICO_BUILD_GUI   "Compila la GUI JUCE 8 e l'app desktop (scarica JUCE)" ON)
option(ELOGICO_BUILD_TESTS "Compila i test" ON)
option(ELOGICO_WITH_CLAP   "Hosting CLAP (scarica gli header CLAP)" ON)
option(ELOGICO_WITH_VST3   "Hosting VST3 (scarica il VST3 SDK)" OFF)
set(ELOGICO_ASIO_SDK_DIR "" CACHE PATH "Cartella dell'ASIO SDK di Steinberg; vuota = backend ASIO disattivato")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerOptions)
include(Dependencies)

add_subdirectory(src/core)
add_subdirectory(src/dsp)
add_subdirectory(src/engine)
add_subdirectory(src/platform)
add_subdirectory(src/plugins)
add_subdirectory(src/model)
add_subdirectory(src/io)
add_subdirectory(src/tools)

if(ELOGICO_BUILD_GUI)
    add_subdirectory(src/gui)
    add_subdirectory(src/app)
endif()

if(ELOGICO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
