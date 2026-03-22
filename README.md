# Arkanoid

Výukový projekt – klasická arkádová hra napsaná v **C++** s využitím knihovny **SDL2**.

## Funkce

- Pohyblivá palka ovládaná šipkami
- Kulička s fyzikálně správným odrazem od zdí, palky i cihel
- Mřížka 10 × 4 cihel s detekcí kolizí
- Ohraničení hracího pole
- Úhel odrazu od palky závisí na místě dopadu

## Ovládání

| Klávesa | Akce |
|---|---|
| `←` | Pohyb palky doleva |
| `→` | Pohyb palky doprava |
| `Esc` | Ukončit hru |

## Závislosti

| Knihovna | Verze |
|---|---|
| SDL2 | 2.32.8 |
| SDL2_image | 2.8.8 |

Knihovny jsou stahovány automaticky přes **NuGet** při sestavení.

## Sestavení

### Visual Studio 2022

Otevřete `Arkanoid.sln` a stiskněte **Ctrl+Shift+B**.

### Příkazový řádek

```
nuget restore Arkanoid.sln
msbuild Arkanoid.sln /p:Configuration=Debug /p:Platform=Win32
```

## Struktura projektu

```
Arkanoid/
├── main.cpp          – herní smyčka, kreslení, fyzika, kolize
├── funkce.h          – deklarace datových struktur
├── funkce.cpp        – implementace struktur (bod, vector, palka, cihla, kulicka)
└── images/           – grafické assety
Arkanoid_popis.md     – podrobný popis kódu
```
