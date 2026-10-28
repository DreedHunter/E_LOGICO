# ADR 0003 — Toolchain: CMake, MSVC (VS 2022), MinGW per la parte headless

Data: 28 ottobre 2026 · Stato: accettata

## Decisione

Build con CMake 3.25+ e preset. Il compilatore di riferimento è MSVC di Visual Studio 2022 (anche clang-cl). La parte senza GUI (core, engine, platform, model, io, test) deve compilare anche con MinGW-w64 GCC, senza warning.

## Motivi

- JUCE 8, VST3 SDK e ASIO SDK sono pensati per MSVC.
- Una seconda toolchain trova prima codice non portabile e permette build veloci con Ninja.

## Conseguenze

- Le opzioni del compilatore e le define Windows (`_WIN32_WINNT=0x0A00`, `NOMINMAX`...) restano private ai nostri moduli, per non entrare nei sorgenti JUCE.
- Le dipendenze esterne sono a versione fissa in `cmake/Dependencies.cmake`.
