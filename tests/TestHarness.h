// SPDX-License-Identifier: AGPL-3.0-or-later
// Mini harness di test: niente dipendenze, così la CI headless non scarica nulla.
// TODO: passare a Catch2 quando servirà (fixture, generatori, benchmark).
#pragma once

#include <cmath>
#include <cstdio>
#include <functional>
#include <vector>

namespace elogico::test
{

struct Case
{
    const char* name;
    std::function<void()> body;
};

inline std::vector<Case>& registry()
{
    static std::vector<Case> cases;
    return cases;
}

inline int& failures()
{
    static int count = 0;
    return count;
}

struct Registrar
{
    Registrar(const char* name, std::function<void()> body) { registry().push_back({name, std::move(body)}); }
};

inline int runAll()
{
    for (const auto& c : registry())
    {
        const int before = failures();
        c.body();
        std::printf("%s %s\n", failures() == before ? "[ OK ]" : "[FAIL]", c.name);
    }
    std::printf("%zu test, %d controlli falliti\n", registry().size(), failures());
    return failures() == 0 ? 0 : 1;
}

} // namespace elogico::test

#define ELOGICO_CONCAT2(a, b) a##b
#define ELOGICO_CONCAT(a, b) ELOGICO_CONCAT2(a, b)

#define TEST_CASE(name)                                                                                   \
    static void ELOGICO_CONCAT(test_fn_, __LINE__)();                                                     \
    static ::elogico::test::Registrar ELOGICO_CONCAT(test_reg_, __LINE__)(name, ELOGICO_CONCAT(test_fn_, __LINE__)); \
    static void ELOGICO_CONCAT(test_fn_, __LINE__)()

#define CHECK(cond)                                                                                       \
    do                                                                                                    \
    {                                                                                                     \
        if (!(cond))                                                                                      \
        {                                                                                                 \
            std::printf("  %s:%d: CHECK(%s) fallito\n", __FILE__, __LINE__, #cond);                       \
            ++::elogico::test::failures();                                                                \
        }                                                                                                 \
    } while (0)

#define CHECK_NEAR(a, b, eps) CHECK(std::fabs(double(a) - double(b)) <= double(eps))

#define TEST_MAIN()                                                                                       \
    int main() { return ::elogico::test::runAll(); }
