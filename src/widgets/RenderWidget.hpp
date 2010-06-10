#ifndef RENDERWIDGET_HPP
#define RENDERWIDGET_HPP

#include <QGLWidget>
#include <QTime>

class RenderWidget: public QGLWidget
{
public:
	RenderWidget(QWidget* parent = 0);

	~RenderWidget();

private:
	void paintGL();
	void resizeGL(int width, int height);
	void initializeGL();
	bool event(QEvent* e);

private:
	QTime qtime;

	int last_t, time;
	int x, y;
};

#endif /* RENDERWIDGET_HPP */
