# Arkanoid – popis zdrojového kódu

Výukový projekt v C++ s využitím knihovny SDL2.

---

## 1. Přehled projektu

Projekt implementuje klasickou arkádovou hru Arkanoid v jazyce C++ s využitím grafické knihovny SDL2 (Simple DirectMedia Layer). Hra zobrazuje hrací pole s ohraničením, palku ovládanou šipkami, pohybující se kuličku a mřížku cihel, které je třeba sestřelit.

**Soubory projektu:**
- `main.cpp` – hlavní herní logika, kreslení, fyzika, kolize
- `funkce.h` – deklarace datových struktur
- `funkce.cpp` – implementace datových struktur

---

## 2. Datové struktury (funkce.h a funkce.cpp)

### 2.1 bod

Reprezentuje bod v 2D prostoru s desetinnými souřadnicemi.

```cpp
struct bod {
    float x, y;                  // souřadnice bodu
    void nova_pozice(vector v);  // posune bod o vektor v
    int vrat_x();                // vrátí x zaokrouhlené na celé číslo
    int vrat_y();                // vrátí y zaokrouhlené na celé číslo
};
```

### 2.2 vector

Reprezentuje směrový vektor pro pohyb.

```cpp
struct vector {
    float x, y;
    void normalizace();  // upraví délku vektoru na 1.0
};
```

> **Normalizace:** vydělí obě složky délkou vektoru `sqrt(x²+y²)`, výsledek má délku přesně 1.0 a zachovává původní směr.

### 2.3 palka

Datová struktura pro palku. V aktuální verzi jsou pozice palky řízeny přímo přes proměnné `xpos`/`ypos` v main.cpp.

```cpp
struct palka {
    int x, y;        // pozice
    int delka;       // délka palky
    int rychlost;    // rychlost pohybu
    void plus_x();   // posun doprava
    void minus_x();  // posun doleva
};
```

### 2.4 cihla

Jedna cihla v mřížce. Pole `active` určuje, zda cihla ještě existuje.

```cpp
struct cihla {
    int x, y;      // pozice levého horního rohu
    bool active;   // true = cihla existuje, false = zničená
};
```

### 2.5 kulicka

Pohybující se kulička. Používá `float` souřadnice pro plynulý pohyb.

```cpp
struct kulicka {
    float x, y;    // pozice středu
    float dx, dy;  // rychlost: o kolik px se posune každý snímek
    int r;         // poloměr (radius)
    void update(); // provede x += dx; y += dy;
};
```

> Proč `float` a ne `int`? Při použití celých čísel by kulička pohybující se pod malým úhlem „skákala" místo plynulého klouzání. Float umožňuje například rychlost `dx = 1.5 px/snímek`.

---

## 3. Konstanty hry (main.cpp)

Na začátku `main.cpp` jsou definovány všechny číselné hodnoty hry jako konstanty.

```cpp
const int OKNO_SIRKA    = 800;   // šířka okna v pixelech
const int OKNO_VYSKA    = 600;   // výška okna
const int LEVY_OKRAJ    = 20;    // šířka levé zdi
const int PRAVY_OKRAJ   = 780;   // x souřadnice pravé zdi
const int HORNI_OKRAJ   = 20;    // výška horní zdi

const int CIHLA_SIRKA   = 70;    // šířka jedné cihly
const int CIHLA_VYSKA   = 20;    // výška jedné cihly
const int CIHLA_MEZERA  = 5;     // mezera mezi cihlami
const int POCET_SLOUPCU = 10;    // počet sloupců cihel
const int POCET_RADKU   = 4;     // počet řádků cihel
const int RYCHLOST_PALKY = 6;    // px za snímek
```

Počáteční X pozice mřížky cihel se vypočítá automaticky tak, aby byla vycentrovaná:

```cpp
const int CIHLY_START_X = LEVY_OKRAJ +
    ((PRAVY_OKRAJ - LEVY_OKRAJ)
    - (POCET_SLOUPCU * (CIHLA_SIRKA + CIHLA_MEZERA) - CIHLA_MEZERA))
    / 2;
```

