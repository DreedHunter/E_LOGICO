# SPDX-License-Identifier: AGPL-3.0-or-later
# Test senza dipendenze esterne: ogni file è un eseguibile registrato in CTest.

function(elogico_add_test name)
    add_executable(test_${name} ${name}.cpp TestHarness.h)
    target_link_libraries(test_${name} PRIVATE ${ARGN})
    set_target_properties(test_${name} PROPERTIES FOLDER "test")
    add_test(NAME ${name} COMMAND test_${name})
endfunction()

elogico_add_test(spsc_queue elogico::core)
elogico_add_test(tempo_map elogico::core)
elogico_add_test(graph_pdc elogico::engine)
elogico_add_test(engine_render elogico::engine)
elogico_add_test(undo_manager elogico::model)
elogico_add_test(logicx_package elogico::io)
