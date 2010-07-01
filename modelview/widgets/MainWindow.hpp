#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

class MainWindow: public QMainWindow
{
//Q_OBJECT

public:
	MainWindow();
	~MainWindow();

	static void add_dock(const char* name, Qt::DockWidgetArea area,
		QWidget *widget);

	static void add_central(QWidget *widget);

private:
	static MainWindow* instance;
};

#endif /* MAINWINDOW_HPP */
