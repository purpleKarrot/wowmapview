
#include <QApplication>
//#include <QGLWidget>

#include <ctime>
#include <cstdlib>

#include "mpq.h"
#include "video.h"
#include "appstate.h"

#include "test.h"
#include "menu.h"
#include "areadb.h"

#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QTime>

class View: public QGLWidget
{
public:
	View(QWidget* parent = 0) :
		QGLWidget(parent)
	{
		QTimer* timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
		timer->start();
	}

private:
	void paintGL();
	//	void resizeGL(int width, int height);
	void initializeGL();

	bool event(QEvent* e);

private:
	QTime qtime;

	int last_t, time;
	int x, y;
};

int fullscreen = 0;


std::vector<AppState*> gStates;
bool gPop = false;

static std::string gamepath;


float gFPS;

GLuint ftex;
Font *f16, *f24, *f32;


void initFonts()
{
	ftex = loadTGA("arial.tga",false);

	f16 = new Font(ftex, 256, 256, 16, "arial.info");
	f24 = new Font(ftex, 256, 256, 24, "arial.info");
	f32 = new Font(ftex, 256, 256, 32, "arial.info");
}

void deleteFonts()
{
	glDeleteTextures(1, &ftex);

	delete f16;
	delete f24;
	delete f32;
}


void getGamePath()
{
#ifdef _WIN32
	HKEY key;
	DWORD t,s;
	LONG l;
	s = 1024;
	memset(gamepath,0,s);
	l = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Blizzard Entertainment\\World of Warcraft\\Beta",0,KEY_QUERY_VALUE,&key);
	if (l != ERROR_SUCCESS)
		l = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Blizzard Entertainment\\World of Warcraft\\PTR",0,KEY_QUERY_VALUE,&key);
	if (l != ERROR_SUCCESS)
		l = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Blizzard Entertainment\\World of Warcraft",0,KEY_QUERY_VALUE,&key);
	if (l == ERROR_SUCCESS) {
		l = RegQueryValueEx(key,"InstallPath",0,&t,(LPBYTE)gamepath,&s);
		RegCloseKey(key);
		strcat(gamepath,"Data\\");
	}
#else
	gamepath = "data/";

	const char* wow_path = getenv("WOW_PATH");
	if (wow_path)
	{
		gamepath = wow_path;
		gamepath += "Data/";
	}
#endif
}

int file_exists(const std::string& path)
{
	FILE *f = fopen(path.c_str(), "r");
	if (f) {
		fclose(f);
		return true;
	}
	return false;
}

bool View::event(QEvent* e)
{
	AppState *as = 0;
	if (!gStates.empty())
		as = gStates[gStates.size() - 1];

	if (as)
	{
		switch (e->type())
		{
		case QEvent::MouseMove:
		{
			QMouseEvent* me = (QMouseEvent*) e;
			as->mousemove(x - me->x(), y - me->y());
			x = me->x();
			y = me->y();
			return true;
		}
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		{
			QMouseEvent* me = (QMouseEvent*) e;
			as->mouseclick(me->x(), me->y(), e->type() == QEvent::MouseButtonPress);
			x = me->x();
			y = me->y();
			return true;
		}
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
		{
			QKeyEvent* ke = (QKeyEvent*) e;
			as->keypressed(ke->key(), e->type() == QEvent::KeyPress);
			return true;
		}
		}

		if (gPop)
		{
			gPop = false;
			gStates.pop_back();
			delete as;
		}

		if (gStates.empty())
		{
			close();
			return true;
		}
	}

	return QGLWidget::event(e);
}

void View::initializeGL()
{
	initFonts();
	qtime.start();
	last_t = qtime.elapsed();
	time = 0;
	video.init(width(), height(), fullscreen != 0);
	gStates.push_back(new Menu());
	gPop = false;
}

void View::paintGL()
{
	if (gStates.empty())
		return;

	int t = qtime.elapsed();
	int dt = t - last_t;
	last_t = t;
	time += dt;
	float ftime = time / 1000.f;

	AppState* as = gStates[gStates.size() - 1];
	as->tick(ftime, dt / 1000.f);
	as->display(ftime, dt / 1000.f);
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	srand((unsigned int)time(0));

	int xres = 1024;
	int yres = 768;

	bool usePatch = true;

	getGamePath();

	std::cout << APP_TITLE " " APP_VERSION "\n"
	"Game path: " << gamepath << std::endl;

	const char* locale = 0;

	const char *locales[] = {"enUS", "enGB", "deDE", "frFR", "zhTW", "ruRU", "esES", "koKR", "zhCN"};

	char path[512];
	for (size_t i=0; i<9; i++) {
		if (file_exists(gamepath + locales[i] + "/base-" + locales[i] + ".MPQ")) {
			locale = locales[i];
			break;
		}
	}
	std::cout << "Locale: " << locale << std::endl;

	if (usePatch)
	{
		FS().add(gamepath + "patch-3.MPQ");
		FS().add(gamepath + "patch-2.MPQ");
		FS().add(gamepath + "patch.MPQ");

		FS().add(gamepath + locale + "/Patch-" + locale + "-2.MPQ");
		FS().add(gamepath + locale + "/Patch-" + locale + ".MPQ");
	}

	FS().add(gamepath + "expansion3.MPQ");
	FS().add(gamepath + "expansion2.MPQ");
	FS().add(gamepath + "lichking.MPQ");
	FS().add(gamepath + "expansion.MPQ");
	FS().add(gamepath + "common-3.MPQ");
	FS().add(gamepath + "common-2.MPQ");
	FS().add(gamepath + "common.MPQ");

	FS().add(gamepath + locale + "/expansion3-locale-" + locale + ".MPQ");
	FS().add(gamepath + locale + "/expansion2-locale-" + locale + ".MPQ");
	FS().add(gamepath + locale + "/lichking-locale-" + locale + ".MPQ");
	FS().add(gamepath + locale + "/expansion-locale-" + locale + ".MPQ");
	FS().add(gamepath + locale + "/locale-" + locale + ".MPQ");

	OpenDBs();

	View view;
	view.resize(xres, yres);
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