---

## 4. Inicializace (main.cpp)

### 4.1 InitImages()

Načte obrázky z disku a převede je na SDL textury. Také zjistí rozměry palky.

```cpp
SDL_Surface *backSurf = SDL_LoadBMP("images/pozadi.bmp");
back = SDL_CreateTextureFromSurface(renderer, backSurf);
SDL_FreeSurface(backSurf);  // surface už nepotřebujeme, GPU má texturu

SDL_QueryTexture(image, NULL, NULL, &palka_sirka, &palka_vyska);
// ^ zjistí rozměry textury palky pro kolize
```

### 4.2 InitCihly()

Naplní dvourozměrné pole cihel. Dvě vnořené smyčky projdou všechny řádky a sloupce.

```cpp
for (int r = 0; r < POCET_RADKU; r++)
    for (int s = 0; s < POCET_SLOUPCU; s++)
    {
        cihly[r][s].x      = CIHLY_START_X + s * (CIHLA_SIRKA + CIHLA_MEZERA);
        cihly[r][s].y      = CIHLY_START_Y + r * (CIHLA_VYSKA + CIHLA_MEZERA);
        cihly[r][s].active = true;
    }
```

### 4.3 ResetMic()

Umístí kuličku nad střed palky a nastaví počáteční rychlost.

```cpp
int stred = xpos + palka_sirka / 2;
mic = kulicka((float)stred, (float)(ypos - 20), 3.0f, -4.0f, 8);
//                                               dx     dy    r
// dy = -4 znamená pohyb nahoru (osa Y roste dolů)
```

---

## 5. Kreslení (DrawScene)

Každý snímek se celá obrazovka vykreslí znovu. Pořadí kreslení je důležité — co se kreslí později, zobrazí se navrchu.

1. Pozadí (`back` textura)
2. Zdi – barevné obdélníky (`SDL_RenderFillRect`)
3. Cihly – barevné obdélníky + obrys
4. Kulička – `DrawCircle()`
5. Palka – textura `palka.png`
6. `SDL_RenderPresent` – zobrazí hotový snímek

### DrawCircle()

SDL2 nemá vestavěnou funkci pro kreslení kruhu. Algoritmus projde čtvercovou oblast kolem středu a nakreslí pouze body uvnitř kružnice pomocí Pythagorovy věty:

```cpp
for (int dy = -r; dy <= r; dy++)
    for (int dx = -r; dx <= r; dx++)
        if (dx*dx + dy*dy <= r*r)   // bod leží uvnitř kružnice
            SDL_RenderDrawPoint(renderer, cx+dx, cy+dy);
```

### Barvy cihel podle řádku

```cpp
Uint8 red   = 220 - r * 40;  // 1. řádek: 220, 2.: 180, 3.: 140, 4.: 100
Uint8 green = 60  + r * 50;  // 1. řádek:  60, 2.: 110, 3.: 160, 4.: 210
```

---

## 6. Fyzika a kolize (UpdateHra)

Tato funkce se volá každý snímek a řeší veškerou fyziku hry.

### 6.1 Pohyb kuličky

```cpp
mic.update();  // x += dx;  y += dy;
```

### 6.2 Kolize se zdmi

Vždy kontrolujeme okraj kuličky (střed ± poloměr), ne střed — jinak by kulička částečně prolézala zdí. Odraz = negace příslušné složky rychlosti.

```cpp
if (mic.x - mic.r <= LEVY_OKRAJ)   { mic.dx = -mic.dx; }  // levá zeď
if (mic.x + mic.r >= PRAVY_OKRAJ)  { mic.dx = -mic.dx; }  // pravá zeď
if (mic.y - mic.r <= HORNI_OKRAJ)  { mic.dy = -mic.dy; }  // horní zeď
if (mic.y - mic.r > OKNO_VYSKA)    { ResetMic(); }         // vypadla dolu
```

### 6.3 Kolize s palkou

Kolize se testuje pouze když kulička letí dolů (`mic.dy > 0`), aby nedošlo k duplicitnímu odrazu.

