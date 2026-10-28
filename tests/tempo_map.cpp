// SPDX-License-Identifier: AGPL-3.0-or-later
#include "TestHarness.h"

#include "elogico/core/TempoMap.h"

using namespace elogico;

TEST_CASE("120 BPM a 48 kHz: una semiminima = 24000 campioni")
{
    TempoMap map(48000.0, 120.0);
    CHECK(map.ticksToSamples(kTicksPerQuarter) == 24000);
    CHECK(map.samplesToTicks(24000) == kTicksPerQuarter);
    CHECK_NEAR(map.samplesToPpq(48000), 2.0, 1e-9);
}

TEST_CASE("cambio di tempo a battuta 2")
{
    TempoMap map(48000.0, 120.0);
    const Ticks bar2 = 4 * kTicksPerQuarter;
    map.setTempo(bar2, 60.0);
    CHECK(map.tempoAt(0) == 120.0);
    CHECK(map.tempoAt(bar2) == 60.0);
    // 4 semiminime a 120 = 96000 campioni, poi una a 60 = 48000.
    CHECK(map.ticksToSamples(bar2) == 96000);
    CHECK(map.ticksToSamples(bar2 + kTicksPerQuarter) == 144000);
    CHECK(map.samplesToTicks(144000) == bar2 + kTicksPerQuarter);
}

TEST_MAIN()
