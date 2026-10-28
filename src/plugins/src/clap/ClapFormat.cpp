// SPDX-License-Identifier: AGPL-3.0-or-later
#include "ClapFormat.h"

#include <clap/clap.h>

#include <cstdlib>
#include <cstring>
#include <filesystem>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace elogico
{

namespace
{

/// Carica la DLL di un plugin e ne libera le risorse alla distruzione.
class ClapLibrary
{
public:
    explicit ClapLibrary(const std::string& path)
    {
#if defined(_WIN32)
        module_ = LoadLibraryExW(std::filesystem::path(path).wstring().c_str(), nullptr,
                                 LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        if (module_ != nullptr)
            entry_ = reinterpret_cast<const clap_plugin_entry_t*>(GetProcAddress(module_, "clap_entry"));
#endif
        if (entry_ != nullptr && entry_->init(path.c_str()))
            initialised_ = true;
    }

    ~ClapLibrary()
    {
        if (initialised_)
            entry_->deinit();
#if defined(_WIN32)
        if (module_ != nullptr)
            FreeLibrary(module_);
#endif
    }

    ClapLibrary(const ClapLibrary&) = delete;
    ClapLibrary& operator=(const ClapLibrary&) = delete;

    bool ok() const noexcept { return initialised_; }
    const clap_plugin_entry_t* entry() const noexcept { return entry_; }

private:
#if defined(_WIN32)
    HMODULE module_ = nullptr;
#endif
    const clap_plugin_entry_t* entry_ = nullptr;
    bool initialised_ = false;
};

bool hasFeature(const clap_plugin_descriptor_t* d, const char* feature)
{
    for (auto f = d->features; f != nullptr && *f != nullptr; ++f)
        if (std::strcmp(*f, feature) == 0)
            return true;
    return false;
}

} // namespace

std::vector<std::string> ClapFormat::defaultSearchPaths() const
{
    std::vector<std::string> paths;
    if (const char* common = std::getenv("COMMONPROGRAMFILES"))
        paths.push_back(std::string(common) + "\\CLAP");
    if (const char* local = std::getenv("LOCALAPPDATA"))
        paths.push_back(std::string(local) + "\\Programs\\Common\\CLAP");
    return paths;
}

bool ClapFormat::canHandle(const std::string& path) const
{
    return std::filesystem::path(path).extension() == ".clap";
}

std::vector<PluginDescription> ClapFormat::scan(const std::string& path, std::string& errorOut)
{
    std::vector<PluginDescription> result;
    ClapLibrary library(path);
    if (!library.ok())
    {
        errorOut = "impossibile caricare il plugin CLAP o clap_entry mancante";
        return result;
    }
    if (!clap_version_is_compatible(library.entry()->clap_version))
    {
        errorOut = "versione CLAP incompatibile";
        return result;
    }

    const auto* factory =
        static_cast<const clap_plugin_factory_t*>(library.entry()->get_factory(CLAP_PLUGIN_FACTORY_ID));
    if (factory == nullptr)
    {
        errorOut = "il plugin non espone una plugin factory";
        return result;
    }

    const uint32_t count = factory->get_plugin_count(factory);
    for (uint32_t i = 0; i < count; ++i)
    {
        const auto* d = factory->get_plugin_descriptor(factory, i);
        if (d == nullptr)
            continue;
        PluginDescription desc;
        desc.format = "CLAP";
        desc.id = d->id ? d->id : "";
        desc.name = d->name ? d->name : "";
        desc.vendor = d->vendor ? d->vendor : "";
        desc.version = d->version ? d->version : "";
        desc.path = path;
        desc.isInstrument = hasFeature(d, CLAP_PLUGIN_FEATURE_INSTRUMENT);
        result.push_back(std::move(desc));
    }
    return result;
}

std::unique_ptr<Node> ClapFormat::instantiate(const PluginDescription&, std::string& errorOut)
{
    errorOut = "istanziazione CLAP non ancora implementata";
    return nullptr;
}

} // namespace elogico
