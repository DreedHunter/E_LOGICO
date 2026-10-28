# SPDX-License-Identifier: AGPL-3.0-or-later
# Opzioni comuni a tutti i target di E_LOGICO.

add_library(elogico_options INTERFACE)
add_library(elogico::options ALIAS elogico_options)

target_compile_features(elogico_options INTERFACE cxx_std_20)

if(MSVC)
    target_compile_options(elogico_options INTERFACE
        /W4 /permissive- /utf-8 /Zc:__cplusplus /Zc:preprocessor /MP
        $<$<CONFIG:Release,RelWithDebInfo>:/O2 /Oi /fp:fast>)
else()
    target_compile_options(elogico_options INTERFACE
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Release,RelWithDebInfo>:-O3 -ffast-math>)
endif()

if(WIN32)
    # Windows 10 come minimo: serve per IAudioClient3, CPU Sets e power throttling.
    target_compile_definitions(elogico_options INTERFACE
        _WIN32_WINNT=0x0A00 WINVER=0x0A00 NOMINMAX WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS UNICODE _UNICODE)
endif()

# Il DSP sceglie AVX2/AVX-512 a runtime: qui non si forza nessuna ISA,
# i file che ne hanno bisogno la impostano da soli (vedi src/dsp).

# Aggiunge un modulo E_LOGICO come libreria statica con include pubblici in include/.
function(elogico_add_module name)
    cmake_parse_arguments(ARG "" "" "SOURCES;DEPS;PRIVATE_DEPS" ${ARGN})
    add_library(elogico_${name} STATIC ${ARG_SOURCES})
    add_library(elogico::${name} ALIAS elogico_${name})
    target_include_directories(elogico_${name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
    # Le opzioni (warning, define Windows) restano private: non devono finire nei sorgenti JUCE.
    target_compile_features(elogico_${name} PUBLIC cxx_std_20)
    target_link_libraries(elogico_${name}
        PUBLIC ${ARG_DEPS}
        PRIVATE elogico::options ${ARG_PRIVATE_DEPS})
    set_target_properties(elogico_${name} PROPERTIES FOLDER "moduli")
endfunction()
