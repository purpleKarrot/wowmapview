#ifndef QTCANVAS_HPP
#define QTCANVAS_HPP

#include <QGLViewer/qglviewer.h>

class ModelCanvas;

class qtcanvas: public QGLViewer
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
	void draw();
	void init();

private:
	ModelCanvas* old;
};

#endif /* QTCANVAS_HPP */
