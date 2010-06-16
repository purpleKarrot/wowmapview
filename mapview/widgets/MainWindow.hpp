#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
	MainWindow();
	~MainWindow();

private slots:
	void about();
	void controls();
};

#endif /* MAINWINDOW_HPP */
