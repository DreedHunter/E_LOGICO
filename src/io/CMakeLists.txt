# SPDX-License-Identifier: AGPL-3.0-or-later
# Import/export: pacchetti .logicx, AAF, Final Cut Pro XML, MIDI, formato nativo.
elogico_add_module(io
    SOURCES
        src/ImporterRegistry.cpp
        src/logicx/LogicxPackage.cpp
        src/logicx/LogicxImporter.cpp
        include/elogico/io/ImportReport.h
        include/elogico/io/Importer.h
        include/elogico/io/ImporterRegistry.h
        include/elogico/io/logicx/LogicxPackage.h
        include/elogico/io/logicx/LogicxImporter.h
    DEPS elogico::core elogico::model)
