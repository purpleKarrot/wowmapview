#include "test.h"
#include "wowmapview.h"
#include "areadb.h"
#include "shaders.h"

#include <cmath>
#include <string>
#include <Qt>

static int gV = 0;

#define XSENS 4.0f
#define YSENS 8.0f
#define SPEED 66.6f

// why the hell is this class called Test, anyway
// I should rename it to MapViewer or something when I'm not lazy

Test::Test(World *w, float ah0, float av0): world(w), ah(ah0), av(av0)
{
	moving = strafing = updown = 0;

	mousedir = -1.0f;

	movespd = SPEED;

	look = false;
	mapmode = false;
	hud = false;

	world->thirdperson = false;
	world->lighting = true;
	world->drawmodels = true;
	world->drawdoodads = true;
	world->drawterrain = true;
	world->drawwmo = true;
	world->drawhighres = true;
	world->drawfog = true; // should this be on or off by default..? :(

	// in the wow client, fog distance is stored in wtf\config.wtf as "farclip"
	// minimum is 357, maximum is 777
	world->fogdistance = 512.0f;

	world->l_const = 0.0f;
	world->l_linear = 0.7f;
	world->l_quadratic = 0.03f;
}

Test::~Test()
{
	delete world;
}

void Test::tick(float t, float dt)
{
	Vec3D dir(1,0,0);
	rotate(0,0, &dir.x,&dir.y, av*PI/180.0f);
    rotate(0,0, &dir.x,&dir.z, ah*PI/180.0f);

	if (moving != 0) world->camera += dir * dt * movespd * moving;
	if (strafing != 0) {
		Vec3D right = dir % Vec3D(0,1,0);
		right.normalize();
		world->camera += right * dt * movespd * strafing;
	}
	if (updown != 0) world->camera += Vec3D(0, dt * movespd * updown, 0);
	world->lookat = world->camera + dir;

	world->time += (gV * /*360.0f*/ 90.0f * dt);
	world->animtime += dt * 1000.0f;
	globalTime = (int)world->animtime;

	world->tick(dt);
}

void Test::display(float t, float dt)
{
	if (mapmode && world->minimap) {
		// show map
        // TODO: try to use a real map from WoW? either the large map or the minimap would be nice
		video.clearScreen();
		video.set2D();

		const int len = 768;
		const int basex = 200;
		const int basey = 0;
		glColor4f(1,1,1,1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, world->minimap);
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2i(basex,basey);
		glTexCoord2f(1,0);
		glVertex2i(basex+len,basey);
		glTexCoord2f(1,1);
		glVertex2i(basex+len,basey+len);
		glTexCoord2f(0,1);
		glVertex2i(basex,basey+len);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glBegin(GL_LINES);
		float fx, fz;
		fx = basex + world->camera.x / TILESIZE * 12.0f;
		fz = basey + world->camera.z / TILESIZE * 12.0f;
		glVertex2f(fx, fz);
		glColor4f(1,1,1,0);
		glVertex2f(fx + 10.0f*cosf(ah/180.0f*PI), fz + 10.0f*sinf(ah/180.0f*PI));
		glEnd();
	} else {
        // draw 3D view
		video.set3D();
		world->draw();
		
		video.set2D();
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glColor4f(1,1,1,1);

		if (hud) {
			//char *sn = world->skies->getSkyName();
			//if (sn)	f16->print(5,60,"%s", sn);

			// TODO: look up WMO names/group names as well from some client db?
			unsigned int areaID = world->getAreaID();
			unsigned int regionID = 0;
			/// Look up area
			try {
				AreaDB::Record rec = gAreaDB.getByID(areaID);
				std::string areaName = rec.getString(AreaDB::Name);
				regionID = rec.getUInt(AreaDB::Region);
				f16->print(5,20,"%s", areaName.c_str());
			} catch(AreaDB::NotFound)
			{
				/// Not found, unknown area
				//f16->print(5,20,"Unknown [%i]", areaID);
			}
			if (regionID != 0) {
				/// Look up region
				try {
					std::string regionName = gAreaDB.getAreaName(regionID);
					f16->print(5,40,"%s", regionName.c_str());
				} catch(AreaDB::NotFound)
				{
					//f16->print(5,40,"Unknown [%i]", regionID);
				}
			}

			int time = ((int)world->time)%2880;
			int hh,mm;

            hh = time / 120;
			mm = (time % 120) / 2;
			
			//f16->print(5, 60, "%02d:%02d", hh,mm);
			f16->print(video.xres - 50, 0, "%02d:%02d", hh,mm);

			f16->print(5, video.yres-22, "(%.0f, %.0f, %.0f)", 
				-(world->camera.x - ZEROPOINT), 
				-(world->camera.z - ZEROPOINT),
				world->camera.y);

		}

		if (world->loading) {
			const char* loadstr = "Loading...";
			const char* oobstr = "Out of bounds";

			f16->print(video.xres/2 - f16->textwidth(loadstr)/2, /*video.yres/2-8*/ 0, world->oob?oobstr:loadstr);
		}

		/*
		f16->print(5,20,"C: %.1f", world->l_const);
		f16->print(5,40,"L: %.2f", world->l_linear);
		f16->print(5,60,"Q: %.3f", world->l_quadratic);
		*/
	}

};

