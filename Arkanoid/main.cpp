#include <iostream>
#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "funkce.h"

#include <SDL.h>
#include <SDL_image.h>

using namespace std;

// ---- Konstanty ---------------------------------------------------------

const int OKNO_SIRKA   = 800;
const int OKNO_VYSKA   = 600;
const int LEVY_OKRAJ   = 20;   // x zacatek hraciho pole
const int PRAVY_OKRAJ  = 780;  // x konec hraciho pole
const int HORNI_OKRAJ  = 20;   // y zacatek hraciho pole

const int CIHLA_SIRKA  = 70;
const int CIHLA_VYSKA  = 20;
const int CIHLA_MEZERA = 5;
const int POCET_SLOUPCU = 10;
const int POCET_RADKU   = 4;
// x tak, aby cihly byly vycentrovany v hracim poli
const int CIHLY_START_X = LEVY_OKRAJ + ((PRAVY_OKRAJ - LEVY_OKRAJ) - (POCET_SLOUPCU * (CIHLA_SIRKA + CIHLA_MEZERA) - CIHLA_MEZERA)) / 2;
const int CIHLY_START_Y = 60;

const int RYCHLOST_PALKY = 6;

// ---- Globalni promenne -------------------------------------------------

SDL_Texture  *back;
SDL_Texture  *image;
SDL_Window   *window;
SDL_Renderer *renderer;

int xpos = 310, ypos = 550;   // pozice palky
int palka_sirka, palka_vyska; // nacteme z textury

cihla cihly[POCET_RADKU][POCET_SLOUPCU];
kulicka  mic;

// ---- Pomocne funkce ----------------------------------------------------

void InitImages()
{
  SDL_Surface *backSurf = SDL_LoadBMP("images/pozadi.bmp");
  back = SDL_CreateTextureFromSurface(renderer, backSurf);
  SDL_FreeSurface(backSurf);

  SDL_Surface *imageSurf = IMG_Load("images/palka.png");
  image = SDL_CreateTextureFromSurface(renderer, imageSurf);
  SDL_FreeSurface(imageSurf);

  SDL_QueryTexture(image, NULL, NULL, &palka_sirka, &palka_vyska);
}

void InitCihly()
{
  for (int r = 0; r < POCET_RADKU; r++)
    for (int s = 0; s < POCET_SLOUPCU; s++)
    {
      cihly[r][s].x      = CIHLY_START_X + s * (CIHLA_SIRKA + CIHLA_MEZERA);
      cihly[r][s].y      = CIHLY_START_Y + r * (CIHLA_VYSKA + CIHLA_MEZERA);
      cihly[r][s].active = true;
    }
}

void ResetMic()
{
  int stred = xpos + palka_sirka / 2;
  mic = kulicka((float)stred, (float)(ypos - 20), 3.0f, -4.0f, 8);
}

bool VsechnyCihlyZniceny()
{
  for (int r = 0; r < POCET_RADKU; r++)
    for (int s = 0; s < POCET_SLOUPCU; s++)
      if (cihly[r][s].active) return false;
  return true;
}

// ---- Kresleni ----------------------------------------------------------

void DrawIMG(SDL_Texture *img, int x, int y)
{
  int w, h;
  SDL_QueryTexture(img, NULL, NULL, &w, &h);
  SDL_Rect dest = {x, y, w, h};
  SDL_RenderCopy(renderer, img, NULL, &dest);
}

void DrawCircle(int cx, int cy, int r)
{
  for (int dy = -r; dy <= r; dy++)
    for (int dx = -r; dx <= r; dx++)
      if (dx*dx + dy*dy <= r*r)
        SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
}

void DrawScene()
{
  SDL_RenderClear(renderer);

  // Pozadi
  DrawIMG(back, 0, 0);

  // Ohraniceni hraciho pole
  SDL_SetRenderDrawColor(renderer, 80, 80, 180, 255);
  SDL_Rect leva   = {0,           0, LEVY_OKRAJ,                OKNO_VYSKA};
  SDL_Rect prava  = {PRAVY_OKRAJ, 0, OKNO_SIRKA - PRAVY_OKRAJ, OKNO_VYSKA};
  SDL_Rect horni  = {0,           0, OKNO_SIRKA,                HORNI_OKRAJ};
  SDL_RenderFillRect(renderer, &leva);
  SDL_RenderFillRect(renderer, &prava);
  SDL_RenderFillRect(renderer, &horni);

  // Cihly
  for (int r = 0; r < POCET_RADKU; r++)
    for (int s = 0; s < POCET_SLOUPCU; s++)
      if (cihly[r][s].active)
      {
        // Kazdy radek ma jinou barvu
        Uint8 red   = (Uint8)(220 - r * 40);
        Uint8 green = (Uint8)(60  + r * 50);
        SDL_SetRenderDrawColor(renderer, red, green, 80, 255);
        SDL_Rect brick = {cihly[r][s].x, cihly[r][s].y, CIHLA_SIRKA - 1, CIHLA_VYSKA - 1};
        SDL_RenderFillRect(renderer, &brick);

        // Obrys cihly
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderDrawRect(renderer, &brick);
      }

  // Kulicka
  SDL_SetRenderDrawColor(renderer, 255, 230, 50, 255);
  DrawCircle((int)mic.x, (int)mic.y, mic.r);

  // Palka
  DrawIMG(image, xpos, ypos);

  SDL_RenderPresent(renderer);
}

