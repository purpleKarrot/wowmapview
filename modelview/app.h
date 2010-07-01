#ifndef APP_H
#define APP_H

#include <QObject>
#include <wx/app.h>
#include "modelviewer.h"

struct QtDummy: QObject
{
Q_OBJECT

private slots:
	void shutdown();
};

class WowModelViewApp: public wxApp
{
	bool OnInit();
	int MainLoop();

	ModelViewer *frame;
};

#endif
