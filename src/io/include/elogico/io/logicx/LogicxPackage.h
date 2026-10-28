// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace elogico::logicx
{

/// Struttura su disco di un pacchetto .logicx (una cartella-pacchetto macOS).
/// Lettura solo dei file: il contenuto binario di ProjectData si decodifica in LogicxImporter.
struct Package
{
    struct Alternative
    {
        std::string name;                  // "000", "001", ...
        std::filesystem::path projectData; // Alternatives/<n>/ProjectData
        std::uintmax_t projectDataSize = 0;
    };

    std::filesystem::path root;
    std::vector<Alternative> alternatives;
    std::vector<std::filesystem::path> audioFiles; // Media/Audio Files o Audio Files
    std::filesystem::path projectInformation;      // Resources/ProjectInformation.plist, se presente

    /// Ritorna nullopt se la cartella non ha la forma di un pacchetto .logicx.
    static std::optional<Package> open(const std::filesystem::path& path, std::string& errorOut);
};

} // namespace elogico::logicx
