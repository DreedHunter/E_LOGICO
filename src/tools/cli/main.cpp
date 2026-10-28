// SPDX-License-Identifier: AGPL-3.0-or-later
// elogico-cli: prova motore, driver e import senza GUI.

#include "elogico/core/Version.h"
#include "elogico/engine/Engine.h"
#include "elogico/engine/nodes/ChannelStripNode.h"
#include "elogico/engine/nodes/SineNode.h"
#include "elogico/io/ImporterRegistry.h"
#include "elogico/platform/DeviceManager.h"
#include "elogico/platform/ThreadPriority.h"
#include "elogico/plugins/PluginRegistry.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#endif

using namespace elogico;

namespace
{

void printUsage()
{
    std::printf("%s %s — strumenti da riga di comando\n\n", kProductName, kVersionString);
    std::printf("  elogico-cli devices              elenca i device audio\n");
    std::printf("  elogico-cli tone [secondi]       suona un La 440 Hz sul device WASAPI di default\n");
    std::printf("  elogico-cli bench [tracce]       misura il motore offline (nessun suono)\n");
    std::printf("  elogico-cli import <file.logicx> mostra il report di import\n");
    std::printf("  elogico-cli plugins              elenca i file plugin trovati\n");
}

int listDevices()
{
    DeviceManager manager;
    for (const auto& backend : manager.backends())
    {
        std::printf("[%s]\n", backend->name().c_str());
        for (const auto& d : backend->listDevices())
            std::printf("  %s %s  (out %d)\n", d.isDefault ? "*" : " ", d.name.c_str(), d.numOutputs);
    }
    return 0;
}

GraphDescription toneGraph()
{
    GraphDescription g;
    auto master = std::make_shared<ChannelStripNode>("Stereo Out");
    master->setVolumeDb(-12.0f);
    g.addNode(1, master);
    g.addNode(2, std::make_shared<SineNode>(440.0f, 0.5f));
    g.connect(2, 1);
    g.output = 1;
    return g;
}

int playTone(int seconds)
{
    DeviceManager manager;
    auto* wasapi = manager.findBackend("WASAPI");
    if (wasapi == nullptr)
        return std::printf("WASAPI non disponibile\n"), 1;

    std::string error;
    auto device = wasapi->open({}, error);
    if (!device)
        return std::printf("errore: %s\n", error.c_str()), 1;

    Engine engine;
    engine.audioDeviceAboutToStart(device->sampleRate(), device->bufferSize(), 0, device->numOutputs());
    if (auto e = engine.setGraph(toneGraph()); !e.empty())
        return std::printf("errore grafo: %s\n", e.c_str()), 1;

    std::printf("%.0f Hz, buffer %d, latenza uscita %d campioni\n", device->sampleRate(), device->bufferSize(),
                device->outputLatency());
    device->start(engine);
    for (int i = 0; i < seconds * 10; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        engine.collectGarbage();
    }
    device->stop();
    std::printf("carico CPU ultimo blocco: %.2f%%, xrun: %llu\n", engine.feedback().cpuLoad.load() * 100.0f,
                static_cast<unsigned long long>(engine.feedback().xruns.load()));
    return 0;
}

int bench(int numTracks)
{
    constexpr double kRate = 48000.0;
    constexpr int kBlock = 128;
    Engine engine;
    engine.audioDeviceAboutToStart(kRate, kBlock, 0, 2);

    GraphDescription g;
    g.addNode(1, std::make_shared<ChannelStripNode>("Stereo Out"));
    g.output = 1;
    for (int t = 0; t < numTracks; ++t)
    {
        const NodeId src = NodeId(100 + t * 2), strip = NodeId(101 + t * 2);
        g.addNode(src, std::make_shared<SineNode>(110.0f + float(t), 0.01f));
        g.addNode(strip, std::make_shared<ChannelStripNode>("Track"));
        g.connect(src, strip);
        g.connect(strip, 1);
    }
    if (auto e = engine.setGraph(g); !e.empty())
        return std::printf("errore grafo: %s\n", e.c_str()), 1;

    std::vector<float> left(kBlock), right(kBlock);
    float* outs[] = {left.data(), right.data()};
    const int blocks = int(kRate * 10 / kBlock); // 10 secondi di audio
    const auto start = std::chrono::steady_clock::now();
    for (int b = 0; b < blocks; ++b)
        engine.processOffline(outs, 2, kBlock);
    const double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();

    std::printf("%d tracce, 10 s di audio in %.3f s (%.1fx tempo reale), xrun simulati: %llu\n", numTracks,
                elapsed, 10.0 / elapsed, static_cast<unsigned long long>(engine.feedback().xruns.load()));
    return 0;
}

int importProject(const char* path)
{
    ImporterRegistry registry;
    Importer* importer = registry.findFor(path);
    if (importer == nullptr)
        return std::printf("formato non riconosciuto: %s\n", path), 1;

    ImportReport report;
    const auto project = importer->import(path, report);
    static const char* labels[] = {"importato", "sostituito", "mancante", "ERRORE"};
    for (const auto& item : report.items)
        std::printf("[%s] %s%s%s\n", labels[int(item.severity)], item.what.c_str(),
                    item.detail.empty() ? "" : " — ", item.detail.c_str());
    if (project)
        std::printf("\nProgetto \"%s\": %zu tracce\n", project->name.c_str(), project->tracks.size());
    return project ? 0 : 1;
}

int listPlugins()
{
    PluginRegistry registry;
    if (registry.formats().empty())
        std::printf("nessun formato plugin in questa build (ELOGICO_WITH_CLAP / ELOGICO_WITH_VST3)\n");
    for (const auto& file : registry.findCandidateFiles())
        std::printf("%s\n", file.c_str());
    return 0;
}

} // namespace

int main(int argc, char** argv)
{
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
#endif
    disableProcessPowerThrottling();

    if (argc < 2)
        return printUsage(), 0;

    const std::string cmd = argv[1];
    if (cmd == "devices")
        return listDevices();
    if (cmd == "tone")
        return playTone(argc > 2 ? std::atoi(argv[2]) : 2);
    if (cmd == "bench")
        return bench(argc > 2 ? std::atoi(argv[2]) : 100);
    if (cmd == "import" && argc > 2)
        return importProject(argv[2]);
    if (cmd == "plugins")
        return listPlugins();

    printUsage();
    return 1;
}
