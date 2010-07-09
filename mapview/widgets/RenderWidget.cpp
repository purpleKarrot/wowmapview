#include "RenderWidget.hpp"
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <sstream>

#include "../wowmapview.h"
#include "../menu.h"
#include "shaders.h"
#include "../areadb.h"

#define XSENS 4.0f
#define YSENS 8.0f
#define SPEED 66.6f

static int gV = 0;
static char buffer[1024];
static bool mapmode = true;

RenderWidget::RenderWidget(QWidget* parent) :
	QGLWidget(parent)
{
	setFocusPolicy(Qt::StrongFocus);
	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	timer->start();

	ah = -90.f;
	av = -30.f;

	moving = strafing = updown = 0;

	mousedir = -1.0f;

	movespd = SPEED;

	look = false;
}

RenderWidget::~RenderWidget()
{
}

bool RenderWidget::event(QEvent* e)
{
	switch (e->type())
	{
	case QEvent::MouseMove:
	{
		QMouseEvent* me = (QMouseEvent*) e;
		mousemove(x - me->x(), y - me->y());
		x = me->x();
		y = me->y();
		return true;
	}
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	{
		QMouseEvent* me = (QMouseEvent*) e;
		mouseclick(me->x(), me->y(), e->type() == QEvent::MouseButtonPress);
		x = me->x();
		y = me->y();
		return true;
	}
	case QEvent::KeyPress:
	case QEvent::KeyRelease:
	{
		QKeyEvent* ke = (QKeyEvent*) e;
		keypressed(ke->key(), e->type() == QEvent::KeyPress);
		return true;
	}
	}

	return QGLWidget::event(e);
}

void RenderWidget::initializeGL()
{
	qtime.start();
	last_t = qtime.elapsed();
	time = 0;

	initShaders();
	//	video.xres = width();
	//	video.yres = height();

	glViewport(0, 0, width(), height());
	glMatrixMode( GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, GLfloat(width()) / GLfloat(height()), 1.0f, 1024.0f);

	// hmmm...
	glEnableClientState( GL_VERTEX_ARRAY);
	glEnableClientState( GL_NORMAL_ARRAY);
	glEnableClientState( GL_TEXTURE_COORD_ARRAY);
}

void RenderWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, (GLint) width, (GLint) height);
	//	video.xres = width;
	//	video.yres = height;
}

void RenderWidget::paintGL()
{
	int t = qtime.elapsed();
	int dt = t - last_t;
	last_t = t;
	time += dt;
	float ftime = time / 1000.f;

	tick(ftime, dt / 1000.f);

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	display(ftime, dt / 1000.f);
}

void enterWorld(const MapEntry& entry)
{
	std::cout << "Entering " << entry.name << std::endl;
	gWorld.reset(new World(entry.name.c_str(), entry.id));
	gWorld->initDisplay();
	gWorld->thirdperson = false;
	gWorld->lighting = true;
	gWorld->drawmodels = true;
	gWorld->drawdoodads = true;
	gWorld->drawterrain = true;
	gWorld->drawwmo = true;
	gWorld->drawhighres = true;
	gWorld->drawfog = true; // should this be on or off by default..? :(

	// in the wow client, fog distance is stored in wtf\config.wtf as "farclip"
	// minimum is 357, maximum is 777
	gWorld->fogdistance = 512.0f;

	gWorld->l_const = 0.0f;
	gWorld->l_linear = 0.7f;
	gWorld->l_quadratic = 0.03f;
	mapmode = true;
}

