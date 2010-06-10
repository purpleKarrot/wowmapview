#ifndef APPSTATE_H
#define APPSTATE_H

#include <string>

struct MapEntry
{
	int id;
	std::string name, description;
};

class AppState
{
public:
	AppState()
	{
	}

	virtual ~AppState()
	{
	}

	virtual void tick(float t, float dt)
	{
	}

	virtual void display(float t, float dt)
	{
	}

	virtual void mousemove(int xrel, int yrel)
	{
	}

	virtual void mouseclick(int x, int y, bool down)
	{
	}

	virtual void keypressed(int key, bool down)
	{
	}

	virtual void enter(const MapEntry& entry)
	{
	}
};

#endif
