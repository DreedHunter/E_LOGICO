// SPDX-License-Identifier: AGPL-3.0-or-later
#include "elogico/engine/WorkerPool.h"

#include "elogico/core/ScopedNoDenormals.h"

#include <algorithm>

namespace elogico
{

WorkerPool::WorkerPool(int numWorkers)
{
    for (int i = 0; i < numWorkers; ++i)
    {
        auto worker = std::make_unique<Worker>();
        Worker& ref = *worker;
        workers_.push_back(std::move(worker));
        ref.thread = std::thread([this, &ref] { workerLoop(ref); });
    }
}

WorkerPool::~WorkerPool()
{
    quit_.store(true, std::memory_order_release);
    for (auto& w : workers_)
        w->wake.release();
    for (auto& w : workers_)
        w->thread.join();
}

int WorkerPool::defaultWorkerCount()
{
    // Il thread audio è già un esecutore: servono core - 1 worker.
    return std::max(0, int(std::thread::hardware_concurrency()) - 1);
}

void WorkerPool::drain() noexcept
{
    std::size_t processed = 0;
    for (std::size_t i = next_.fetch_add(1, std::memory_order_acq_rel); i < count_;
         i = next_.fetch_add(1, std::memory_order_acq_rel))
    {
        fn_(context_, i);
        ++processed;
    }
    if (processed > 0)
        done_.fetch_add(processed, std::memory_order_acq_rel);
}

void WorkerPool::run(std::size_t count, JobFn fn, void* context) noexcept
{
    if (count == 0)
        return;

    fn_ = fn;
    context_ = context;
    count_ = count;
    done_.store(0, std::memory_order_relaxed);
    next_.store(0, std::memory_order_release);

    const std::size_t helpers = std::min(workers_.size(), count - 1);
    active_.store(int(helpers), std::memory_order_release);
    for (std::size_t w = 0; w < helpers; ++w)
        workers_[w]->wake.release();

    drain();

    // Attesa attiva: siamo sul thread audio, non possiamo bloccarci sul kernel.
    // Aspettiamo anche i worker svegliati, così nessuno resta a metà verso il prossimo run().
    while (done_.load(std::memory_order_acquire) < count || active_.load(std::memory_order_acquire) > 0)
        std::this_thread::yield();

    count_ = 0;
}

void WorkerPool::workerLoop(Worker& self)
{
    ScopedNoDenormals noDenormals;
    for (;;)
    {
        self.wake.acquire();
        if (quit_.load(std::memory_order_acquire))
            return;
        drain();
        active_.fetch_sub(1, std::memory_order_acq_rel);
    }
}

} // namespace elogico
