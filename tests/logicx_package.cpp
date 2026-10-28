// SPDX-License-Identifier: AGPL-3.0-or-later
// Costruisce su disco un finto pacchetto .logicx con la struttura di cartelle reale
// (nessun contenuto Apple) e verifica che l'importatore lo riconosca.
#include "TestHarness.h"

#include "elogico/io/ImporterRegistry.h"
#include "elogico/io/logicx/LogicxPackage.h"

#include <filesystem>
#include <fstream>

using namespace elogico;
namespace fs = std::filesystem;

namespace
{
fs::path makeFakePackage()
{
    const fs::path root = fs::temp_directory_path() / "elogico_test" / "Prova.logicx";
    fs::remove_all(root.parent_path());
    fs::create_directories(root / "Alternatives" / "000");
    fs::create_directories(root / "Media" / "Audio Files");
    fs::create_directories(root / "Resources");
    std::ofstream(root / "Alternatives" / "000" / "ProjectData", std::ios::binary) << "dati di prova";
    std::ofstream(root / "Media" / "Audio Files" / "Voce#01.wav", std::ios::binary) << "RIFF";
    std::ofstream(root / "Media" / "Audio Files" / "Basso.aif", std::ios::binary) << "FORM";
    std::ofstream(root / "Media" / "Audio Files" / "note.txt") << "non audio";
    return root;
}
} // namespace

TEST_CASE("struttura del pacchetto")
{
    const auto root = makeFakePackage();
    std::string error;
    const auto package = logicx::Package::open(root, error);
    CHECK(package.has_value());
    if (!package)
        return;
    CHECK(package->alternatives.size() == 1);
    CHECK(package->alternatives[0].projectDataSize == 13);
    CHECK(package->audioFiles.size() == 2);
}

TEST_CASE("import: una traccia per file audio e report con le parti mancanti")
{
    const auto root = makeFakePackage();
    ImporterRegistry registry;
    const std::string path = root.string();
    Importer* importer = registry.findFor(path);
    CHECK(importer != nullptr);
    if (importer == nullptr)
        return;

    ImportReport report;
    const auto project = importer->import(path, report);
    CHECK(project.has_value());
    CHECK(!report.hasErrors());
    if (project)
    {
        CHECK(project->name == "Prova");
        CHECK(project->tracks.size() == 2);
        CHECK(project->tracks[0].regions.size() == 1);
    }
}

TEST_CASE("un file singolo non è un pacchetto valido")
{
    const fs::path file = fs::temp_directory_path() / "elogico_test_file.logicx";
    std::ofstream(file) << "x";
    ImportReport report;
    ImporterRegistry registry;
    const auto project = registry.findFor(file.string())->import(file.string(), report);
    CHECK(!project.has_value());
    CHECK(report.hasErrors());
    fs::remove(file);
}

TEST_MAIN()
