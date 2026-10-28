# ADR 0002 — Licenza AGPL-3.0-or-later per tutto il repository

Data: 28 ottobre 2026 · Stato: accettata

## Decisione

Tutto il repository è AGPL-3.0-or-later, con un'unica licenza. Ogni file sorgente inizia con `SPDX-License-Identifier: AGPL-3.0-or-later`.

## Motivi

- L'uso gratuito di JUCE 8 richiede AGPLv3 per il programma completo.
- Il codice GPLv3 di terzi (Rubber Band, Surge XT) resta combinabile grazie alla sezione 13 della GPLv3.

## Conseguenze

- Ogni nuova dipendenza va verificata prima dell'uso; da evitare componenti con clausole non commerciali (es. modelli di madmom).
- ASIO SDK e VST3 SDK: verificare la licenza al momento dell'integrazione, Steinberg le ha cambiate di recente.