void RenderWidget::keypressed(int key, bool down)
{
	if (!gWorld)
		return;

	if (down)
	{
		switch (key)
		{
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
			gWorld->thirdperson = !gWorld->thirdperson;
			break;
		case Qt::Key_L:
			gWorld->lighting = !gWorld->lighting;
			break;
		case Qt::Key_F1:
			gWorld->drawmodels = !gWorld->drawmodels;
			break;
		case Qt::Key_F2:
			gWorld->drawdoodads = !gWorld->drawdoodads;
			break;
		case Qt::Key_F3:
			gWorld->drawterrain = !gWorld->drawterrain;
			break;
		case Qt::Key_F6:
			gWorld->drawwmo = !gWorld->drawwmo;
			break;
		case Qt::Key_F7:
			gWorld->useshaders = !gWorld->useshaders;
			break;
		case Qt::Key_F8:
			initShaders();
			break;
		case Qt::Key_H:
			gWorld->drawhighres = !gWorld->drawhighres;
			break;
		case Qt::Key_F:
			gWorld->drawfog = !gWorld->drawfog;
			break;
		case Qt::Key_Plus:
			gWorld->fogdistance += 60.0f;
			break;
		case Qt::Key_Minus:
			gWorld->fogdistance -= 60.0f;
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

void RenderWidget::mousemove(int xrel, int yrel)
{
	if (look)
	{
		ah += xrel / XSENS;
		av += mousedir * yrel / YSENS;
		if (av < -80)
			av = -80;
		else if (av > 80)
			av = 80;
	}
}

void RenderWidget::mouseclick(int x, int y, bool down)
{
	if (!mapmode)
	{
		look = down;
		return;
	}

	if (!gWorld)
		return;

	int mx = x - 200;
	int my = y;

	//	world->initDisplay();

	int cz = 0;
	int cx = 0;

	if (gWorld->nMaps > 0)
	{
		float fx = (mx / 12.0f);
		float fz = (my / 12.0f);

		cx = (int) fx;
		cz = (int) fz;

		gWorld->camera = Vec3D(fx * TILESIZE, 0, fz * TILESIZE);
		gWorld->autoheight = true;
	}
	else
	{
		Vec3D p;
		if (gWorld->gwmois.size() >= 1)
			p = gWorld->gwmois[0].pos;
		else
			p = Vec3D(0, 0, 0); // empty map? :|

		cx = (int) (p.x / TILESIZE);
		cz = (int) (p.z / TILESIZE);

		gWorld->camera = p + Vec3D(0, 25.0f, 0);
	}

	gWorld->lookat = gWorld->camera + Vec3D(0, 0, -1.0f);
	gWorld->enterTile(cx, cz);
	mapmode = false;
}

void RenderWidget::tick(float t, float dt)
{
	if (!gWorld)
		return;

	Vec3D dir(1, 0, 0);
	rotate(0, 0, &dir.x, &dir.y, av * PI / 180.0f);
	rotate(0, 0, &dir.x, &dir.z, ah * PI / 180.0f);

	if (moving != 0)
		gWorld->camera += dir * dt * movespd * moving;
	if (strafing != 0)
	{
		Vec3D right = dir % Vec3D(0, 1, 0);
		right.normalize();
		gWorld->camera += right * dt * movespd * strafing;
	}
	if (updown != 0)
		gWorld->camera += Vec3D(0, dt * movespd * updown, 0);
	gWorld->lookat = gWorld->camera + dir;

	gWorld->time += (gV * /*360.0f*/90.0f * dt);
	gWorld->animtime += dt * 1000.0f;
	globalTime = (int) gWorld->animtime;

	gWorld->tick(dt);
}

void RenderWidget::drawMinimap()
{
	glDisable( GL_FOG);

	glMatrixMode( GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width(), height(), 0, -1.0, 1.0);
	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity();

	glEnable( GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable( GL_DEPTH_TEST);
	glDisable( GL_CULL_FACE);
	glDisable( GL_LIGHTING);

	glColor4f(1, 1, 1, 1);

	glEnable( GL_TEXTURE_2D);

	int basex = 200;
	int basey = 0;
	int tilesize = 12;

	if (gWorld->minimap)
	{
		// minimap time! ^_^
		const int len = 768;
		glColor4f(1, 1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, gWorld->minimap);
		glBegin( GL_QUADS);
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
			if (gWorld->maps[j][i])
			{
				glColor4f(0.7f, 0.9f, 0.8f, 0.2f);
				glBegin( GL_QUADS);
				glVertex2i(basex + i * tilesize, basey + j * tilesize);
				glVertex2i(basex + (i + 1) * tilesize, basey + j * tilesize);
				glVertex2i(basex + (i + 1) * tilesize, basey + (j + 1)
					* tilesize);
				glVertex2i(basex + i * tilesize, basey + (j + 1) * tilesize);
				glEnd();
			}
		}
	}

	glBegin( GL_LINES);
	float fx, fz;
	fx = basex + gWorld->camera.x / TILESIZE * 12.0f;
	fz = basey + gWorld->camera.z / TILESIZE * 12.0f;
	glVertex2f(fx, fz);
	glColor4f(1, 1, 1, 0);
	glVertex2f(fx + 10.0f * cosf(ah / 180.0f * PI), fz + 10.0f * sinf(ah
		/ 180.0f * PI));
	glEnd();

	glEnable(GL_TEXTURE_2D);
}

void RenderWidget::display(float t, float dt)
{
	if (!gWorld)
		return;

	if (mapmode)
	{
		drawMinimap();
		return;
	}

	glMatrixMode( GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, GLfloat(width()) / GLfloat(height()), 1.0f, 1024.0f);
	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity();

	gWorld->draw();

	std::stringstream status;

	status << getAreaName(gAreaDB, gWorld->getAreaID()) << "; ";

	int time = ((int) gWorld->time) % 2880;
	int hh, mm;

	hh = time / 120;
	mm = (time % 120) / 2;

	sprintf(buffer, "%02d:%02d", hh, mm);
	status << buffer << "; ";

	sprintf(buffer, "(%.0f, %.0f, %.0f)", -(gWorld->camera.x - ZEROPOINT),
		-(gWorld->camera.z - ZEROPOINT), gWorld->camera.y);
	status << buffer;

	status_message(status.str());

	if (gWorld->loading)
		status_message(gWorld->oob ? "Out of bounds" : "Loading...");
}
