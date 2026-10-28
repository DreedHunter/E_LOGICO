# SPDX-License-Identifier: AGPL-3.0-or-later
juce_add_gui_app(ELogico
    PRODUCT_NAME "E_LOGICO"
    COMPANY_NAME "E_LOGICO"
    VERSION ${PROJECT_VERSION}
    NEEDS_WEB_BROWSER FALSE
    NEEDS_CURL FALSE)

target_sources(ELogico PRIVATE Main.cpp AppController.cpp AppController.h)

target_compile_definitions(ELogico PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:ELogico,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:ELogico,JUCE_VERSION>")

target_link_libraries(ELogico
    PRIVATE
        elogico::gui
        elogico::platform
        elogico::io
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

set_target_properties(ELogico PROPERTIES FOLDER "app")
