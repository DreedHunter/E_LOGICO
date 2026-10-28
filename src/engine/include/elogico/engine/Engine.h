// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/core/AudioCallback.h"
#include "elogico/core/SpscQueue.h"
#include "elogico/core/TempoMap.h"
#include "elogico/engine/CompiledGraph.h"
#include "elogico/engine/GraphDescription.h"
#include "elogico/engine/Transport.h"
#include "elogico/engine/WorkerPool.h"

#include <atomic>
#include <memory>
#include <string>

namespace elogico
{

/// Comandi dal modello/GUI al thread audio. Piccoli e copiabili, passano in una coda lock-free.
struct EngineCommand
{
    enum class Type : std::uint8_t
    {
        Play,
        Stop,
        Locate,
        SetLoop,
        SetLooping
    };

    Type type = Type::Stop;
    SamplePos a = 0;
    SamplePos b = 0;
};

/// Valori letti dalla GUI con un timer: posizione, meter, carico CPU. Solo atomiche.
struct EngineFeedback
{
    std::atomic<SamplePos> position{0};
    std::atomic<bool> playing{false};
    std::atomic<float> cpuLoad{0.0f};      // frazione del tempo di buffer usata
    std::atomic<float> outputPeak[2]{};
    std::atomic<std::uint64_t> xruns{0};   // blocchi più lunghi del previsto
};

/// Il motore audio. Implementa AudioCallback per i driver; tutte le altre funzioni
/// pubbliche si chiamano da thread non real-time.
class Engine final : public AudioCallback
{
public:
    explicit Engine(int numWorkers = WorkerPool::defaultWorkerCount());
    ~Engine() override;

    /// Compila e pubblica un nuovo grafo. Il thread audio lo adotta al prossimo blocco.
    /// Ritorna una stringa vuota se riuscito, altrimenti il motivo.
    std::string setGraph(const GraphDescription& description);

    /// Libera i grafi che il thread audio ha restituito. Da chiamare periodicamente (es. timer GUI).
    void collectGarbage();

    bool post(const EngineCommand& command) noexcept { return commands_.push(command); }
    void play() noexcept { post({EngineCommand::Type::Play}); }
    void stop() noexcept { post({EngineCommand::Type::Stop}); }
    void locate(SamplePos position) noexcept { post({EngineCommand::Type::Locate, position}); }

    const EngineFeedback& feedback() const noexcept { return feedback_; }
    TempoMap& tempoMap() noexcept { return tempoMap_; } // TODO: passarla al RT con swap atomico
    double sampleRate() const noexcept { return sampleRate_; }
    int maxBlockSize() const noexcept { return maxBlock_; }

    /// Esegue un blocco senza driver (bounce offline, test). Stesso percorso del callback.
    void processOffline(float* const* outputs, int numOutputs, int numFrames) noexcept;

    // AudioCallback
    void audioDeviceAboutToStart(double sampleRate, int maxBlockSize, int numInputs, int numOutputs) override;
    void audioCallback(const float* const* inputs, int numInputs, float* const* outputs, int numOutputs,
                       int numFrames) noexcept override;
    void audioDeviceStopped() override;

private:
    void handleCommands() noexcept;
    void adoptPendingGraph() noexcept;

    WorkerPool pool_;
    TempoMap tempoMap_;
    Transport transport_; // solo thread audio
    EngineFeedback feedback_;

    GraphDescription lastDescription_; // per ricompilare al cambio di sample rate
    CompiledGraph* current_ = nullptr; // solo thread audio
    CompiledGraph* deferredGarbage_ = nullptr; // solo thread audio
    SpscQueue<CompiledGraph*, 8> incoming_;
    SpscQueue<CompiledGraph*, 8> garbage_;
    SpscQueue<EngineCommand, 256> commands_;

    double sampleRate_ = 48000.0;
    int maxBlock_ = 1024;
};

} // namespace elogico
