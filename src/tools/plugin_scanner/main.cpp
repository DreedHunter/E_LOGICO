// SPDX-License-Identifier: AGPL-3.0-or-later
// elogico-plugin-scanner <file>: carica un plugin e stampa i descrittori, una riga per plugin.
// La DAW lo lancia con un timeout; se il processo crasha, il file finisce in blacklist.

#include "elogico/plugins/PluginRegistry.h"

#include <cstdio>

using namespace elogico;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::fprintf(stderr, "uso: elogico-plugin-scanner <file .clap/.vst3>\n");
        return 2;
    }

    PluginRegistry registry;
    PluginFormat* format = registry.formatFor(argv[1]);
    if (format == nullptr)
    {
        std::fprintf(stderr, "formato non supportato in questa build\n");
        return 3;
    }

    std::string error;
    const auto plugins = format->scan(argv[1], error);
    if (!error.empty())
    {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }

    // Formato a righe separate da tab: semplice da leggere dal processo principale.
    // TODO(MVP): JSON con escaping, quando ci sarà la cache su disco.
    for (const auto& p : plugins)
        std::printf("%s\t%s\t%s\t%s\t%s\t%d\n", p.format.c_str(), p.id.c_str(), p.name.c_str(), p.vendor.c_str(),
                    p.version.c_str(), p.isInstrument ? 1 : 0);
    return 0;
}
