#include "MainWindow.hpp"
#include <QDockWidget>

MainWindow* MainWindow::instance;

MainWindow::MainWindow()
{
	instance = this;
}

MainWindow::~MainWindow()
{
}

void MainWindow::add_dock(const char* name, Qt::DockWidgetArea area,
	QWidget *widget)
{
	QDockWidget *dock = new QDockWidget(name, instance);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);

	//	QAction* action = new QAction(name, this);
	//	connect(action, SIGNAL(triggered()), dock, SLOT(show()));
	//	dock_menu->addAction(action);

	dock->setWidget(widget);
	instance->addDockWidget(area, dock);
}

void MainWindow::add_central(QWidget* widget)
{
	instance->setCentralWidget(widget);
}
