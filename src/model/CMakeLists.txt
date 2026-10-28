# SPDX-License-Identifier: AGPL-3.0-or-later
# Modello progetto: unica fonte di verità. La GUI lo modifica solo tramite comandi (undo/redo).
elogico_add_module(model
    SOURCES
        src/Project.cpp
        src/UndoManager.cpp
        src/Commands.cpp
        src/ProjectDocument.cpp
        src/EngineGraphBuilder.cpp
        include/elogico/model/Project.h
        include/elogico/model/Command.h
        include/elogico/model/Commands.h
        include/elogico/model/UndoManager.h
        include/elogico/model/ProjectDocument.h
        include/elogico/model/EngineGraphBuilder.h
    DEPS elogico::core elogico::engine)
