# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

**Visual Studio 2022** (doporučeno): Otevři `Arkanoid.sln`, stiskni `Ctrl+Shift+B`.

**Příkazový řádek:**
```
nuget restore Arkanoid.sln
msbuild Arkanoid.sln /p:Configuration=Debug /p:Platform=Win32
```

SDL2, SDL2_image a SDL2_ttf se stahují automaticky přes NuGet (`packages.config`). Není třeba instalovat knihovny ručně.

## Architektura

Projekt je záměrně jednoduchý (výukový). Veškerá herní logika žije v `main.cpp`:

- **Herní smyčka** — SDL2 event polling + `UpdateHra()` + Draw funkce
- **Stavy hry** — `enum GameState { STAV_MENU, STAV_HRA, STAV_GAME_OVER, STAV_VYSLEDKY }`, globální proměnná `stav`
- **Fyzika/kolize** — v `UpdateHra()`: zdi, palka, cihly (AABB s výpočtem nejmenšího překryvu pro určení osy odrazu)
- **Kreslení** — SDL2 renderer, pozadí z BMP, text přes SDL2_ttf s `C:\Windows\Fonts\arial.ttf`

Datové struktury (`bod`, `vector`, `palka`, `cihla`, `kulicka`) jsou deklarovány v `funkce.h` a implementovány v `funkce.cpp`. `cihla::active()` je inline metoda (`hits > 0`), ne proměnná — volat jako `active()`.

## Assets

- `Arkanoid/images/pozadi.bmp` — pozadí herní plochy (600×600 px)
- `Arkanoid/images/pozadi-full.bmp` — pozadí pro menu a výsledky (800×600 px)
- `scores.txt` — high score, generuje se za běhu vedle `.exe`

Herní plocha je 600 px široká (levý sloupec), panel se skóre a životy je vpravo (600–800 px).
