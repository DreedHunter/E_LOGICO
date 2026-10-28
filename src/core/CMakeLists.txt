# SPDX-License-Identifier: AGPL-3.0-or-later
elogico_add_module(core
    SOURCES
        src/TempoMap.cpp
        include/elogico/core/AudioCallback.h
        include/elogico/core/Types.h
        include/elogico/core/SpscQueue.h
        include/elogico/core/ScopedNoDenormals.h
        include/elogico/core/TempoMap.h
        include/elogico/core/Version.h)