// ---- Fyzika a kolize ---------------------------------------------------

void UpdateHra()
{
  // Pohyb kulicky
  mic.update();

  // Kolize se stenami
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

  // Kulicka vypadla dolu -> reset
  if (mic.y - mic.r > OKNO_VYSKA)
  {
    ResetMic();
    return;
  }

  // Kolize s palkou (pouze kdyz kulicka jede dolu)
  if (mic.dy > 0 &&
      mic.y + mic.r >= ypos &&
      mic.y - mic.r <= ypos + palka_vyska &&
      mic.x + mic.r >= xpos &&
      mic.x - mic.r <= xpos + palka_sirka)
  {
    mic.dy = -mic.dy;
    mic.y  = (float)(ypos - mic.r);

    // Uhel odrazu zavisi na miste dopadu na palce
    float stred = xpos + palka_sirka / 2.0f;
    mic.dx = (mic.x - stred) / (palka_sirka / 2.0f) * 5.0f;
  }

  // Kolize s cihlami
  for (int r = 0; r < POCET_RADKU; r++)
    for (int s = 0; s < POCET_SLOUPCU; s++)
    {
      if (!cihly[r][s].active) continue;

      int bx = cihly[r][s].x;
      int by = cihly[r][s].y;

      if (mic.x + mic.r >= bx &&
          mic.x - mic.r <= bx + CIHLA_SIRKA &&
          mic.y + mic.r >= by &&
          mic.y - mic.r <= by + CIHLA_VYSKA)
      {
        cihly[r][s].active = false;

        // Urcime ze ktere strany kulicka narazila (nejmensi prusnik)
        float ol = (mic.x + mic.r) - bx;
        float op = (bx + CIHLA_SIRKA) - (mic.x - mic.r);
        float ot = (mic.y + mic.r) - by;
        float od = (by + CIHLA_VYSKA) - (mic.y - mic.r);

        float min_x = ol < op ? ol : op;
        float min_y = ot < od ? ot : od;

        if (min_x < min_y)
          mic.dx = -mic.dx;
        else
          mic.dy = -mic.dy;

        // Resit jen jednu cihlu za snimek
        return;
      }
    }
}

// ---- Main --------------------------------------------------------------

int main(int argc, char *argv[])
{
  const Uint8 *keys;

  if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
  {
    printf("Unable to init SDL: %s\n", SDL_GetError());
    exit(1);
  }
  atexit(SDL_Quit);

  window = SDL_CreateWindow("Arkanoid", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            OKNO_SIRKA, OKNO_VYSKA, 0);
  if (window == NULL) { printf("Unable to create window: %s\n", SDL_GetError()); exit(1); }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) { printf("Unable to create renderer: %s\n", SDL_GetError()); exit(1); }

  InitImages();
  InitCihly();
  ResetMic();

  int done = 0;

  while (done == 0)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT) { done = 1; }
      if (event.type == SDL_KEYDOWN)
        if (event.key.keysym.sym == SDLK_ESCAPE) { done = 1; }
    }

    // Pohyb palky
    keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_LEFT])  xpos -= RYCHLOST_PALKY;
    if (keys[SDL_SCANCODE_RIGHT]) xpos += RYCHLOST_PALKY;

    // Palka nezajede za okraj
    if (xpos < LEVY_OKRAJ)                    xpos = LEVY_OKRAJ;
    if (xpos + palka_sirka > PRAVY_OKRAJ)     xpos = PRAVY_OKRAJ - palka_sirka;

    UpdateHra();

    // Vsechny cihly zniceny -> reset
    if (VsechnyCihlyZniceny()) InitCihly();

    DrawScene();
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  return 0;
}
