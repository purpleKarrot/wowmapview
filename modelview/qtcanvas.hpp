#ifndef QTCANVAS_HPP
#define QTCANVAS_HPP

#include <QGLWidget>

class ModelCanvas;

class qtcanvas: public QGLWidget
{
public:
	qtcanvas(ModelCanvas* old) :
		old(old)
	{
		show();
	}

	~qtcanvas()
	{
	}

private:
	void paintGL();

private:
	ModelCanvas* old;
};

#endif /* QTCANVAS_HPP */
