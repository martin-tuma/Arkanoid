#include <iostream>
#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funkce.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

using namespace std;

// ---- Konstanty ---------------------------------------------------------

const int OKNO_VYSKA    = 600;
const int LEVY_OKRAJ    = 0;
const int HORNI_OKRAJ   = 0;

const int CIHLA_SIRKA   = 60;
const int CIHLA_VYSKA   = 20;
const int CIHLA_MEZERA  = 0;
const int POCET_SLOUPCU = 10;
const int POCET_RADKU   = 6;
const int PRAVY_OKRAJ   = LEVY_OKRAJ + POCET_SLOUPCU * (CIHLA_SIRKA + CIHLA_MEZERA) - CIHLA_MEZERA;
const int CIHLY_START_X = LEVY_OKRAJ;
const int CIHLY_START_Y = 60;

const int OKNO_SIRKA    = PRAVY_OKRAJ + 200;  // hrací pole + panel vpravo

const int RYCHLOST_PALKY  = 9;
const float RYCHLOST_MICKU = 7.0f;

const int PALKA_VYSKA     = 12;
const int MAX_VYSLEDKU    = 15;
const int POCET_LEVELU    = 10;

// ---- Rozložení levelů --------------------------------------------------
// 0 = prázdno, 1–4 = cihla se zadaným počtem zásahů, 9 = nezničitelná

const int LEVEL_DATA[POCET_LEVELU][POCET_RADKU][POCET_SLOUPCU] = {
  // Level 1 – Řady (plné 3 řady, vše 1 zásah)
  {
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
  },
  // Level 2 – Střídání (šachovnice 1–2 zásahy)
  {
    {2,0,2,0,2,0,2,0,2,0},
    {0,2,0,2,0,2,0,2,0,2},
    {1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
  },
  // Level 3 – Pyramida (1–2 zásahy)
  {
    {0,0,0,0,1,1,0,0,0,0},
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,1,1,2,2,1,1,0,0},
    {0,1,1,2,2,2,2,1,1,0},
    {1,1,2,2,2,2,2,2,1,1},
    {0,0,0,0,0,0,0,0,0,0},
  },
  // Level 4 – Diamant (nezničitelné rohy, 1–3 zásahy)
  {
    {9,0,0,0,1,1,0,0,0,9},
    {0,0,0,1,2,2,1,0,0,0},
    {0,0,1,2,3,3,2,1,0,0},
    {0,0,1,2,3,3,2,1,0,0},
    {0,0,0,1,2,2,1,0,0,0},
    {9,0,0,0,1,1,0,0,0,9},
  },
  // Level 5 – Rám (nezničitelné pilíře, 2 zásahy)
  {
    {2,2,2,2,2,2,2,2,2,2},
    {2,9,0,0,0,0,0,0,9,2},
    {2,0,1,1,1,1,1,1,0,2},
    {2,0,1,1,1,1,1,1,0,2},
    {2,9,0,0,0,0,0,0,9,2},
    {2,2,2,2,2,2,2,2,2,2},
  },
  // Level 6 – Kříž (nezničitelné boxy v rozích, 2–3 zásahy)
  {
    {0,0,0,0,2,2,0,0,0,0},
    {0,0,0,0,3,3,0,0,0,0},
    {2,2,3,3,3,3,3,3,2,2},
    {2,2,3,3,3,3,3,3,2,2},
    {0,0,0,0,3,3,0,0,0,0},
    {9,9,0,0,2,2,0,0,9,9},
  },
  // Level 7 – Labyrint (nezničitelné stěny, 3 zásahy)
  {
    {3,3,3,9,0,0,9,3,3,3},
    {3,0,0,9,3,3,9,0,0,3},
    {3,0,3,0,3,3,0,3,0,3},
    {3,0,3,0,3,3,0,3,0,3},
    {3,0,0,9,3,3,9,0,0,3},
    {3,3,3,9,0,0,9,3,3,3},
  },
  // Level 8 – Vlny (střídání nezničitelných, 3–4 zásahy)
  {
    {4,3,4,3,4,3,4,3,4,3},
    {3,4,3,4,3,4,3,4,3,4},
    {0,9,0,9,0,9,0,9,0,9},
    {9,0,9,0,9,0,9,0,9,0},
    {3,4,3,4,3,4,3,4,3,4},
    {4,3,4,3,4,3,4,3,4,3},
  },
  // Level 9 – Hvězda (nezničitelné body hvězdy, 4 zásahy)
  {
    {0,0,9,0,4,4,0,9,0,0},
    {0,9,4,4,4,4,4,4,9,0},
    {9,4,4,3,3,3,3,4,4,9},
    {9,4,4,3,3,3,3,4,4,9},
    {0,9,4,4,4,4,4,4,9,0},
    {0,0,9,0,4,4,0,9,0,0},
  },
  // Level 10 – Pevnost (nezničitelné věže, 3–4 zásahy)
  {
    {9,4,4,4,4,4,4,4,4,9},
    {4,9,3,3,3,3,3,3,9,4},
    {4,3,9,2,2,2,2,9,3,4},
    {4,3,9,2,2,2,2,9,3,4},
    {4,9,3,3,3,3,3,3,9,4},
    {9,4,4,4,4,4,4,4,4,9},
  },
};

