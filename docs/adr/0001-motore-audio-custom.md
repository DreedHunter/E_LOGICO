# ADR 0001 — Motore audio custom in C++20, JUCE solo per la GUI

Data: 28 ottobre 2026 · Stato: accettata

## Decisione

Il motore audio (`src/engine`) è scritto da noi in C++20 e non dipende da JUCE. JUCE 8 è usato per la GUI e le utilità dell'app. Tracktion Engine resta il piano B se il motore custom slitta.

## Motivi

- Il modello di lavoro da replicare (track stack, VCA, Flex, buffer "live" separato dal prefetch) non coincide con Tracktion Engine.
- L'obiettivo "più efficiente di Logic" richiede controllo completo su scheduler, memoria e SIMD.
- Un motore senza GUI si testa in CI senza finestre (test di null della PDC, render deterministico).

## Conseguenze

- Driver (WASAPI, ASIO) e hosting plugin (VST3, CLAP) sono nostri, in `src/platform` e `src/plugins`.
- Il confine tra motore e resto è `AudioCallback`, `GraphDescription` e la coda di `EngineCommand`.
