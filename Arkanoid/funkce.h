struct vector;

struct bod {
	float x,y;

	bod();
	bod(float x, float y);
	void nova_pozice(vector v);
	int vrat_x();
	int vrat_y();
};

struct vector {
	float x,y;

	vector();
	vector(float x, float y);
	void normalizace();
};

struct palka {
	int x,y,delka,rychlost;

	palka();
	palka(int x, int y, int delka, int rychlost);
	void plus_x();
	void minus_x();
};

struct cihla {
	int x,y;
	int hits;            // zbývající počet zásahů (0 = zničená)
	int max_hits;        // maximální počet zásahů (pro výpočet barvy)
	bool indestructible; // nezničitelná cihla (tmavě šedá)

	cihla();
	cihla(int x, int y, int hits, bool indestructible = false);
	bool active() const { return indestructible || hits > 0; }
};

struct kulicka {
	float x, y;
	float dx, dy;
	int r;

	kulicka();
	kulicka(float x, float y, float dx, float dy, int r);
	void update();
};
