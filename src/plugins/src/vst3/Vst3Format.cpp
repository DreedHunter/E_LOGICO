// SPDX-License-Identifier: AGPL-3.0-or-later
#include "Vst3Format.h"

#include <cstdlib>
#include <filesystem>

namespace elogico
{

std::vector<std::string> Vst3Format::defaultSearchPaths() const
{
    std::vector<std::string> paths;
    if (const char* common = std::getenv("COMMONPROGRAMFILES"))
        paths.push_back(std::string(common) + "\\VST3");
    return paths;
}

bool Vst3Format::canHandle(const std::string& path) const
{
    return std::filesystem::path(path).extension() == ".vst3";
}

std::vector<PluginDescription> Vst3Format::scan(const std::string&, std::string& errorOut)
{
    errorOut = "scansione VST3 non ancora implementata";
    return {};
}

std::unique_ptr<Node> Vst3Format::instantiate(const PluginDescription&, std::string& errorOut)
{
    errorOut = "istanziazione VST3 non ancora implementata";
    return nullptr;
}

} // namespace elogico
