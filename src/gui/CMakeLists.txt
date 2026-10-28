# SPDX-License-Identifier: AGPL-3.0-or-later
# GUI JUCE 8 (renderer Direct2D su Windows). Libreria INTERFACE: i sorgenti si compilano
# dentro l'app insieme ai moduli JUCE, così JUCE resta un'unica copia.
add_library(elogico_gui INTERFACE)
add_library(elogico::gui ALIAS elogico_gui)

target_sources(elogico_gui INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Theme.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ELogicoLookAndFeel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ControlBar.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/TracksArea.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/MixerPanel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/MainComponent.cpp")

target_include_directories(elogico_gui INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(elogico_gui INTERFACE
    elogico::model
    elogico::engine
    juce::juce_gui_extra)