// ---- Herní stav --------------------------------------------------------

enum GameState { STAV_MENU, STAV_HRA, STAV_GAME_OVER, STAV_VYSLEDKY };
GameState stav = STAV_MENU;

// ---- High score --------------------------------------------------------

struct ScoreEntry {
  char jmeno[17];
  int  skore;
};

ScoreEntry vysledky[MAX_VYSLEDKU];
int pocet_vysledku  = 0;
int posledni_vlozeny = -1;

// ---- Globální proměnné -------------------------------------------------

SDL_Texture  *back;
SDL_Texture  *back_full;
SDL_Window   *window;
SDL_Renderer *renderer;
TTF_Font     *font_velky;
TTF_Font     *font_maly;

int   level_aktualni = 0;
bool  vyhral_hru     = false;

int   xpos        = 210, ypos = 570;
int   palka_delka = 120;

cihla   cihly[POCET_RADKU][POCET_SLOUPCU];
kulicka mic;
bool    ceka_na_start = true;
int     zivoty = 3;
int     skore  = 0;
int     menu_volba = 0;

char vstup_jmeno[17] = "";
int  vstup_delka = 0;

// ---- High score – soubor -----------------------------------------------

void NactiVysledky()
{
  FILE *f = fopen("scores.txt", "r");
  if (!f) return;
  pocet_vysledku = 0;
  while (pocet_vysledku < MAX_VYSLEDKU &&
         fscanf(f, "%16s %d", vysledky[pocet_vysledku].jmeno,
                               &vysledky[pocet_vysledku].skore) == 2)
    pocet_vysledku++;
  fclose(f);
}

void UlozVysledky()
{
  FILE *f = fopen("scores.txt", "w");
  if (!f) return;
  for (int i = 0; i < pocet_vysledku; i++)
    fprintf(f, "%s %d\n", vysledky[i].jmeno, vysledky[i].skore);
  fclose(f);
}

// Vrátí true pokud bylo skóre vloženo
bool VlozVysledek(const char *jmeno, int s)
{
  if (pocet_vysledku >= MAX_VYSLEDKU && s <= vysledky[MAX_VYSLEDKU - 1].skore)
    return false;

  if (pocet_vysledku < MAX_VYSLEDKU) pocet_vysledku++;

  int pos = pocet_vysledku - 1;
  while (pos > 0 && vysledky[pos - 1].skore < s)
  {
    vysledky[pos] = vysledky[pos - 1];
    pos--;
  }
  strncpy(vysledky[pos].jmeno, jmeno, 16);
  vysledky[pos].jmeno[16] = 0;
  vysledky[pos].skore = s;
  posledni_vlozeny = pos;
  return true;
}

// ---- Herní inicializace ------------------------------------------------

void InitImages()
{
  SDL_Surface *backSurf = SDL_LoadBMP("images/pozadi.bmp");
  back = SDL_CreateTextureFromSurface(renderer, backSurf);
  SDL_FreeSurface(backSurf);

  SDL_Surface *backFullSurf = SDL_LoadBMP("images/pozadi-full.bmp");
  back_full = SDL_CreateTextureFromSurface(renderer, backFullSurf);
  SDL_FreeSurface(backFullSurf);
}

