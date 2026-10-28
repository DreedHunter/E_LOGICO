// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/engine/CompiledGraph.h"

#include "elogico/engine/WorkerPool.h"

namespace elogico
{

int CompiledGraph::indexOf(NodeId id) const noexcept
{
    for (std::size_t i = 0; i < nodes_.size(); ++i)
        if (nodes_[i].id == id)
            return int(i);
    return -1;
}

void CompiledGraph::processNode(int index, const ProcessContext& context) noexcept
{
    auto& cn = nodes_[std::size_t(index)];
    const AudioBuffer io = cn.buffer.view(context.numFrames);
    io.clear();

    for (const auto& input : cn.inputs)
    {
        const AudioBuffer src = nodes_[std::size_t(input.sourceIndex)].buffer.view(context.numFrames);
        if (input.delayLineIndex >= 0)
            delayLines_[std::size_t(input.delayLineIndex)].processAdd(src, io);
        else
            io.addFrom(src);
    }

    cn.node->process(io, context);
}

void CompiledGraph::process(const AudioBuffer& out, const ProcessContext& context, WorkerPool* pool) noexcept
{
    if (outputIndex_ < 0 || context.numFrames > maxBlock_)
    {
        out.clear();
        return;
    }

    // Un livello alla volta; dentro il livello i nodi sono indipendenti.
    // TODO(V1): scheduler a dipendenze con priorità al percorso critico (spec, Ottimizzazione).
    for (const auto& level : levels_)
    {
        if (pool != nullptr && level.size() > 1)
        {
            pool->parallelFor(level.size(), [this, &level, &context](std::size_t i) noexcept {
                processNode(level[i], context);
            });
        }
        else
        {
            for (int index : level)
                processNode(index, context);
        }
    }

    out.copyFrom(nodes_[std::size_t(outputIndex_)].buffer.view(context.numFrames));
}

} // namespace elogico
