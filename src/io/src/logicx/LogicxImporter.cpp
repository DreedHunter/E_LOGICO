// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/io/logicx/LogicxImporter.h"

#include "elogico/io/logicx/LogicxPackage.h"

#include <filesystem>

namespace elogico
{

namespace fs = std::filesystem;

bool LogicxImporter::canImport(const std::string& path) const
{
    return fs::path(path).extension() == ".logicx";
}

std::optional<Project> LogicxImporter::import(const std::string& path, ImportReport& report)
{
    std::string error;
    const auto package = logicx::Package::open(fs::path(std::u8string(path.begin(), path.end())), error);
    if (!package)
    {
        report.error("Pacchetto .logicx non leggibile", error);
        return std::nullopt;
    }

    Project project;
    project.name = fs::path(path).stem().string();

    const auto& alt = package->alternatives.front();
    report.imported("Alternativa di progetto " + alt.name,
                    "ProjectData: " + std::to_string(alt.projectDataSize) + " byte");
    if (package->alternatives.size() > 1)
        report.missing("Altre alternative",
                       std::to_string(package->alternatives.size() - 1) + " ignorate, si apre solo la prima");

    // Livello 1 parziale: una traccia per file audio, regione a inizio progetto.
    for (const auto& file : package->audioFiles)
    {
        Track track;
        track.id = project.allocateId();
        track.type = TrackType::Audio;
        track.name = file.stem().string();

        Region region;
        region.id = project.allocateId();
        region.kind = Region::Kind::Audio;
        region.name = track.name;
        region.audioFile = fs::relative(file, package->root).generic_string();
        track.regions.push_back(std::move(region));

        project.tracks.push_back(std::move(track));
    }
    if (!package->audioFiles.empty())
        report.replaced("Tracce audio",
                        std::to_string(package->audioFiles.size())
                            + " file audio importati come tracce, tutti a inizio progetto");

    // TODO(M2): decodificare ProjectData (tracce, regioni, tempo, metrica, marker, mixer).
    report.missing("Posizione delle regioni, MIDI, tempo e mixer",
                   "la decodifica di ProjectData non è ancora implementata");
    return project;
}

} // namespace elogico
