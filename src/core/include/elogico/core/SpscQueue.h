// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <optional>
#include <type_traits>

namespace elogico
{

/// Coda lock-free a produttore singolo e consumatore singolo.
/// È il canale tra thread non real-time e thread audio: nessuna allocazione, nessun lock.
/// Capacity deve essere una potenza di due; la coda contiene al massimo Capacity - 1 elementi.
template <typename T, std::size_t Capacity>
class SpscQueue
{
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity deve essere una potenza di due");
    static_assert(std::is_nothrow_move_assignable_v<T>, "T deve essere spostabile senza eccezioni");

public:
    bool push(T value) noexcept
    {
        const auto head = head_.load(std::memory_order_relaxed);
        const auto next = (head + 1) & kMask;
        if (next == tail_.load(std::memory_order_acquire))
            return false; // piena
        slots_[head] = std::move(value);
        head_.store(next, std::memory_order_release);
        return true;
    }

    std::optional<T> pop() noexcept
    {
        const auto tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire))
            return std::nullopt; // vuota
        T value = std::move(slots_[tail]);
        tail_.store((tail + 1) & kMask, std::memory_order_release);
        return value;
    }

    bool empty() const noexcept
    {
        return tail_.load(std::memory_order_acquire) == head_.load(std::memory_order_acquire);
    }

private:
    static constexpr std::size_t kMask = Capacity - 1;

    std::array<T, Capacity> slots_{};
    alignas(64) std::atomic<std::size_t> head_{0};
    alignas(64) std::atomic<std::size_t> tail_{0};
};

} // namespace elogico
