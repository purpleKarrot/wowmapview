
#include <QApplication>
//#include <QGLWidget>

#include <ctime>
#include <cstdlib>

#include "mpq.h"
#include "video.h"
#include "appstate.h"

#include "areadb.h"

#include "widgets/MainWindow.hpp"

static std::string gamepath;

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

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	srand((unsigned int)time(0));

	int xres = 1024;
	int yres = 768;

	bool usePatch = true;

	getGamePath();

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

	MainWindow view;
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