void InitCihly(int level)
{
  for (int r = 0; r < POCET_RADKU; r++)
    for (int s = 0; s < POCET_SLOUPCU; s++)
    {
      int val = LEVEL_DATA[level][r][s];
      cihly[r][s].x = CIHLY_START_X + s * (CIHLA_SIRKA + CIHLA_MEZERA);
      cihly[r][s].y = CIHLY_START_Y + r * (CIHLA_VYSKA + CIHLA_MEZERA);
      if (val == 9)
      {
        cihly[r][s].indestructible = true;
        cihly[r][s].hits           = 0;
        cihly[r][s].max_hits       = 0;
      }
      else
      {
        cihly[r][s].indestructible = false;
        cihly[r][s].hits           = val;
        cihly[r][s].max_hits       = val;
      }
    }
}

void ResetMic()
{
  mic.r  = 8;
  mic.dx = RYCHLOST_MICKU * 0.6f;
  mic.dy = -RYCHLOST_MICKU * 0.8f;
  ceka_na_start = true;
}

void ResetHra()
{
  zivoty         = 3;
  skore          = 0;
  level_aktualni = 0;
  vyhral_hru     = false;
  xpos           = 210;
  palka_delka    = 120;
  InitCihly(level_aktualni);
  ResetMic();
}

bool VsechnyCihlyZniceny()
{
  for (int r = 0; r < POCET_RADKU; r++)
    for (int s = 0; s < POCET_SLOUPCU; s++)
      if (!cihly[r][s].indestructible && cihly[r][s].hits > 0) return false;
  return true;
}

// ---- Kreslení ----------------------------------------------------------

void DrawCircle(int cx, int cy, int r)
{
  for (int dy = -r; dy <= r; dy++)
    for (int dx = -r; dx <= r; dx++)
      if (dx*dx + dy*dy <= r*r)
        SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
}

void DrawIMG(SDL_Texture *img, int x, int y)
{
  int w, h;
  SDL_QueryTexture(img, NULL, NULL, &w, &h);
  SDL_Rect dest = {x, y, w, h};
  SDL_RenderCopy(renderer, img, NULL, &dest);
}

void DrawText(TTF_Font *font, const char *text, int x, int y, SDL_Color barva)
{
  SDL_Surface *surf = TTF_RenderUTF8_Blended(font, text, barva);
  SDL_Texture *tex  = SDL_CreateTextureFromSurface(renderer, surf);
  int w, h;
  SDL_QueryTexture(tex, NULL, NULL, &w, &h);
  SDL_Rect dest = {x, y, w, h};
  SDL_RenderCopy(renderer, tex, NULL, &dest);
  SDL_DestroyTexture(tex);
  SDL_FreeSurface(surf);
}

void DrawTextCentered(TTF_Font *font, const char *text, int cx, int y, SDL_Color barva)
{
  SDL_Surface *surf = TTF_RenderUTF8_Blended(font, text, barva);
  SDL_Texture *tex  = SDL_CreateTextureFromSurface(renderer, surf);
  int w, h;
  SDL_QueryTexture(tex, NULL, NULL, &w, &h);
  SDL_Rect dest = {cx - w / 2, y, w, h};
  SDL_RenderCopy(renderer, tex, NULL, &dest);
  SDL_DestroyTexture(tex);
  SDL_FreeSurface(surf);
}

// Pomocná pro panel vpravo – životy a skóre
void DrawPanel()
{
  int panel_cx = PRAVY_OKRAJ + (OKNO_SIRKA - PRAVY_OKRAJ) / 2;

  SDL_Color bila  = {255, 255, 255, 255};
  SDL_Color zluta = {255, 220, 50,  255};

  // Skóre
  char buf[32];
  sprintf(buf, "%d", skore);
  DrawTextCentered(font_maly, "SKORE", panel_cx, 20,  bila);
  DrawTextCentered(font_maly, buf,     panel_cx, 50,  zluta);

  // Level
  char lbuf[16];
  sprintf(lbuf, "%d / %d", level_aktualni + 1, POCET_LEVELU);
  DrawTextCentered(font_maly, "LEVEL",  panel_cx, 100, bila);
  DrawTextCentered(font_maly, lbuf,     panel_cx, 130, zluta);

  // Životy – červené kuličky
  DrawTextCentered(font_maly, "ZIVOTY", panel_cx, 185, bila);
  int r = 8, mezera = 25;
  int start_x = panel_cx - ((zivoty - 1) * mezera) / 2;
  for (int i = 0; i < zivoty; i++)
  {
    SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
    DrawCircle(start_x + i * mezera, 220, r);
  }
}

