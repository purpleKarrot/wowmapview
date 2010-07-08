
#include <QApplication>
#include <iostream>

#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "mpq.hpp"
#include "appstate.h"

#include "areadb.h"

#include "widgets/MainWindow.hpp"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	srand((unsigned int)time(0));

	OpenDBs();

	MainWindow view;
	view.resize(1024, 768);
	view.show();

	return app.exec();
}

float frand()
{
    return rand()/(float)RAND_MAX;
}

float randfloat(float lower, float upper)
{
	return lower + (upper-lower)*(rand()/(float)RAND_MAX);
}

int randint(int lower, int upper)
{
    return lower + (int)((upper+1-lower)*frand());
}
