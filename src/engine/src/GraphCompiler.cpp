// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/engine/GraphCompiler.h"

#include <algorithm>
#include <unordered_map>

namespace elogico
{

GraphCompiler::Result GraphCompiler::compile(const GraphDescription& description, double sampleRate, int maxBlock)
{
    Result result;
    const std::size_t n = description.nodes.size();

    std::unordered_map<NodeId, std::size_t> indexById;
    for (std::size_t i = 0; i < n; ++i)
    {
        const auto& entry = description.nodes[i];
        if (!entry.node)
            return {nullptr, "nodo senza implementazione: " + std::to_string(entry.id)};
        if (!indexById.emplace(entry.id, i).second)
            return {nullptr, "id di nodo duplicato: " + std::to_string(entry.id)};
    }

    std::vector<std::vector<std::size_t>> sources(n), destinations(n);
    for (const auto& c : description.connections)
    {
        const auto s = indexById.find(c.source);
        const auto d = indexById.find(c.destination);
        if (s == indexById.end() || d == indexById.end())
            return {nullptr, "connessione verso un nodo inesistente"};
        sources[d->second].push_back(s->second);
        destinations[s->second].push_back(d->second);
    }

    // Ordinamento topologico (Kahn), che rileva anche i cicli.
    std::vector<int> pending(n);
    std::vector<std::size_t> order, ready;
    for (std::size_t i = 0; i < n; ++i)
        if ((pending[i] = int(sources[i].size())) == 0)
            ready.push_back(i);
    while (!ready.empty())
    {
        const auto i = ready.back();
        ready.pop_back();
        order.push_back(i);
        for (auto d : destinations[i])
            if (--pending[d] == 0)
                ready.push_back(d);
    }
    if (order.size() != n)
        return {nullptr, "il grafo contiene un ciclo"};

    auto graph = std::make_unique<CompiledGraph>();
    graph->maxBlock_ = maxBlock;
    graph->nodes_.resize(n);

    std::vector<int> compiledIndex(n);
    for (std::size_t k = 0; k < n; ++k)
        compiledIndex[order[k]] = int(k);

    for (std::size_t k = 0; k < n; ++k)
    {
        const auto original = order[k];
        auto& cn = graph->nodes_[k];
        cn.id = description.nodes[original].id;
        cn.node = description.nodes[original].node;
        // TODO(MVP): un nodo condiviso col grafo in esecuzione non va ripreparato mentre il RT lo usa;
        // servirà un flag "già pronto con questi parametri" oppure una copia del nodo.
        cn.node->prepare(sampleRate, maxBlock);
        cn.buffer = AudioBlock(cn.node->numChannels(), maxBlock);

        // Livello e latenza in arrivo: i sorgenti sono già stati visitati (ordine topologico).
        int level = 0, arrival = 0;
        for (auto s : sources[original])
        {
            const auto& src = graph->nodes_[std::size_t(compiledIndex[s])];
            level = std::max(level, src.level + 1);
            arrival = std::max(arrival, src.arrivalLatency + src.node->latencySamples());
        }
        cn.level = level;
        cn.arrivalLatency = arrival;

        for (auto s : sources[original])
        {
            const int si = compiledIndex[s];
            const auto& src = graph->nodes_[std::size_t(si)];
            CompiledGraph::Input input;
            input.sourceIndex = si;
            input.compensation = arrival - (src.arrivalLatency + src.node->latencySamples());
            if (input.compensation > 0)
            {
                input.delayLineIndex = int(graph->delayLines_.size());
                graph->delayLines_.emplace_back().prepare(src.node->numChannels(), input.compensation, maxBlock);
            }
            cn.inputs.push_back(input);
        }
    }

    int maxLevel = 0;
    for (const auto& cn : graph->nodes_)
        maxLevel = std::max(maxLevel, cn.level);
    graph->levels_.resize(n == 0 ? 0 : std::size_t(maxLevel) + 1);
    for (std::size_t k = 0; k < n; ++k)
        graph->levels_[std::size_t(graph->nodes_[k].level)].push_back(int(k));

    if (n > 0)
    {
        const auto out = indexById.find(description.output);
        if (out == indexById.end())
            return {nullptr, "nodo di output non trovato"};
        graph->outputIndex_ = compiledIndex[out->second];
        const auto& outNode = graph->nodes_[std::size_t(graph->outputIndex_)];
        graph->totalLatency_ = outNode.arrivalLatency + outNode.node->latencySamples();
    }

    result.graph = std::move(graph);
    return result;
}

} // namespace elogico
