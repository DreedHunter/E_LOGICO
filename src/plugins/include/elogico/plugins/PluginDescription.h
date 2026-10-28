// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <string>

namespace elogico
{

/// Risultato della scansione di un plugin, salvato in cache tra un avvio e l'altro.
struct PluginDescription
{
    std::string format;     // "VST3" o "CLAP"
    std::string id;         // id univoco del formato (CLAP id, VST3 class id)
    std::string name;
    std::string vendor;
    std::string version;
    std::string path;       // file .vst3 / .clap
    bool isInstrument = false;
    int numInputs = 2;
    int numOutputs = 2;
};

} // namespace elogico
