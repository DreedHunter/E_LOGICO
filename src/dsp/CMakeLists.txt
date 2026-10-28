# SPDX-License-Identifier: AGPL-3.0-or-later
# DSP nativi: effetti e strumenti stock. Nessuna dipendenza da JUCE.
elogico_add_module(dsp
    SOURCES
        src/Biquad.cpp
        include/elogico/dsp/Biquad.h
        include/elogico/dsp/SmoothedValue.h
        include/elogico/dsp/Decibels.h
    DEPS elogico::core)
