#include "RenderWidget.hpp"
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>

#include "../wowmapview.h"
#include "../video.h"
#include "../menu.h"

RenderWidget::RenderWidget(QWidget* parent) :
	QGLWidget(parent)
{
	setFocusPolicy(Qt::StrongFocus);
	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	timer->start();
}

RenderWidget::~RenderWidget()
{
}

bool RenderWidget::event(QEvent* e)
{
	AppState *as = 0;
	if (!gStates.empty())
		as = gStates[gStates.size() - 1];

	if (as)
	{
		switch (e->type())
		{
		case QEvent::MouseMove:
		{
			QMouseEvent* me = (QMouseEvent*) e;
			as->mousemove(x - me->x(), y - me->y());
			x = me->x();
			y = me->y();
			return true;
		}
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		{
			QMouseEvent* me = (QMouseEvent*) e;
			as->mouseclick(me->x(), me->y(), e->type() == QEvent::MouseButtonPress);
			x = me->x();
			y = me->y();
			return true;
		}
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
		{
			QKeyEvent* ke = (QKeyEvent*) e;
			as->keypressed(ke->key(), e->type() == QEvent::KeyPress);
			return true;
		}
		}

		if (gPop)
		{
			gPop = false;
			gStates.pop_back();
			delete as;
		}

		if (gStates.empty())
		{
			close();
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
	video.init(width(), height());
	gStates.push_back(new Menu());
	gPop = false;
}

void RenderWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, (GLint)width, (GLint)height);
	video.xres = width;
	video.yres = height;
}

void RenderWidget::paintGL()
{
	if (gStates.empty())
		return;

	int t = qtime.elapsed();
	int dt = t - last_t;
	last_t = t;
	time += dt;
	float ftime = time / 1000.f;

	AppState* as = gStates[gStates.size() - 1];
	as->tick(ftime, dt / 1000.f);
	as->display(ftime, dt / 1000.f);
}
