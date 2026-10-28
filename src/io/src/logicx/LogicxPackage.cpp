// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/io/logicx/LogicxPackage.h"

#include <algorithm>
#include <cctype>
#include <system_error>

namespace elogico::logicx
{

namespace fs = std::filesystem;

namespace
{
bool isAudioFile(const fs::path& p)
{
    auto ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return char(std::tolower(c)); });
    return ext == ".wav" || ext == ".aif" || ext == ".aiff" || ext == ".caf" || ext == ".m4a" || ext == ".mp3";
}
} // namespace

std::optional<Package> Package::open(const fs::path& path, std::string& errorOut)
{
    std::error_code ec;
    if (!fs::is_directory(path, ec))
    {
        errorOut = "un .logicx deve essere una cartella (pacchetto macOS); se arriva come file singolo va "
                   "copiato da un Mac con \"Mostra contenuto pacchetto\" o compresso in zip";
        return std::nullopt;
    }

    Package package;
    package.root = path;

    const fs::path alternatives = path / "Alternatives";
    if (fs::is_directory(alternatives, ec))
    {
        for (const auto& entry : fs::directory_iterator(alternatives, ec))
        {
            const fs::path data = entry.path() / "ProjectData";
            if (entry.is_directory(ec) && fs::is_regular_file(data, ec))
                package.alternatives.push_back({entry.path().filename().string(), data, fs::file_size(data, ec)});
        }
        std::sort(package.alternatives.begin(), package.alternatives.end(),
                  [](const Alternative& a, const Alternative& b) { return a.name < b.name; });
    }
    if (package.alternatives.empty())
    {
        errorOut = "nessun Alternatives/*/ProjectData nel pacchetto";
        return std::nullopt;
    }

    for (const auto* dir : {"Media/Audio Files", "Audio Files"})
    {
        const fs::path audio = path / dir;
        if (!fs::is_directory(audio, ec))
            continue;
        for (const auto& entry : fs::recursive_directory_iterator(audio, ec))
            if (entry.is_regular_file(ec) && isAudioFile(entry.path()))
                package.audioFiles.push_back(entry.path());
    }
    std::sort(package.audioFiles.begin(), package.audioFiles.end());

    const fs::path info = path / "Resources" / "ProjectInformation.plist";
    if (fs::is_regular_file(info, ec))
        package.projectInformation = info;

    return package;
}

} // namespace elogico::logicx
