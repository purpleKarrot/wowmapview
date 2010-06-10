#ifndef TEST_H
#define TEST_H

#include "appstate.h"
#include "video.h"
#include "world.h"

class Test :public AppState
{
public:
	Test(World *w, float ah0 = -90.0f, float av0 = -30.0f);
	~Test();

	void tick(float t, float dt);
	void display(float t, float dt);

private:
	void keypressed(int key, bool down);
	void mousemove(int xrel, int yrel);
	void mouseclick(int x, int y, bool down);

private:
	float ah,av,moving,strafing,updown,mousedir,movespd;
	bool look;
	bool mapmode;

	World* world;
};

#endif
