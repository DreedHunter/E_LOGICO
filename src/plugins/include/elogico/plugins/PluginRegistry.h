// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/plugins/PluginFormat.h"

#include <memory>
#include <vector>

namespace elogico
{

/// Formati disponibili in questa build e file trovati nelle cartelle di ricerca.
/// TODO(MVP): cache su disco dei risultati e blacklist dei plugin che crashano in scansione.
class PluginRegistry
{
public:
    PluginRegistry();

    const std::vector<std::unique_ptr<PluginFormat>>& formats() const noexcept { return formats_; }
    PluginFormat* formatFor(const std::string& path) const;

    /// Elenca i file candidati (senza caricarli) in tutte le cartelle standard.
    std::vector<std::string> findCandidateFiles() const;

private:
    std::vector<std::unique_ptr<PluginFormat>> formats_;
};

} // namespace elogico
