// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/plugins/PluginFormat.h"

namespace elogico
{

/// Hosting VST3 con l'SDK Steinberg (attivo con ELOGICO_WITH_VST3=ON).
/// TODO(MVP): scansione delle classi, IComponent/IAudioProcessor, IParameterChanges a campione, stato.
class Vst3Format final : public PluginFormat
{
public:
    std::string name() const override { return "VST3"; }
    std::vector<std::string> defaultSearchPaths() const override;
    bool canHandle(const std::string& path) const override;
    std::vector<PluginDescription> scan(const std::string& path, std::string& errorOut) override;
    std::unique_ptr<Node> instantiate(const PluginDescription& description, std::string& errorOut) override;
};

} // namespace elogico
