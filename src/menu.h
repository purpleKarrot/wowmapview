#ifndef MENU_H
#define MENU_H

#include "appstate.h"
#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>

#include "world.h"

enum Commands
{
	CMD_SELECT,
	CMD_LOAD_WORLD,
	CMD_DO_LOAD_WORLD,
	CMD_BACK_TO_MENU,
	CMD_SELECT_MINIMAP
};

class Menu: public AppState
{
	int cmd, mx, my;

	World* world;

	void enter(const MapEntry& entry);

public:
	Menu();
	~Menu();

	void tick(float t, float dt);
	void display(float t, float dt);

	void mouseclick(int x, int y, bool down);

	void shprint(Font *f, int x, int y, char *text);
};

#endif
