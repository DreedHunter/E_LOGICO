// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/io/Importer.h"

#include <memory>
#include <vector>

namespace elogico
{

class ImporterRegistry
{
public:
    ImporterRegistry();

    Importer* findFor(const std::string& path) const;
    const std::vector<std::unique_ptr<Importer>>& importers() const noexcept { return importers_; }

private:
    std::vector<std::unique_ptr<Importer>> importers_;
};

} // namespace elogico
