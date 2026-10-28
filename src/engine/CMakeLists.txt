# SPDX-License-Identifier: AGPL-3.0-or-later
# Motore audio custom: grafo real-time multicore, PDC, trasporto. Nessuna dipendenza da JUCE.
elogico_add_module(engine
    SOURCES
        src/AudioBuffer.cpp
        src/GraphCompiler.cpp
        src/CompiledGraph.cpp
        src/WorkerPool.cpp
        src/Engine.cpp
        src/nodes/ChannelStripNode.cpp
        src/nodes/SineNode.cpp
        include/elogico/engine/AudioBuffer.h
        include/elogico/engine/EventList.h
        include/elogico/engine/ProcessContext.h
        include/elogico/engine/Node.h
        include/elogico/engine/GraphDescription.h
        include/elogico/engine/GraphCompiler.h
        include/elogico/engine/CompiledGraph.h
        include/elogico/engine/DelayLine.h
        include/elogico/engine/WorkerPool.h
        include/elogico/engine/Transport.h
        include/elogico/engine/Engine.h
        include/elogico/engine/nodes/ChannelStripNode.h
        include/elogico/engine/nodes/SineNode.h
        include/elogico/engine/nodes/DelayNode.h
    DEPS elogico::core elogico::dsp)
