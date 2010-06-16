#include "util.h"
//#include "CxImage/ximage.h"
#ifdef _WINDOWS
#include <windows.h>
#include <wx/msw/winundef.h>
#endif
#include <wx/choicdlg.h>
#include <wx/dir.h>
#include <wx/dirdlg.h>

wxString gamePath;
wxString cfgPath;
wxString bgImagePath;

wxArrayString mpqArchives;

int gameVersion = 0;

bool useLocalFiles = false;
bool useRandomLooks = true;
bool bHideHelmet = false;
bool bKnightEyeGlow = true;
bool bShowParticle = true;
bool bZeroParticle = true;

// Model Export Options
// General Options
bool modelExportInitOnly = true;
bool modelExport_PreserveDir = true;
bool modelExport_UseWMVPosRot = false;

// Lightwave Options
bool modelExport_LW_PreserveDir = true;
bool modelExport_LW_ExportLights = true;
bool modelExport_LW_ExportDoodads = true;
int modelExport_LW_DoodadsAs = 0;
// X3D Options
bool modelExport_X3D_ExportAnimation = false;
bool modelExport_X3D_CenterModel = true;

long langID = -1;
long langOffset = -1;
long interfaceID = -1;
int ssCounter = 100; // ScreenShot Counter
int imgFormat = 0;

wxString locales[8] = {_T("enUS"), _T("koKR"), _T("frFR"), _T("deDE"), _T("zhCN"),  _T("zhTW"),  _T("esES"),  _T("ruRU")};

/*
wxString langCSConv[] =
{
	_T("iso-8859-1"),
	wxEmptyString,
	wxEmptyString,
	wxEmptyString,
	_T("gb2312"),
	_T("big5"),
	wxEmptyString,
	wxEmptyString,
};
wxString CSConvStr;
*/

