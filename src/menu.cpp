#include "menu.h"
#include "mpq.h"
#include "test.h"
#include "dbcfile.h"

#include <Qt>
#include <fstream>

Menu::Menu()
{
	cmd = 0;
	world = 0;
}

Menu::~Menu()
{
}

void Menu::tick(float t, float dt)
{
	if (cmd == CMD_DO_LOAD_WORLD)
	{
		gWorld = world;

		world->initDisplay();
		// calc coordinates

		int cz = 0;
		int cx = 0;

		if (world->nMaps > 0)
		{

			float fx = (mx / 12.0f);
			float fz = (my / 12.0f);

			cx = (int) fx;
			cz = (int) fz;

			world->camera = Vec3D(fx * TILESIZE, 0, fz * TILESIZE);
			world->autoheight = true;

		}
		else
		{
			Vec3D p;
			if (world->gwmois.size() >= 1)
				p = world->gwmois[0].pos;
			else
				p = Vec3D(0, 0, 0); // empty map? :|

			cx = (int) (p.x / TILESIZE);
			cz = (int) (p.z / TILESIZE);

			world->camera = p + Vec3D(0, 25.0f, 0);
		}
		world->lookat = world->camera + Vec3D(0, 0, -1.0f);

		world->enterTile(cx, cz);

		gStates.push_back(new Test(world));

		world = 0;

		cmd = CMD_BACK_TO_MENU;
	}
	else if (cmd == CMD_BACK_TO_MENU)
	{
		cmd = CMD_SELECT;
		gWorld = 0;
	}
}

void Menu::display(float t, float dt)
{
	video.clearScreen();
	glDisable(GL_FOG);

	video.set2D();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	glColor4f(1, 1, 1, 1);

	glEnable(GL_TEXTURE_2D);

	int basex = 200;
	int basey = 0;
	int tilesize = 12;

	if (cmd == CMD_LOAD_WORLD)
	{
		status_message("Loading...");
		cmd = CMD_DO_LOAD_WORLD;
		return;
	}

	if (cmd != CMD_SELECT_MINIMAP || !world)
		return;

	if (world->minimap)
	{
		// minimap time! ^_^
		const int len = 768;
		glColor4f(1, 1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, world->minimap);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2i(basex, basey);
		glTexCoord2f(1, 0);
		glVertex2i(basex + len, basey);
		glTexCoord2f(1, 1);
		glVertex2i(basex + len, basey + len);
		glTexCoord2f(0, 1);
		glVertex2i(basex, basey + len);
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
	for (int j = 0; j < 64; j++)
	{
		for (int i = 0; i < 64; i++)
		{
			if (world->maps[j][i])
			{
				glColor4f(0.7f, 0.9f, 0.8f, 0.2f);
				glBegin(GL_QUADS);
				glVertex2i(basex + i * tilesize, basey + j * tilesize);
				glVertex2i(basex + (i + 1) * tilesize, basey + j * tilesize);
				glVertex2i(basex + (i + 1) * tilesize, basey + (j + 1)
					* tilesize);
				glVertex2i(basex + i * tilesize, basey + (j + 1) * tilesize);
				glEnd();
			}
		}
	}
	glEnable(GL_TEXTURE_2D);

	glColor4f(1, 1, 1, 1);
	if (world->nMaps == 0)
	{
		status_message("Click to enter");
	}
	else
	{
		status_message("Select your starting point");
	}
}

void Menu::enter(const MapEntry& entry)
{
	if (cmd != CMD_SELECT)
		return;

	std::cout << "Entering " << entry.name << std::endl;

	if (world)
		delete world;

	world = new World(entry.name.c_str(), entry.id);
	cmd = CMD_SELECT_MINIMAP;
}

void Menu::mouseclick(int x, int y, bool down)
{
	if (cmd != CMD_SELECT_MINIMAP)
		return;

	if (world != 0)
	{
		mx = x - 200;
		my = y;
		cmd = CMD_LOAD_WORLD;
	}
	else
	{
		cmd = CMD_SELECT;
	}
}
