// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/core/Types.h"

#include <cstdint>
#include <string>
#include <vector>

namespace elogico
{

/// Regione audio o MIDI sulla timeline, in tick.
/// TODO(M2): loop, fade, guadagno di regione, take folder, marcatori flex.
struct Region
{
    enum class Kind : std::uint8_t
    {
        Audio,
        Midi
    };

    ObjectId id = kInvalidId;
    Kind kind = Kind::Midi;
    std::string name;
    Ticks start = 0;
    Ticks length = 4 * 4 * kTicksPerQuarter; // 4 battute in 4/4
    std::string audioFile; // solo regioni audio, percorso relativo al progetto
};

/// Tipi di strip, gli stessi di Logic per tradurre direttamente l'import .logicx.
enum class TrackType : std::uint8_t
{
    Audio,
    SoftwareInstrument,
    Aux,
    Output
};

struct Track
{
    ObjectId id = kInvalidId;
    TrackType type = TrackType::Audio;
    std::string name;
    std::uint32_t colour = 0xff4aa3e8; // ARGB, dalla palette di progetto
    float volumeDb = 0.0f;
    float pan = 0.0f; // -1 .. +1
    bool muted = false;
    bool soloed = false;
    bool armed = false;
    std::vector<Region> regions;
};

struct TimeSignature
{
    int numerator = 4;
    int denominator = 4;
};

/// Il documento di progetto. Si modifica solo tramite Command (vedi ProjectDocument).
struct Project
{
    std::string name = "Senza titolo";
    double bpm = 120.0;
    TimeSignature timeSignature;
    std::string key = "Do";
    double sampleRate = 48000.0;
    float masterVolumeDb = 0.0f;
    std::vector<Track> tracks;
    ObjectId nextId = 1;

    ObjectId allocateId() noexcept { return nextId++; }
    Track* findTrack(ObjectId id) noexcept;
    const Track* findTrack(ObjectId id) const noexcept;
    Region* findRegion(ObjectId id, Track** owner = nullptr) noexcept;
    bool anySoloed() const noexcept;
};

} // namespace elogico
