// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/io/ImportReport.h"
#include "elogico/model/Project.h"

#include <optional>
#include <string>

namespace elogico
{

/// Un importatore di progetti di altre DAW. Gira fuori dal thread audio e non tocca il motore:
/// produce un Project che il ProjectDocument adotta con replace().
class Importer
{
public:
    virtual ~Importer() = default;

    virtual std::string name() const = 0;
    virtual bool canImport(const std::string& path) const = 0;
    virtual std::optional<Project> import(const std::string& path, ImportReport& report) = 0;
};

} // namespace elogico
