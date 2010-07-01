#include <GL/glew.h>
#include "qtcanvas.hpp"
#include "modelcanvas.h"
#include "widgets/MainWindow.hpp"

qtcanvas::qtcanvas(ModelCanvas* old) :
	old(old)
{
	MainWindow::add_central(this);
}

void qtcanvas::init()
{
	glewInit();
}

void qtcanvas::draw()
{
	if (old->wmo)
	{
		old->RenderWMO();
	}
	else if (old->model)
	{
		glPushMatrix();

		if (old->useCamera && old->model->hasCamera)
		{
			old->model->cam.setup();
		}
		else
		{
			float scale = 2.f / old->model->rad;
			glScalef(scale, scale, scale);
			glRotatef(-90.f, 0.f, 1.f, 0.f);
			glTranslatef(0.f, -1.f, 0.f);
		}

		old->RenderObjects();

		glPopMatrix();
	}
	else if (old->adt)
	{
		old->RenderADT();
	}
}
