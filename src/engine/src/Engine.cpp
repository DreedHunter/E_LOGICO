// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/engine/Engine.h"

#include "elogico/core/ScopedNoDenormals.h"
#include "elogico/engine/GraphCompiler.h"

#include <algorithm>
#include <chrono>

namespace elogico
{

Engine::Engine(int numWorkers) : pool_(numWorkers) {}

Engine::~Engine()
{
    collectGarbage();
    while (auto pending = incoming_.pop())
        delete *pending;
    delete deferredGarbage_;
    delete current_;
}

std::string Engine::setGraph(const GraphDescription& description)
{
    auto result = GraphCompiler::compile(description, sampleRate_, maxBlock_);
    if (!result.graph)
        return result.error;
    lastDescription_ = description;

    collectGarbage();
    if (!incoming_.push(result.graph.get()))
        return "troppi grafi in attesa: il thread audio non sta girando?";
    result.graph.release();
    return {};
}

void Engine::collectGarbage()
{
    while (auto old = garbage_.pop())
        delete *old;
}

void Engine::adoptPendingGraph() noexcept
{
    // Un grafo che il GC non ha potuto accettare resta qui fino al prossimo blocco: mai liberare sul RT.
    if (deferredGarbage_ != nullptr && garbage_.push(deferredGarbage_))
        deferredGarbage_ = nullptr;
    if (deferredGarbage_ != nullptr)
        return;

    // Prende solo l'ultimo grafo pubblicato; gli intermedi tornano subito al GC.
    while (auto next = incoming_.pop())
    {
        if (current_ != nullptr && !garbage_.push(current_))
        {
            deferredGarbage_ = current_;
            current_ = *next;
            return;
        }
        current_ = *next;
    }
    // TODO(MVP): crossfade di 10 ms sui nodi cambiati (spec, Modifiche a caldo).
}

void Engine::handleCommands() noexcept
{
    while (auto cmd = commands_.pop())
    {
        switch (cmd->type)
        {
        case EngineCommand::Type::Play: transport_.playing = true; break;
        case EngineCommand::Type::Stop: transport_.playing = false; break;
        case EngineCommand::Type::Locate: transport_.position = std::max<SamplePos>(0, cmd->a); break;
        case EngineCommand::Type::SetLoop:
            transport_.loopStart = cmd->a;
            transport_.loopEnd = cmd->b;
            break;
        case EngineCommand::Type::SetLooping: transport_.looping = cmd->a != 0; break;
        }
    }
}

void Engine::audioDeviceAboutToStart(double sampleRate, int maxBlockSize, int, int)
{
    const bool changed = sampleRate != sampleRate_ || maxBlockSize > maxBlock_;
    sampleRate_ = sampleRate;
    maxBlock_ = std::max(maxBlock_, maxBlockSize);
    tempoMap_.setSampleRate(sampleRate);
    if (changed && !lastDescription_.nodes.empty())
        setGraph(lastDescription_);
}

void Engine::audioDeviceStopped()
{
    feedback_.playing.store(false, std::memory_order_relaxed);
}

void Engine::audioCallback(const float* const*, int, float* const* outputs, int numOutputs,
                           int numFrames) noexcept
{
    // TODO(MVP): ingressi verso le tracce armate e allineamento della latenza di registrazione.
    processOffline(outputs, numOutputs, numFrames);
}

void Engine::processOffline(float* const* outputs, int numOutputs, int numFrames) noexcept
{
    ScopedNoDenormals noDenormals;
    const auto started = std::chrono::steady_clock::now();

    handleCommands();
    adoptPendingGraph();

    ProcessContext context;
    context.samplePos = transport_.advance(numFrames);
    context.ppqPos = tempoMap_.samplesToPpq(context.samplePos);
    context.bpm = tempoMap_.tempoAt(tempoMap_.samplesToTicks(context.samplePos));
    context.sampleRate = sampleRate_;
    context.numFrames = numFrames;
    context.playing = transport_.playing;

    const AudioBuffer out(outputs, numOutputs, numFrames);
    if (current_ != nullptr)
        current_->process(out, context, &pool_);
    else
        out.clear();

    feedback_.position.store(transport_.position, std::memory_order_relaxed);
    feedback_.playing.store(transport_.playing, std::memory_order_relaxed);
    for (int c = 0; c < std::min(numOutputs, 2); ++c)
        feedback_.outputPeak[c].store(out.peak(c), std::memory_order_relaxed);

    const double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - started).count();
    const double budget = double(numFrames) / sampleRate_;
    const float load = float(elapsed / budget);
    feedback_.cpuLoad.store(load, std::memory_order_relaxed);
    if (load > 1.0f)
        feedback_.xruns.fetch_add(1, std::memory_order_relaxed);
}

} // namespace elogico
