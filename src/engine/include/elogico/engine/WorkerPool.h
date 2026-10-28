// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <atomic>
#include <cstddef>
#include <memory>
#include <semaphore>
#include <thread>
#include <vector>

namespace elogico
{

/// Pool di worker real-time: un thread per core, il thread audio partecipa al lavoro.
/// TODO(MVP): legare i worker ai P-core (CPU Sets) e registrarli in MMCSS "Pro Audio".
class WorkerPool
{
public:
    explicit WorkerPool(int numWorkers = defaultWorkerCount());
    ~WorkerPool();

    WorkerPool(const WorkerPool&) = delete;
    WorkerPool& operator=(const WorkerPool&) = delete;

    static int defaultWorkerCount();
    int numWorkers() const noexcept { return int(workers_.size()); }

    /// Esegue fn(i) per i in [0, count) e ritorna quando tutti hanno finito. Non alloca.
    template <typename Fn>
    void parallelFor(std::size_t count, Fn&& fn) noexcept
    {
        auto trampoline = [](void* ctx, std::size_t i) noexcept { (*static_cast<Fn*>(ctx))(i); };
        run(count, trampoline, &fn);
    }

private:
    using JobFn = void (*)(void*, std::size_t) noexcept;

    struct Worker
    {
        std::thread thread;
        std::binary_semaphore wake{0};
    };

    void run(std::size_t count, JobFn fn, void* context) noexcept;
    void workerLoop(Worker& self);
    void drain() noexcept;

    std::vector<std::unique_ptr<Worker>> workers_;
    std::atomic<bool> quit_{false};
    std::atomic<std::size_t> next_{0};
    std::atomic<std::size_t> done_{0};
    std::atomic<int> active_{0};
    std::size_t count_ = 0;
    JobFn fn_ = nullptr;
    void* context_ = nullptr;
};

} // namespace elogico
