// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <string>
#include <vector>

namespace elogico
{

/// Il report che l'utente vede sempre all'apertura di un progetto importato:
/// cosa è stato importato, cosa è stato sostituito, cosa manca.
struct ImportReport
{
    enum class Severity
    {
        Imported,
        Replaced,
        Missing,
        Error
    };

    struct Item
    {
        Severity severity = Severity::Imported;
        std::string what;
        std::string detail;
    };

    std::vector<Item> items;

    void imported(std::string what, std::string detail = {}) { items.push_back({Severity::Imported, std::move(what), std::move(detail)}); }
    void replaced(std::string what, std::string detail = {}) { items.push_back({Severity::Replaced, std::move(what), std::move(detail)}); }
    void missing(std::string what, std::string detail = {}) { items.push_back({Severity::Missing, std::move(what), std::move(detail)}); }
    void error(std::string what, std::string detail = {}) { items.push_back({Severity::Error, std::move(what), std::move(detail)}); }

    bool hasErrors() const noexcept
    {
        for (const auto& i : items)
            if (i.severity == Severity::Error)
                return true;
        return false;
    }
};

} // namespace elogico
