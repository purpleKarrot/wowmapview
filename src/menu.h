#ifndef MENU_H
#define MENU_H

#include "appstate.h"
#include <string>
#include <vector>

#include "world.h"

struct Clickable {
	int x0, y0, x1, y1;

	bool hit(int x, int y);

};

struct MapEntry: public Clickable {
	int id;
	std::string name, description;
	Font *font;
};

struct Bookmark: public Clickable {
	std::string basename, name, label;
	int mapid;
	Vec3D pos;
	float ah,av;
};

enum Commands {
	CMD_SELECT,
	CMD_LOAD_WORLD,
	CMD_DO_LOAD_WORLD,
	CMD_BACK_TO_MENU,
	CMD_SELECT_MINIMAP
};

class Menu :public AppState
{

	int sel,cmd,mx,my,cz,cx;
	int last_key_x, last_key_y;
	bool last_key_down;

	World *world;

	std::vector<MapEntry> maps;

	bool setpos;
	float ah,av;

	Model *bg;
	float mt;

	int lastbg;
	bool darken;

public:
	Menu();
	~Menu();

	void tick(float t, float dt);
	void display(float t, float dt);

	void keypressed(int key, bool down);
	void mousemove(int xrel, int yrel);
	void mouseclick(int x, int y, bool down);

	void randBackground();

	void shprint(Font *f, int x, int y, char *text);

};


#endif
