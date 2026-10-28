// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/io/ImporterRegistry.h"

#include "elogico/io/logicx/LogicxImporter.h"

namespace elogico
{

ImporterRegistry::ImporterRegistry()
{
    importers_.push_back(std::make_unique<LogicxImporter>());
    // TODO(MVP): AAF, Final Cut Pro XML, MIDI (livello 0 del piano di import).
}

Importer* ImporterRegistry::findFor(const std::string& path) const
{
    for (const auto& i : importers_)
        if (i->canImport(path))
            return i.get();
    return nullptr;
}

} // namespace elogico