void DrawHra()
{
  SDL_RenderClear(renderer);
  DrawIMG(back, 0, 0);

  // Oddělovací čára
  SDL_SetRenderDrawColor(renderer, 80, 80, 180, 255);
  SDL_RenderDrawLine(renderer, PRAVY_OKRAJ, 0, PRAVY_OKRAJ, OKNO_VYSKA);

  DrawPanel();

  // Cihly – barva odpovídá zbývajícím hitům
  // 1 hit = zelená, 2 = žlutá, 3 = oranžová, 4+ = červená
  static const SDL_Color BARVY_CIHEL[] = {
    {80,  200, 80,  255},  // 1 hit  – zelená
    {220, 220, 40,  255},  // 2 hity – žlutá
    {230, 130, 30,  255},  // 3 hity – oranžová
    {210, 50,  50,  255},  // 4 hity – červená
  };
  for (int r = 0; r < POCET_RADKU; r++)
    for (int s = 0; s < POCET_SLOUPCU; s++)
      if (cihly[r][s].active())
      {
        SDL_Rect brick = {cihly[r][s].x, cihly[r][s].y, CIHLA_SIRKA - 1, CIHLA_VYSKA - 1};
        if (cihly[r][s].indestructible)
        {
          SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
          SDL_RenderFillRect(renderer, &brick);
          SDL_SetRenderDrawColor(renderer, 130, 130, 130, 255);
          SDL_RenderDrawRect(renderer, &brick);
        }
        else
        {
          int idx = cihly[r][s].hits - 1;
          if (idx < 0) idx = 0;
          if (idx > 3) idx = 3;
          SDL_Color c = BARVY_CIHEL[idx];
          SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
          SDL_RenderFillRect(renderer, &brick);
          SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
          SDL_RenderDrawRect(renderer, &brick);
        }
      }

  // Kulička
  SDL_SetRenderDrawColor(renderer, 255, 230, 50, 255);
  DrawCircle((int)mic.x, (int)mic.y, mic.r);

  // Palka (obdélník)
  SDL_SetRenderDrawColor(renderer, 80, 160, 255, 255);
  SDL_Rect p = {xpos, ypos, palka_delka, PALKA_VYSKA};
  SDL_RenderFillRect(renderer, &p);
  SDL_SetRenderDrawColor(renderer, 180, 220, 255, 255);
  SDL_RenderDrawLine(renderer, xpos, ypos, xpos + palka_delka - 1, ypos);

  // Nápověda při čekání
  if (ceka_na_start)
  {
    SDL_Color bila = {255, 255, 255, 200};
    DrawTextCentered(font_maly, "STISKNI MEZERNIK", PRAVY_OKRAJ / 2, OKNO_VYSKA / 2, bila);
  }

  SDL_RenderPresent(renderer);
}

void DrawMenu()
{
  SDL_RenderClear(renderer);
  DrawIMG(back_full, 0, 0);

  SDL_Color bila  = {255, 255, 255, 255};
  SDL_Color zluta = {255, 220, 50,  255};

  DrawTextCentered(font_velky, "ARKANOID", OKNO_SIRKA / 2, 120, bila);

  const char *volby[] = {"NOVA HRA", "VYSLEDKY", "KONEC"};
  for (int i = 0; i < 3; i++)
    DrawTextCentered(font_maly, volby[i], OKNO_SIRKA / 2, 270 + i * 70,
                     (i == menu_volba) ? zluta : bila);

  SDL_RenderPresent(renderer);
}

void DrawGameOver()
{
  SDL_RenderClear(renderer);
  DrawIMG(back_full, 0, 0);

  SDL_Color bila  = {255, 255, 255, 255};
  SDL_Color zluta = {255, 220, 50,  255};
  SDL_Color seda  = {160, 160, 160, 255};

  DrawTextCentered(font_velky, vyhral_hru ? "VYHRAL JSI!" : "KONEC HRY", OKNO_SIRKA / 2, 120, bila);

  char buf[64];
  sprintf(buf, "Skore: %d", skore);
  DrawTextCentered(font_maly, buf,               OKNO_SIRKA / 2, 220, zluta);
  DrawTextCentered(font_maly, "Zadejte jmeno:",  OKNO_SIRKA / 2, 300, bila);
  DrawTextCentered(font_maly, vstup_jmeno,       OKNO_SIRKA / 2, 350, zluta);
  DrawTextCentered(font_maly, "ENTER = potvrdit",OKNO_SIRKA / 2, 460, seda);

  SDL_RenderPresent(renderer);
}

