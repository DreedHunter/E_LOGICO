// SPDX-License-Identifier: AGPL-3.0-or-later
#include "AppController.h"

#include "elogico/io/ImporterRegistry.h"
#include "elogico/platform/ThreadPriority.h"

namespace elogico
{

AppController::AppController() : document_(demoProject())
{
    disableProcessPowerThrottling();
    document_.addListener([this](const ProjectDocument::Change& change) {
        if (change.graphChanged)
            rebuildGraph();
        else
            graphBuilder_.syncParameters(document_.project());
    });
    rebuildGraph();
}

AppController::~AppController()
{
    stopAudio();
}

void AppController::rebuildGraph()
{
    engine_.tempoMap().setTempo(0, document_.project().bpm);
    engine_.setGraph(graphBuilder_.build(document_.project()));
}

std::string AppController::startAudio()
{
    auto* wasapi = devices_.findBackend("WASAPI");
    if (wasapi == nullptr)
        return "nessun backend audio disponibile";
    std::string error;
    device_ = wasapi->open({}, error);
    if (!device_)
        return error;
    device_->start(engine_);
    return {};
}

void AppController::stopAudio()
{
    if (device_)
        device_->stop();
    device_.reset();
}

std::string AppController::importProject(const std::string& path)
{
    ImporterRegistry registry;
    Importer* importer = registry.findFor(path);
    if (importer == nullptr)
        return "formato non supportato";
    ImportReport report;
    auto project = importer->import(path, report);
    if (!project)
        return report.items.empty() ? "import fallito" : report.items.back().detail;
    document_.replace(std::move(*project));
    // TODO(MVP): mostrare il report all'utente in una finestra dedicata.
    return {};
}

Project AppController::demoProject()
{
    // Lo stesso progetto del mockup, per vedere subito la finestra popolata.
    Project p;
    p.name = "Notte a Torino";
    p.bpm = 96.0;
    p.key = "Lam";
    struct Demo
    {
        const char* name;
        TrackType type;
        std::uint32_t colour;
        int startBar, bars;
    };
    const Demo demo[] = {
        {"Batterista", TrackType::SoftwareInstrument, 0xffe7b84a, 1, 16},
        {"Basso", TrackType::SoftwareInstrument, 0xff5fc46b, 5, 12},
        {"Piano elettrico", TrackType::SoftwareInstrument, 0xff4aa3e8, 1, 16},
        {"Archi", TrackType::SoftwareInstrument, 0xff8d7be8, 9, 8},
        {"Voce", TrackType::Audio, 0xffe8735a, 9, 8},
        {"Cori", TrackType::Audio, 0xffe86fa8, 25, 8},
        {"Lead synth", TrackType::SoftwareInstrument, 0xff42cfc4, 17, 8},
        {"Chitarra", TrackType::Audio, 0xffc9a26a, 1, 8},
    };
    const Ticks bar = 4 * kTicksPerQuarter;
    for (const auto& d : demo)
    {
        Track t;
        t.id = p.allocateId();
        t.name = d.name;
        t.type = d.type;
        t.colour = d.colour;
        Region r;
        r.id = p.allocateId();
        r.kind = d.type == TrackType::Audio ? Region::Kind::Audio : Region::Kind::Midi;
        r.name = d.name;
        r.start = Ticks(d.startBar - 1) * bar;
        r.length = Ticks(d.bars) * bar;
        t.regions.push_back(r);
        p.tracks.push_back(std::move(t));
    }
    return p;
}

} // namespace elogico
