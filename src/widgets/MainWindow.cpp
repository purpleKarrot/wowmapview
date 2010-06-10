#include "MainWindow.hpp"

#include <QDockWidget>
#include <QMessageBox>
#include <QMenuBar>
#include <QStatusBar>

#include "RenderWidget.hpp"
#include "FileList.hpp"
#include <cassert>

static QStatusBar* status_bar = 0;
void status_message(std::string const& message)
{
	assert(status_bar);
	status_bar->showMessage(QString::fromStdString(message));
}

MainWindow::MainWindow()
{
	setCentralWidget(new RenderWidget());

	QDockWidget* dock = new QDockWidget("File List", this);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	dock->setWidget(new FileList());
	addDockWidget(Qt::LeftDockWidgetArea, dock);

	QMenu* help = menuBar()->addMenu("&Help");

	QAction* action = new QAction("&Controls", this);
	connect(action, SIGNAL(triggered()), this, SLOT(controls()));
	help->addAction(action);

	action = new QAction("&About", this);
	connect(action, SIGNAL(triggered()), this, SLOT(about()));
	help->addAction(action);

	status_bar = statusBar();
}

MainWindow::~MainWindow()
{
}

void MainWindow::about()
{
	QMessageBox::about(this, "wowmapview", //
		"Original program has been reverse engineered and written by ufoz\n\n"
		"World of Warcraft is (C) Blizzard Entertainment");
}

void MainWindow::controls()
{
	QMessageBox::about(this, "Controls", //
		"F1 - toggle models\n"
		"F2 - toggle doodads\n"
		"F3 - toggle terrain\n"
		"F4 - toggle stats\n"
		"F5 - save bookmark\n"
		"F6 - toggle map objects\n"
		"H - disable highres terrain\n"
		"I - toggle invert mouse\n"
		"M - minimap\n"
		"Esc - back/exit\n"
		"WASD - move\n"
		"R - quick 180 degree turn\n"
		"F - toggle fog\n"
		"+,- - adjust fog distance\n"
		"O,P - slower/faster movement\n"
		"B,N - slower/faster time\n");
}
