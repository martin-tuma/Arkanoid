#include <math.h>
#include "funkce.h"
#include <stdlib.h>

bod::bod(){
	x = 0.0f;
	y = 0.0f;
}

bod::bod(float x, float y) {
	this->x = x;
	this->y = y;
}

void bod::nova_pozice(vector v) {
	x += v.x;
	y += v.y;
}

int bod::vrat_x() {
	return (int)(x + 0.5f);
}

int bod::vrat_y() {
	return (int)(y + 0.5f);
}


vector::vector(){
	x = 0.0f;
	y = 0.0f;
}

vector::vector(float x, float y) {
	this->x = x;
	this->y = y;
}

void vector::normalizace() {
	float velikost = sqrtf(x*x + y*y);
	x /= velikost;
	y /= velikost;
}


palka::palka() : x(0), y(0), delka(0), rychlost(0) {}

palka::palka(int x, int y, int delka, int rychlost) {
	this->x = x;
	this->y = y;
	this->delka = delka;
	this->rychlost = rychlost;
}

void palka::plus_x() {
	x += rychlost;
}

void palka::minus_x() {
	x -= rychlost;
}


cihla::cihla() : x(0), y(0), hits(0), max_hits(1), indestructible(false) {}

cihla::cihla(int x, int y, int hits, bool indestr) {
	this->x             = x;
	this->y             = y;
	this->indestructible = indestr;
	this->hits          = indestr ? 0 : hits;
	this->max_hits      = indestr ? 0 : hits;
}


kulicka::kulicka() : x(0), y(0), dx(0), dy(0), r(8) {}

kulicka::kulicka(float x, float y, float dx, float dy, int r) {
	this->x  = x;
	this->y  = y;
	this->dx = dx;
	this->dy = dy;
	this->r  = r;
}

void kulicka::update() {
	x += dx;
	y += dy;
}
