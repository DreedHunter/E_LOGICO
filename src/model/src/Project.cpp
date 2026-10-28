// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/model/Project.h"

#include <algorithm>

namespace elogico
{

Track* Project::findTrack(ObjectId id) noexcept
{
    for (auto& t : tracks)
        if (t.id == id)
            return &t;
    return nullptr;
}

const Track* Project::findTrack(ObjectId id) const noexcept
{
    return const_cast<Project*>(this)->findTrack(id);
}

Region* Project::findRegion(ObjectId id, Track** owner) noexcept
{
    for (auto& t : tracks)
        for (auto& r : t.regions)
            if (r.id == id)
            {
                if (owner != nullptr)
                    *owner = &t;
                return &r;
            }
    return nullptr;
}

bool Project::anySoloed() const noexcept
{
    return std::any_of(tracks.begin(), tracks.end(), [](const Track& t) { return t.soloed; });
}

} // namespace elogico
