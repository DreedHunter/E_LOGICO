// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <array>
#include <cstdint>

namespace elogico
{

/// Evento posizionato al campione dentro il blocco: MIDI o cambio di parametro.
struct Event
{
    enum class Type : std::uint8_t
    {
        NoteOn,
        NoteOff,
        Controller,
        PitchBend,
        Parameter
    };

    std::int32_t sampleOffset = 0;
    Type type = Type::NoteOn;
    std::uint8_t channel = 0;
    std::uint8_t note = 0;    // nota MIDI o numero di controller
    std::uint32_t paramId = 0;
    float value = 0.0f;       // velocity normalizzata, valore CC o parametro 0..1
};

/// Lista di eventi a capacità fissa: viene riempita sul thread audio senza allocare.
class EventList
{
public:
    static constexpr int kCapacity = 1024;

    bool add(const Event& e) noexcept
    {
        if (size_ >= kCapacity)
            return false;
        events_[std::size_t(size_++)] = e;
        return true;
    }

    void clear() noexcept { size_ = 0; }
    int size() const noexcept { return size_; }
    const Event& operator[](int i) const noexcept { return events_[std::size_t(i)]; }
    const Event* begin() const noexcept { return events_.data(); }
    const Event* end() const noexcept { return events_.data() + size_; }

private:
    std::array<Event, kCapacity> events_{};
    int size_ = 0;
};

} // namespace elogico
