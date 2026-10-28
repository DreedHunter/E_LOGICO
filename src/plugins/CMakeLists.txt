# SPDX-License-Identifier: AGPL-3.0-or-later
# Hosting plugin VST3 e CLAP. La scansione gira in un processo separato (src/tools/plugin_scanner).
set(plugin_sources
    src/PluginRegistry.cpp
    include/elogico/plugins/PluginDescription.h
    include/elogico/plugins/PluginFormat.h
    include/elogico/plugins/PluginRegistry.h)

if(ELOGICO_WITH_CLAP)
    list(APPEND plugin_sources src/clap/ClapFormat.cpp src/clap/ClapFormat.h)
endif()
if(ELOGICO_WITH_VST3)
    list(APPEND plugin_sources src/vst3/Vst3Format.cpp src/vst3/Vst3Format.h)
endif()

elogico_add_module(plugins
    SOURCES ${plugin_sources}
    DEPS elogico::core elogico::engine)

if(ELOGICO_WITH_CLAP)
    target_link_libraries(elogico_plugins PRIVATE clap)
    target_compile_definitions(elogico_plugins PRIVATE ELOGICO_HAS_CLAP=1)
endif()
if(ELOGICO_WITH_VST3)
    target_link_libraries(elogico_plugins PRIVATE sdk_hosting)
    target_compile_definitions(elogico_plugins PRIVATE ELOGICO_HAS_VST3=1)
endif()