wxString CSConv(wxString str)
{
	if (langID <= 0) // || langCSConv[langID].IsEmpty())
		return str;
	return wxConvLocal.cWC2WX(wxConvUTF8.cMB2WC(str.mb_str())); // from private.h
	//CSConvStr = wxCSConv(langCSConv[langID]).cWC2WX(wxConvUTF8.cMB2WC(str));
	//return CSConvStr;
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

void fixname(std::string &name)
{
	for (uint32 i=0; i<name.length(); i++) {
		if (i>0 && name[i]>='A' && name[i]<='Z' && isalpha(name[i-1])) {
			name[i] |= 0x20;
		} else if ((i==0 || !isalpha(name[i-1])) && name[i]>='a' && name[i]<='z') {
			name[i] &= ~0x20;
		}
	}
}
void fixnamen(char *name, uint32 len)
{
	for (uint32 i=0; i<len; i++) {
		if (i>0 && name[i]>='A' && name[i]<='Z' && isalpha(name[i-1])) {
			name[i] |= 0x20;
		} else if ((i==0 || !isalpha(name[i-1])) && name[i]>='a' && name[i]<='z') {
			name[i] &= ~0x20;
		}
	}
}

int wxStringToInt(const wxString& str)
{
	long number = 0;
	str.ToLong(&number);
	return number;
}

// Byteswap for 2 Bytes
unsigned short _SwapTwoBytes (unsigned short w)
{
	unsigned short tmp;
	tmp =  (w & 0x00ff);
	tmp = ((w & 0xff00) >> 0x08) | (tmp << 0x08);
	return tmp;
}

// Round a float, down to the specified decimal
float round(float input, int limit = 2){
	if (limit > 0){
		input *= (10^limit);
	}
	input = int(input+0.5);
	if (limit > 0){
		input /= (10^limit);
	}
	return input;
}

void MakeDirs(wxString base, wxString paths){
	wxString NewBase = base;
	//wxLogMessage("MKDIR Paths\nBasePath: %s\nOthers Paths: %s", base, paths);
	wxString Paths[30];
	unsigned int PathNum = 0;
	while (paths.Find(SLASH)>0){
		Paths[PathNum] = paths.BeforeFirst(SLASH);
		wxString rep = Paths[PathNum];
		paths.Replace(rep.Append(SLASH),wxT(""),true);
		//wxLogMessage("\nBuilding Paths: %s\npaths:%s",Paths[PathNum],paths);
		PathNum++;
	}
	Paths[PathNum] = paths;
	PathNum++;

	for (unsigned int x=0;x<PathNum;x++){
		NewBase = wxString(NewBase << SLASH << Paths[x]);
		//wxLogMessage("Attempting to create the following directory: %s",NewBase);
		wxMkdir(NewBase);
	}
}

void getGamePath()
{
#ifdef _WINDOWS
	HKEY key;
	unsigned long t, s;
	long l;
	unsigned char path[1024];
	memset(path, 0, sizeof(path));

	wxString sNames[5];
	int nNames = 0;
	int sName = 0;

	// if it failed, look for World of Warcraft install
	const wxString regpaths[] = {
#ifdef _WIN32
		_T("SOFTWARE\\Blizzard Entertainment\\World of Warcraft"),
		_T("SOFTWARE\\Blizzard Entertainment\\World of Warcraft\\PTR"),
		_T("SOFTWARE\\Blizzard Entertainment\\World of Warcraft\\Beta")
#else //_WIN64
		_T("SOFTWARE\\Wow6432Node\\Blizzard Entertainment\\World of Warcraft"),
		_T("SOFTWARE\\Wow6432Node\\Blizzard Entertainment\\World of Warcraft\\PTR"),
		_T("SOFTWARE\\Wow6432Node\\Blizzard Entertainment\\World of Warcraft\\Beta")
#endif
		 };
	int sTypes[3];

	for (uint32 i=0; i<WXSIZEOF(regpaths); i++) {
		l = RegOpenKeyEx((HKEY)HKEY_LOCAL_MACHINE, regpaths[i], 0, KEY_QUERY_VALUE, &key);

		if (l == ERROR_SUCCESS) {
			s = sizeof(path);
			l = RegQueryValueEx(key, _T("InstallPath"), 0, &t,(LPBYTE)path, &s);
			if (l == ERROR_SUCCESS && wxDir::Exists(path)) {
				sNames[nNames] = path;
				if (i==1)
					sTypes[nNames] = 1;
				else
					sTypes[nNames] = 0;
				nNames++;
			}
			RegCloseKey(key);
		}
	}
	if (nNames == 1)
		sName = 0;
	else if (nNames >= 1) {
		sName = wxGetSingleChoiceIndex(_T("Please select a Path:"), _T("Path"), nNames, sNames);
		if (sName == -1)
			sName = 0;
	} else
		sName = -1;

	// If we found an install then set the game path, otherwise just set to C:\ for now
	if (sName >= 0) {
		gamePath = sNames[sName];
		gamePath.Append(_T("Data\\"));
	} else {
		gamePath = _T("C:")+SLASH;
		if (!wxFileExists(gamePath + SLASH + _T("data") + SLASH + _T("common.MPQ")) && !gamePath.empty()){
			gamePath = wxDirSelector(wxT("Please select your World of Warcraft folder:"),gamePath);
			gamePath.append(SLASH+_T("Data")+SLASH);
		}
	}
#elif _MAC // Mac OS X
    gamePath = wxT("/Applications/World\\ of\\ Warcraft/");
	if (!wxFileExists(gamePath + SLASH + _T("data") + SLASH + _T("common.MPQ")) && !gamePath.empty()){
        gamePath = wxDirSelector(wxT("Please select your World of Warcraft folder:"),gamePath);
		gamePath.Append(_T("/Data/"));
    }
#else // Linux
	gamePath = _T(".")+SLASH;
	if (!wxFileExists(gamePath + SLASH + _T("data") + SLASH + _T("common.MPQ")) && !gamePath.empty()){
		gamePath = wxDirSelector(wxT("Please select your World of Warcraft folder:"),gamePath);
		gamePath.Append(_T("/Data/"));
	}
#endif
}


