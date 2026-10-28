// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/plugins/PluginRegistry.h"

#if ELOGICO_HAS_CLAP
#include "clap/ClapFormat.h"
#endif
#if ELOGICO_HAS_VST3
#include "vst3/Vst3Format.h"
#endif

#include <filesystem>
#include <system_error>

namespace elogico
{

PluginRegistry::PluginRegistry()
{
#if ELOGICO_HAS_VST3
    formats_.push_back(std::make_unique<Vst3Format>());
#endif
#if ELOGICO_HAS_CLAP
    formats_.push_back(std::make_unique<ClapFormat>());
#endif
}

PluginFormat* PluginRegistry::formatFor(const std::string& path) const
{
    for (const auto& f : formats_)
        if (f->canHandle(path))
            return f.get();
    return nullptr;
}

std::vector<std::string> PluginRegistry::findCandidateFiles() const
{
    namespace fs = std::filesystem;
    std::vector<std::string> files;
    for (const auto& format : formats_)
    {
        for (const auto& dir : format->defaultSearchPaths())
        {
            std::error_code ec;
            if (!fs::is_directory(dir, ec))
                continue;
            for (auto it = fs::recursive_directory_iterator(dir, fs::directory_options::skip_permission_denied, ec);
                 it != fs::recursive_directory_iterator(); it.increment(ec))
            {
                if (ec)
                    break;
                const auto path = it->path().string();
                if (format->canHandle(path))
                {
                    files.push_back(path);
                    if (it->is_directory(ec))
                        it.disable_recursion_pending(); // i bundle .vst3 sono cartelle
                }
            }
        }
    }
    return files;
}

} // namespace elogico
