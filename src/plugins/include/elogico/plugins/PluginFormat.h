// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/engine/Node.h"
#include "elogico/plugins/PluginDescription.h"

#include <memory>
#include <string>
#include <vector>

namespace elogico
{

/// Un formato di plugin (VST3, CLAP). Un'istanza di plugin è un Node del grafo audio.
class PluginFormat
{
public:
    virtual ~PluginFormat() = default;

    virtual std::string name() const = 0;
    /// Cartelle standard di Windows dove cercare i plugin del formato.
    virtual std::vector<std::string> defaultSearchPaths() const = 0;
    virtual bool canHandle(const std::string& path) const = 0;

    /// Elenca i plugin contenuti in un file. Va chiamato solo nel processo di scansione.
    virtual std::vector<PluginDescription> scan(const std::string& path, std::string& errorOut) = 0;

    /// TODO(MVP): istanza come Node con stato, parametri, latenza e editor.
    virtual std::unique_ptr<Node> instantiate(const PluginDescription& description, std::string& errorOut) = 0;
};

} // namespace elogico
