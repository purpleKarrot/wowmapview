#include <GL/glew.h>
#include "qtcanvas.hpp"
#include "modelcanvas.h"
#include "widgets/MainWindow.hpp"

class Camera: public qglviewer::Camera
{
	float zNear() const
	{
		return 0.1f;
	}

	float zFar() const
	{
		return 10000.f;
	}
};

qtcanvas::qtcanvas(ModelCanvas* old) :
	old(old)
{
	qglviewer::Camera* oldcam = camera();
	setCamera(new Camera);
	delete oldcam;

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
