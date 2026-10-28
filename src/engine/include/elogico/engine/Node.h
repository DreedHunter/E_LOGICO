// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/engine/AudioBuffer.h"
#include "elogico/engine/ProcessContext.h"

namespace elogico
{

/// Interfaccia minima che ogni nodo del grafo implementa: canali, bus, plugin, strumenti.
class Node
{
public:
    virtual ~Node() = default;

    /// Non RT: alloca qui tutto quello che serve a process().
    virtual void prepare(double sampleRate, int maxBlock) = 0;

    /// RT: elabora io in place. In ingresso c'è la somma (già compensata) dei nodi a monte.
    virtual void process(const AudioBuffer& io, const ProcessContext& context) noexcept = 0;

    /// Latenza introdotta dal nodo, usata dal compilatore per la PDC.
    virtual int latencySamples() const noexcept { return 0; }

    /// Numero di canali del buffer del nodo (2 = stereo).
    virtual int numChannels() const noexcept { return 2; }

    /// Nome per debug, profiler e meter CPU.
    virtual const char* name() const noexcept { return "Node"; }
};

} // namespace elogico