void Test::keypressed(int key, bool down)
{
	if (down)
	{
		switch (key)
		{
		case Qt::Key_Escape:
			gPop = true;
			break;
		case Qt::Key_W:
			moving = 1.0f;
			break;
		case Qt::Key_S:
			moving = -1.0f;
			break;
		case Qt::Key_A:
			strafing = -1.0f;
			break;
		case Qt::Key_D:
			strafing = 1.0f;
			break;
		case Qt::Key_E:
			updown = -1.0f;
			break;
		case Qt::Key_Q:
			updown = 1.0f;
			break;
		case Qt::Key_I:
			mousedir *= -1.0f;
			break;
		case Qt::Key_P:
			movespd *= 2.0f;
			break;
		case Qt::Key_O:
			movespd *= 0.5f;
			break;
		case Qt::Key_R:
			ah += 180.0f;
			break;
		case Qt::Key_N:
			gV++;
			break;
		case Qt::Key_B:
			gV--;
			if (gV < 0)
				gV = 0;
			break;
		case Qt::Key_T:
			world->thirdperson = !world->thirdperson;
			break;
		case Qt::Key_L:
			world->lighting = !world->lighting;
			break;
		case Qt::Key_F1:
			world->drawmodels = !world->drawmodels;
			break;
		case Qt::Key_F2:
			world->drawdoodads = !world->drawdoodads;
			break;
		case Qt::Key_F3:
			world->drawterrain = !world->drawterrain;
			break;
		case Qt::Key_F4:
			hud = !hud;
			break;
		case Qt::Key_F6:
			world->drawwmo = !world->drawwmo;
			break;
		case Qt::Key_F7:
			world->useshaders = !world->useshaders;
			break;
		case Qt::Key_F8:
			initShaders();
			break;
		case Qt::Key_H:
			world->drawhighres = !world->drawhighres;
			break;
		case Qt::Key_F:
			world->drawfog = !world->drawfog;
			break;
		case Qt::Key_Plus:
			world->fogdistance += 60.0f;
			break;
		case Qt::Key_Minus:
			world->fogdistance -= 60.0f;
			break;
		case Qt::Key_M:
			mapmode = !mapmode;
		}
	}
	else
	{
		switch (key)
		{
		case Qt::Key_W:
			if (moving > 0)
				moving = 0;
			break;
		case Qt::Key_S:
			if (moving < 0)
				moving = 0;
			break;
		case Qt::Key_D:
			if (strafing > 0)
				strafing = 0;
			break;
		case Qt::Key_A:
			if (strafing < 0)
				strafing = 0;
			break;
		case Qt::Key_Q:
			if (updown > 0)
				updown = 0;
			break;
		case Qt::Key_E:
			if (updown < 0)
				updown = 0;
		}
	}
}

void Test::mousemove(int xrel, int yrel)
{
	if (look) {
		ah += xrel / XSENS;
		av += mousedir * yrel / YSENS;
		if (av < -80) av = -80;
		else if (av > 80) av = 80;
	}

}

void Test::mouseclick(int x, int y, bool down)
{
	look = down;
}
