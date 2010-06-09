#ifndef APPSTATE_H
#define APPSTATE_H

class AppState {
public:
	AppState() {};
	virtual ~AppState() {};

	virtual void tick(float t, float dt) {};
	virtual void display(float t, float dt) {};

	virtual void mousemove(int xrel, int yrel) = 0;
	virtual void mouseclick(int x, int y, bool down) = 0;
	virtual void keypressed(int key, bool down) = 0;
};


#endif

