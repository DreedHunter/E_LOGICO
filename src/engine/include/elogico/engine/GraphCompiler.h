// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/engine/CompiledGraph.h"
#include "elogico/engine/GraphDescription.h"

#include <memory>
#include <string>

namespace elogico
{

/// Trasforma una GraphDescription in un CompiledGraph. Gira solo fuori dal thread audio.
///
/// PDC: per ogni nodo calcola la latenza accumulata lungo il percorso più lungo,
/// poi inserisce un ritardo su ogni ramo più corto prima del punto di somma.
class GraphCompiler
{
public:
    struct Result
    {
        std::unique_ptr<CompiledGraph> graph;
        std::string error; // vuota se la compilazione è riuscita
    };

    static Result compile(const GraphDescription& description, double sampleRate, int maxBlock);
};

} // namespace elogico