void DrawVysledky()
{
  SDL_RenderClear(renderer);
  DrawIMG(back_full, 0, 0);

  SDL_Color bila  = {255, 255, 255, 255};
  SDL_Color zluta = {255, 220, 50,  255};
  SDL_Color seda  = {160, 160, 160, 255};

  DrawTextCentered(font_velky, "NEJLEPSI HRACE", OKNO_SIRKA / 2, 20, bila);

  for (int i = 0; i < pocet_vysledku; i++)
  {
    char radek[64];
    sprintf(radek, "%2d.  %-16s  %d", i + 1, vysledky[i].jmeno, vysledky[i].skore);
    SDL_Color c = (i == posledni_vlozeny) ? zluta : bila;
    DrawText(font_maly, radek, 150, 95 + i * 30, c);
  }

  DrawTextCentered(font_maly, "ESC = zpet do menu", OKNO_SIRKA / 2, 570, seda);

  SDL_RenderPresent(renderer);
}

// ---- Fyzika a kolize ---------------------------------------------------

void UpdateHra()
{
  mic.update();

  // Kolize se zdmi
  if (mic.x - mic.r <= LEVY_OKRAJ)
  {
    mic.x = (float)(LEVY_OKRAJ + mic.r);
    mic.dx = -mic.dx;
  }
  if (mic.x + mic.r >= PRAVY_OKRAJ)
  {
    mic.x = (float)(PRAVY_OKRAJ - mic.r);
    mic.dx = -mic.dx;
  }
  if (mic.y - mic.r <= HORNI_OKRAJ)
  {
    mic.y = (float)(HORNI_OKRAJ + mic.r);
    mic.dy = -mic.dy;
  }

  // Vypadnutí dolu → odebrat život
  if (mic.y - mic.r > OKNO_VYSKA)
  {
    zivoty--;
    if (zivoty <= 0)
    {
      // Přechod na zadání jména
      stav = STAV_GAME_OVER;
      vstup_jmeno[0] = 0;
      vstup_delka = 0;
      SDL_StartTextInput();
    }
    else
    {
      ResetMic();
    }
    return;
  }

  // Kolize s palkou
  if (mic.dy > 0 &&
      mic.y + mic.r >= ypos &&
      mic.y - mic.r <= ypos + PALKA_VYSKA &&
      mic.x + mic.r >= xpos &&
      mic.x - mic.r <= xpos + palka_delka)
  {
    mic.dy = -mic.dy;
    mic.y  = (float)(ypos - mic.r);
    float stred = xpos + palka_delka / 2.0f;
    mic.dx = (mic.x - stred) / (palka_delka / 2.0f) * 5.0f;
  }

  // Kolize s cihlami
  for (int r = 0; r < POCET_RADKU; r++)
    for (int s = 0; s < POCET_SLOUPCU; s++)
    {
      if (!cihly[r][s].active()) continue;

      int bx = cihly[r][s].x;
      int by = cihly[r][s].y;

      if (mic.x + mic.r >= bx &&
          mic.x - mic.r <= bx + CIHLA_SIRKA &&
          mic.y + mic.r >= by &&
          mic.y - mic.r <= by + CIHLA_VYSKA)
      {
        if (!cihly[r][s].indestructible)
        {
          cihly[r][s].hits--;
          skore += 10;
        }

        float ol = (mic.x + mic.r) - bx;
        float op = (bx + CIHLA_SIRKA) - (mic.x - mic.r);
        float ot = (mic.y + mic.r) - by;
        float od = (by + CIHLA_VYSKA) - (mic.y - mic.r);
        float min_x = ol < op ? ol : op;
        float min_y = ot < od ? ot : od;

        if (min_x < min_y) mic.dx = -mic.dx;
        else               mic.dy = -mic.dy;

        return;
      }
    }
}

// ---- Main --------------------------------------------------------------

