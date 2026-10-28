// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#if defined(_M_X64) || defined(__x86_64__) || defined(_M_IX86) || defined(__i386__)
#include <xmmintrin.h>
#define ELOGICO_HAS_SSE_CSR 1
#endif

namespace elogico
{

/// Attiva flush-to-zero e denormals-are-zero per la durata dello scope.
/// Va usato in testa a ogni callback audio: evita picchi di CPU sulle code dei riverberi.
class ScopedNoDenormals
{
public:
    ScopedNoDenormals() noexcept
    {
#if ELOGICO_HAS_SSE_CSR
        saved_ = _mm_getcsr();
        _mm_setcsr(saved_ | 0x8040u); // FTZ (bit 15) + DAZ (bit 6)
#endif
    }

    ~ScopedNoDenormals()
    {
#if ELOGICO_HAS_SSE_CSR
        _mm_setcsr(saved_);
#endif
    }

    ScopedNoDenormals(const ScopedNoDenormals&) = delete;
    ScopedNoDenormals& operator=(const ScopedNoDenormals&) = delete;

private:
    unsigned int saved_ = 0;
};

} // namespace elogico
