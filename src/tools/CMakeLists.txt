# SPDX-License-Identifier: AGPL-3.0-or-later

# Riga di comando per provare motore, driver e import senza GUI.
add_executable(elogico-cli cli/main.cpp)
target_link_libraries(elogico-cli PRIVATE
    elogico::engine elogico::platform elogico::model elogico::io elogico::plugins)
set_target_properties(elogico-cli PROPERTIES FOLDER "strumenti")

# Processo separato di scansione: un plugin che crasha non fa cadere la DAW.
add_executable(elogico-plugin-scanner plugin_scanner/main.cpp)
target_link_libraries(elogico-plugin-scanner PRIVATE elogico::plugins)
set_target_properties(elogico-plugin-scanner PROPERTIES FOLDER "strumenti")
