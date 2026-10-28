// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/plugins/PluginFormat.h"

namespace elogico
{

/// Hosting CLAP con gli header ufficiali. Per ora: scansione dei descrittori.
class ClapFormat final : public PluginFormat
{
public:
    std::string name() const override { return "CLAP"; }
    std::vector<std::string> defaultSearchPaths() const override;
    bool canHandle(const std::string& path) const override;
    std::vector<PluginDescription> scan(const std::string& path, std::string& errorOut) override;
    std::unique_ptr<Node> instantiate(const PluginDescription& description, std::string& errorOut) override;
};

} // namespace elogico
