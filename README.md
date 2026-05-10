# Arkanoid

Výukový projekt – klasická arkádová hra napsaná v **C++** s využitím knihovny **SDL2**.

## Funkce

- 10 levelů s různým rozložením cihel a narůstající obtížností
- Mřížka 10 × 6 cihel – cihly mají 1–4 životy (barva podle zbývajících zásahů), nezničitelné cihly (tmavě šedé)
- Pohyblivá palka ovládaná šipkami, úhel odrazu závisí na místě dopadu
- Kulička s fyzikálně správným odrazem od zdí, palky i cihel
- Systém životů a skóre, tabulka nejlepších hráčů (uložena v `scores.txt`)
- Menu, herní obrazovka, konec hry / výhra se zadáním jména

## Ovládání

| Klávesa | Akce |
|---|---|
| `←` / `→` | Pohyb palky |
| `Mezerník` | Vypustit míček |
| `↑` / `↓` | Navigace v menu |
| `Enter` | Potvrdit výběr / jméno |
| `Esc` | Zpět do menu / ukončit |

## Závislosti

| Knihovna | Verze |
|---|---|
| SDL2 | 2.32.8 |
| SDL2_image | 2.8.8 |
| SDL2_ttf | 2.24.0 |

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
├── main.cpp          – herní smyčka, kreslení, fyzika, kolize, level data
├── funkce.h          – deklarace datových struktur
├── funkce.cpp        – implementace struktur (bod, vector, palka, cihla, kulicka)
├── images/           – grafické assety (pozadi.bmp, pozadi-full.bmp)
└── scores.txt        – tabulka výsledků (generuje se za běhu)
```