```cpp
if (mic.dy > 0 &&
    mic.y + mic.r >= ypos &&               // spodní okraj kuličky dosáhl palky
    mic.y - mic.r <= ypos + palka_vyska && // kulička ještě neprolétla palkou
    mic.x + mic.r >= xpos &&              // v rozsahu palky zleva
    mic.x - mic.r <= xpos + palka_sirka)  // v rozsahu palky zprava
{
    mic.dy = -mic.dy;  // odraz nahoru
    float stred = xpos + palka_sirka / 2.0f;
    mic.dx = (mic.x - stred) / (palka_sirka / 2.0f) * 5.0f;
    // výsledek dx: střed palky = 0, levý kraj = -5, pravý kraj = +5
}
```

### 6.4 Kolize s cihlami

Pro každou aktivní cihlu se nejprve otestuje překryv pomocí AABB (Axis-Aligned Bounding Box). Pak se určí strana dopadu podle nejmenšího průniku.

```cpp
// Test překryvu
if (mic.x + mic.r >= bx && mic.x - mic.r <= bx + CIHLA_SIRKA &&
    mic.y + mic.r >= by && mic.y - mic.r <= by + CIHLA_VYSKA)
{
    cihly[r][s].active = false;  // zničení cihly

    // Průniky ze čtyř stran
    float ol = (mic.x + mic.r) - bx;                  // průnik zleva
    float op = (bx + CIHLA_SIRKA) - (mic.x - mic.r);  // průnik zprava
    float ot = (mic.y + mic.r) - by;                  // průnik shora
    float od = (by + CIHLA_VYSKA) - (mic.y - mic.r);  // průnik zdola

    float min_x = ol < op ? ol : op;
    float min_y = ot < od ? ot : od;

    if (min_x < min_y) mic.dx = -mic.dx;  // narazila zboku
    else               mic.dy = -mic.dy;  // narazila shora nebo zdola

    return;  // řeší se max. 1 cihla za snímek
}
```

---

## 7. Herní smyčka (main)

Herní smyčka běží přibližně 60× za sekundu (řízeno VSync). Každá iterace = jeden snímek hry.

```cpp
while (done == 0)
{
    // 1. Zpracuj události (zavření okna, klávesy)
    while (SDL_PollEvent(&event)) { ... }

    // 2. Pohyb palky + omezení na hrací pole
    if (keys[SDL_SCANCODE_LEFT])  xpos -= RYCHLOST_PALKY;
    if (keys[SDL_SCANCODE_RIGHT]) xpos += RYCHLOST_PALKY;
    if (xpos < LEVY_OKRAJ)                xpos = LEVY_OKRAJ;
    if (xpos + palka_sirka > PRAVY_OKRAJ) xpos = PRAVY_OKRAJ - palka_sirka;

    // 3. Fyzika a kolize
    UpdateHra();

    // 4. Výhra = reset cihel
    if (VsechnyCihlyZniceny()) InitCihly();

    // 5. Vykresli snímek
    DrawScene();
}
```

---

## 8. SDL2 – základní koncepty

| Funkce | Co dělá |
|---|---|
| `SDL_CreateWindow` | Vytvoří okno operačního systému |
| `SDL_CreateRenderer` | Vytvoří GPU renderer pro kreslení do okna |
| `SDL_LoadBMP` / `IMG_Load` | Načte obrázek z disku do paměti (SDL_Surface) |
| `SDL_CreateTextureFromSurface` | Nahraje obrázek na GPU jako texturu |
| `SDL_RenderClear` | Smaže celou obrazovku (připraví nový snímek) |
| `SDL_RenderCopy` | Nakreslí texturu na zadané souřadnice |
| `SDL_RenderFillRect` | Nakreslí vyplněný obdélník |
| `SDL_RenderDrawPoint` | Nakreslí jeden pixel |
| `SDL_RenderPresent` | Zobrazí hotový snímek (swap bufferů) |
| `SDL_GetKeyboardState` | Vrátí pole stisklých kláves (pro plynulý pohyb) |
| `SDL_PollEvent` | Načte událost (stisk klávesy, zavření okna...) |
