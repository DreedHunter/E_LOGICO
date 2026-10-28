// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/engine/AudioBuffer.h"
#include "elogico/engine/DelayLine.h"
#include "elogico/engine/GraphDescription.h"

#include <memory>
#include <vector>

namespace elogico
{

class WorkerPool;

/// Grafo pronto per il thread audio: ordine topologico, livelli paralleli,
/// buffer preallocati e linee di ritardo della PDC. Immutabile dopo la compilazione.
class CompiledGraph
{
public:
    struct Input
    {
        int sourceIndex = 0;     // indice in nodes()
        int compensation = 0;    // campioni di ritardo aggiunti dalla PDC
        int delayLineIndex = -1; // -1 = nessun ritardo
    };

    struct CompiledNode
    {
        NodeId id = 0;
        std::shared_ptr<Node> node;
        std::vector<Input> inputs;
        int level = 0;           // nodi dello stesso livello girano in parallelo
        int arrivalLatency = 0;  // latenza accumulata in ingresso al nodo
        AudioBlock buffer;
    };

    /// RT: esegue il grafo e scrive l'uscita del nodo di output in out.
    void process(const AudioBuffer& out, const ProcessContext& context, WorkerPool* pool) noexcept;

    const std::vector<CompiledNode>& nodes() const noexcept { return nodes_; }
    int totalLatency() const noexcept { return totalLatency_; }
    int maxBlock() const noexcept { return maxBlock_; }
    int indexOf(NodeId id) const noexcept;

private:
    friend class GraphCompiler;

    void processNode(int index, const ProcessContext& context) noexcept;

    std::vector<CompiledNode> nodes_;            // in ordine topologico
    std::vector<std::vector<int>> levels_;       // indici di nodes_ per livello
    std::vector<DelayLine> delayLines_;
    int outputIndex_ = -1;
    int totalLatency_ = 0;
    int maxBlock_ = 0;
};

} // namespace elogico
