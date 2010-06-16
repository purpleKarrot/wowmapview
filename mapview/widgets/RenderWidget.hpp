#ifndef RENDERWIDGET_HPP
#define RENDERWIDGET_HPP

#include <QGLWidget>
#include <QTime>
#include "../world.h"
#include <boost/scoped_ptr.hpp>

class RenderWidget: public QGLWidget
{
public:
	RenderWidget(QWidget* parent = 0);

	~RenderWidget();

	void enter(const MapEntry& entry);

private:
	void paintGL();
	void resizeGL(int width, int height);
	void initializeGL();
	bool event(QEvent* e);

private:
	void keypressed(int key, bool down);
	void mousemove(int xrel, int yrel);
	void mouseclick(int x, int y, bool down);

	void tick(float t, float dt);
	void display(float t, float dt);
	void drawMinimap();

private:
	QTime qtime;

	int last_t, time;
	int x, y;

	float ah, av, moving, strafing, updown, mousedir, movespd;
	bool look;
};

#endif /* RENDERWIDGET_HPP */
