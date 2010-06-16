#ifndef APP_H
#define APP_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifndef _WINDOWS
#include "wmv.xpm"
#endif

#include <wx/app.h>
#include <wx/log.h>
#include <wx/tokenzr.h>
#include <wx/dir.h>
#include <wx/aui/aui.h>

#include "util.h"
#include "globalvars.h"
#include "modelviewer.h"

// vars
const wxString langNames[] =
{
	_T("English"),
	_T("Korean"),
	_T("French"),
	_T("German"),
	_T("Chinese (Simplified)"),
	_T("Chinese (Traditional)"),
	_T("Spanish"),
	_T("Russian"),
};

static const wxLanguage langIds[] =
{
	wxLANGUAGE_ENGLISH,
	wxLANGUAGE_KOREAN,
	wxLANGUAGE_FRENCH,
	wxLANGUAGE_GERMAN,
	wxLANGUAGE_CHINESE_SIMPLIFIED,
	wxLANGUAGE_CHINESE_TRADITIONAL,
	wxLANGUAGE_SPANISH,
	wxLANGUAGE_RUSSIAN,
};

class WowModelViewApp : public wxApp
{
public:
    virtual bool OnInit();
	virtual int OnExit();
	virtual void OnUnhandledException();
	virtual void OnFatalException();

	bool LoadSettings();
	void SaveSettings();

	ModelViewer *frame;

	wxLocale locale;
	FILE *LogFile;
};

#endif

