# E_LOGICO

DAW open source italiana per Windows: registrare, editare e mixare con il flusso di lavoro di una DAW classica, aprire i progetti `.logicx` e consumare meno CPU a parità di tracce.

Licenza: **AGPL-3.0-or-later** (vedi [LICENSE](LICENSE)).

## Stato

Scaffold iniziale (ottobre 2026). Funzionano già: grafo audio multicore con compensazione della latenza (PDC), trasporto, strip di canale, uscita WASAPI condivisa, modello progetto con annulla/ripeti, lettura della struttura dei pacchetti `.logicx`, scansione dei descrittori CLAP. La GUI JUCE 8 è abbozzata (barra di controllo, area tracce, mixer).

## Architettura

Quattro livelli; la GUI non parla mai direttamente col motore, ma modifica il modello con comandi.

| Livello | Moduli (`src/`) | Dipendenze |
| --- | --- | --- |
| Interfaccia | `gui`, `app` | JUCE 8 (renderer Direct2D) |
| Modello progetto | `model` (documento, comandi, undo), `io` (import `.logicx`, AAF, MIDI) | — |
| Motore audio | `engine` (grafo RT, PDC, worker pool, trasporto) | nessuna (no JUCE) |
| Piattaforma e DSP | `platform` (WASAPI, ASIO, MMCSS), `plugins` (VST3, CLAP), `dsp`, `core` | header CLAP, VST3 SDK, ASIO SDK (opzionali) |

Strumenti: `elogico-cli` (device, tono di prova, benchmark, report di import) ed `elogico-plugin-scanner` (scansione in processo separato).

Documenti di progetto: [architettura e roadmap](https://claude.ai/artifact/4tzRYWTigdwjKf7ZguBm3B), [specifica del motore audio](https://claude.ai/artifact/TrRFj8viXTF31GW57mf75Z), [mockup GUI](https://claude.ai/artifact/2n3gfPobiseqDCWri7UDvQ). Le decisioni sono in [docs/adr](docs/adr).

## Compilare

Requisiti: Windows 10/11 x64, CMake 3.25+, Visual Studio 2022 con il carico di lavoro **Sviluppo di applicazioni desktop con C++** (compilatore MSVC e Windows SDK).

```bash
cmake --preset windows
cmake --build --preset windows-debug
```

Il primo configure scarica JUCE 8 e gli header CLAP da GitHub. Senza GUI (nessun download):

```bash
cmake --preset headless
cmake --build --preset headless-debug
ctest --preset headless-debug
```

Con MinGW-w64 (GCC 13+) e Ninja si compila la parte senza GUI:

```bash
cmake -S . -B build/mingw -G Ninja -DELOGICO_BUILD_GUI=OFF -DELOGICO_WITH_CLAP=OFF
cmake --build build/mingw
ctest --test-dir build/mingw
```

Opzioni: `ELOGICO_WITH_VST3=ON` (scarica il VST3 SDK), `ELOGICO_ASIO_SDK_DIR=<cartella>` (backend ASIO; l'SDK di Steinberg non va mai committato).

## Regole del thread audio

Nessuna allocazione, nessun lock, nessuna chiamata di sistema dentro `process()`. I grafi si compilano fuori dal thread audio e arrivano con una coda lock-free; i vecchi tornano indietro per essere liberati.

## Note legali

E_LOGICO non è affiliato ad Apple. Non usa icone, grafica, suoni, loop o nomi di prodotto Apple. L'apertura dei file `.logicx` è interoperabilità, sviluppata solo su progetti di prova creati apposta.
