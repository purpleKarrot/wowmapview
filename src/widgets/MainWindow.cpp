#include "MainWindow.hpp"

#include <QDockWidget>

#include "RenderWidget.hpp"
#include "FileList.hpp"

MainWindow::MainWindow()
{
	setCentralWidget(new RenderWidget());

	QDockWidget* dock = new QDockWidget("File List", this);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	dock->setWidget(new FileList());
	addDockWidget(Qt::LeftDockWidgetArea, dock);
}

MainWindow::~MainWindow()
{
}