int main(int argc, char *argv[])
{
  if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
  { printf("SDL_Init: %s\n", SDL_GetError()); exit(1); }

  if (TTF_Init() < 0)
  { printf("TTF_Init: %s\n", TTF_GetError()); exit(1); }

  atexit(SDL_Quit);
  atexit(TTF_Quit);

  window = SDL_CreateWindow("Arkanoid", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            OKNO_SIRKA, OKNO_VYSKA, 0);
  if (!window) { printf("Window: %s\n", SDL_GetError()); exit(1); }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) { printf("Renderer: %s\n", SDL_GetError()); exit(1); }

  // Font – Arial z Windows
  font_velky = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 48);
  font_maly  = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24);
  if (!font_velky || !font_maly)
  { printf("Font: %s\n", TTF_GetError()); exit(1); }

  InitImages();
  NactiVysledky();
  ResetHra();

  int done = 0;

  while (done == 0)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT) { done = 1; break; }

      // ---- MENU ----
      if (stav == STAV_MENU && event.type == SDL_KEYDOWN)
      {
        switch (event.key.keysym.sym)
        {
          case SDLK_ESCAPE: done = 1; break;
          case SDLK_UP:     menu_volba = (menu_volba + 2) % 3; break;
          case SDLK_DOWN:   menu_volba = (menu_volba + 1) % 3; break;
          case SDLK_RETURN:
            if (menu_volba == 0) { ResetHra(); stav = STAV_HRA; }
            if (menu_volba == 1) { posledni_vlozeny = -1; stav = STAV_VYSLEDKY; }
            if (menu_volba == 2)   done = 1;
            break;
        }
      }

      // ---- HRA ----
      else if (stav == STAV_HRA && event.type == SDL_KEYDOWN)
      {
        if (event.key.keysym.sym == SDLK_ESCAPE) { stav = STAV_MENU; }
        if (event.key.keysym.sym == SDLK_SPACE && ceka_na_start)
        {
          ceka_na_start = false;
          mic.dx = 0.0f;
          mic.dy = -RYCHLOST_MICKU;
        }
      }

      // ---- GAME OVER – zadávání jména ----
      else if (stav == STAV_GAME_OVER)
      {
        if (event.type == SDL_TEXTINPUT && vstup_delka < 16)
        {
          strncat(vstup_jmeno, event.text.text, 16 - vstup_delka);
          vstup_delka = (int)strlen(vstup_jmeno);
        }
        if (event.type == SDL_KEYDOWN)
        {
          if (event.key.keysym.sym == SDLK_BACKSPACE && vstup_delka > 0)
          {
            vstup_jmeno[--vstup_delka] = 0;
          }
          if (event.key.keysym.sym == SDLK_RETURN)
          {
            SDL_StopTextInput();
            if (vstup_delka == 0) strcpy(vstup_jmeno, "---");
            VlozVysledek(vstup_jmeno, skore);
            UlozVysledky();
            stav = STAV_VYSLEDKY;
          }
        }
      }

      // ---- VÝSLEDKY ----
      else if (stav == STAV_VYSLEDKY && event.type == SDL_KEYDOWN)
      {
        if (event.key.keysym.sym == SDLK_ESCAPE ||
            event.key.keysym.sym == SDLK_RETURN)
          stav = STAV_MENU;
      }
    }

    // ---- Update hry ----
    if (stav == STAV_HRA)
    {
      const Uint8 *keys = SDL_GetKeyboardState(NULL);
      if (keys[SDL_SCANCODE_LEFT])  xpos -= RYCHLOST_PALKY;
      if (keys[SDL_SCANCODE_RIGHT]) xpos += RYCHLOST_PALKY;
      if (xpos < LEVY_OKRAJ)                    xpos = LEVY_OKRAJ;
      if (xpos + palka_delka > PRAVY_OKRAJ)     xpos = PRAVY_OKRAJ - palka_delka;

      if (ceka_na_start)
      {
        mic.x = (float)(xpos + palka_delka / 2);
        mic.y = (float)(ypos - mic.r);
      }
      else
      {
        UpdateHra();
      }

      if (VsechnyCihlyZniceny())
      {
        level_aktualni++;
        if (level_aktualni >= POCET_LEVELU)
        {
          vyhral_hru = true;
          stav = STAV_GAME_OVER;
          vstup_jmeno[0] = 0;
          vstup_delka = 0;
          SDL_StartTextInput();
        }
        else
        {
          InitCihly(level_aktualni);
          ResetMic();
        }
      }
    }

    // ---- Kreslení ----
    switch (stav)
    {
      case STAV_MENU:      DrawMenu();     break;
      case STAV_HRA:       DrawHra();      break;
      case STAV_GAME_OVER: DrawGameOver(); break;
      case STAV_VYSLEDKY:  DrawVysledky(); break;
    }
  }

  TTF_CloseFont(font_velky);
  TTF_CloseFont(font_maly);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  return 0;
}
