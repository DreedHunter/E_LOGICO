// SPDX-License-Identifier: AGPL-3.0-or-later
#include "TestHarness.h"

#include "elogico/core/SpscQueue.h"

#include <thread>

using namespace elogico;

TEST_CASE("coda vuota e piena")
{
    SpscQueue<int, 4> q;
    CHECK(q.empty());
    CHECK(!q.pop().has_value());
    CHECK(q.push(1));
    CHECK(q.push(2));
    CHECK(q.push(3));
    CHECK(!q.push(4)); // capacità utile = 3
    CHECK(q.pop().value() == 1);
    CHECK(q.push(4));
}

TEST_CASE("ordine preservato tra due thread")
{
    SpscQueue<int, 1024> q;
    constexpr int kCount = 200000;
    std::thread producer([&] {
        for (int i = 0; i < kCount; ++i)
            while (!q.push(i))
                std::this_thread::yield();
    });

    int expected = 0;
    bool ordered = true;
    while (expected < kCount)
    {
        if (auto v = q.pop())
        {
            ordered = ordered && *v == expected;
            ++expected;
        }
    }
    producer.join();
    CHECK(ordered);
    CHECK(q.empty());
}

TEST_MAIN()
