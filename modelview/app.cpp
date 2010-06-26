#include <QApplication>
#include "app.h"
#include "globalvars.h"
#include <wx/dir.h>
#include <wx/tokenzr.h>

IMPLEMENT_APP_NO_MAIN(WowModelViewApp)

static bool qt_running = true;

void QtDummy::shutdown()
{
	qt_running = false;
}

int main(int argc, char **argv)
{
	QApplication qapp(argc, argv);
	QtDummy qtdummy;

	QObject::connect(&qapp, SIGNAL(lastWindowClosed()), //
		&qtdummy, SLOT(shutdown()));

	return wxEntry(argc, argv);
}

int WowModelViewApp::MainLoop()
{
	while (wxAppBase::Pending() || qt_running)
	{
		if(wxAppBase::Pending())
			wxAppBase::Dispatch();

		if(qt_running)
			QApplication::processEvents();
	}

	return 0;
}

bool WowModelViewApp::OnInit()
{
	wxFileName fname(argv[0]);
	wxString userPath = fname.GetPath(wxPATH_GET_VOLUME) + SLASH
		+ wxT("userSettings");
	wxFileName::Mkdir(userPath, 0777, wxPATH_MKDIR_FULL);

	// Application Info
	SetVendorName(_T("WoWModelViewer"));
	SetAppName(_T("WoWModelViewer"));

	// set the config file path.
	cfgPath = userPath + SLASH + wxT("Config.ini");

	bool loadfail = LoadSettings();
	if (loadfail == true)
	{
		return false;
	}

	// Now create our main frame.
	frame = new ModelViewer();
	SetTopWindow(frame);

	// Point our global vars at the correct memory location
	g_modelViewer = frame;
	g_canvas = frame->canvas;
	g_animControl = frame->animControl;
	g_charControl = frame->charControl;

	frame->interfaceManager.Update();

	if (frame->canvas)
	{
		frame->canvas->Show(true);

		if (!frame->canvas->init)
			frame->canvas->InitGL();

		if (frame->lightControl)
			frame->lightControl->UpdateGL();
	}

	// Load previously saved layout
	frame->LoadLayout();

	return true;
}

namespace
{
long traverseLocaleMPQs(const wxString locales[], size_t localeCount,
	const wxString localeArchives[], size_t archiveCount,
	const wxString& gamePath)
{
	long lngID = -1;

	for (size_t i = 0; i < localeCount; i++)
	{
		if (locales[i].IsEmpty())
			continue;
		wxArrayString localeMpqs;
		wxString localePath = gamePath;

		localePath.Append(locales[i]);
		localePath.Append(_T("/"));
		if (wxDir::Exists(localePath))
		{
			wxArrayString localeMpqs;
			wxDir::GetAllFiles(localePath, &localeMpqs, wxEmptyString,
				wxDIR_FILES);

			for (size_t j = 0; j < archiveCount; j++)
			{
				for (size_t k = 0; k < localeMpqs.size(); k++)
				{
					wxString baseName = wxFileName(localeMpqs[k]).GetFullName();
					wxString neededMpq = wxString::Format(localeArchives[j],
						locales[i].c_str());

					if (baseName.CmpNoCase(neededMpq) == 0)
					{
						mpqArchives.Add(localeMpqs[k]);
					}
				}
			}

			lngID = (long) i;
		}
	}

	return lngID;
}
}

bool WowModelViewApp::LoadSettings()
{
	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig(_T("Global"), wxEmptyString,
		cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

	// Graphic / Video display settings
	pConfig->SetPath(_T("/Graphics"));
	pConfig->Read(_T("FSAA"), &video.curCap.aaSamples, 0);
	pConfig->Read(_T("AccumulationBuffer"), &video.curCap.accum, 0);
	pConfig->Read(_T("AlphaBits"), &video.curCap.alpha, 0);
	pConfig->Read(_T("ColourBits"), &video.curCap.colour, 24);
	pConfig->Read(_T("DoubleBuffer"), (bool*) &video.curCap.doubleBuffer, 1); // True
#ifdef _WINDOWS
	pConfig->Read(_T("HWAcceleration"), &video.curCap.hwAcc, WGL_FULL_ACCELERATION_ARB);
#endif
	pConfig->Read(_T("SampleBuffer"), (bool*) &video.curCap.sampleBuffer, 0); // False
	pConfig->Read(_T("StencilBuffer"), &video.curCap.stencil, 0);
	pConfig->Read(_T("ZBuffer"), &video.curCap.zBuffer, 16);

	// Application locale info
	pConfig->SetPath(_T("/Locale"));
	pConfig->Read(_T("LanguageID"), &langID, -1);
	pConfig->Read(_T("InterfaceID"), &interfaceID, -1);

	// Application settings
	pConfig->SetPath(_T("/Settings"));
	pConfig->Read(_T("Path"), &gamePath, wxEmptyString);
	pConfig->Read(_T("TOCVersion"), &gameVersion, 0);

	pConfig->Read(_T("UseLocalFiles"), &useLocalFiles, false);
	pConfig->Read(_T("SSCounter"), &ssCounter, 100);
	//pConfig->Read(_T("AntiAlias"), &useAntiAlias, true);
	//pConfig->Read(_T("DisableHWAcc"), &disableHWAcc, false);
	pConfig->Read(_T("DefaultFormat"), &imgFormat, 0);
	pConfig->Read(_T("ModelExportInitOnly"), &modelExportInitOnly, true);
	pConfig->Read(_T("ModelExportPreserveDirs"), &modelExport_PreserveDir, true);
	pConfig->Read(_T("ModelExportUseWMVPosRot"), &modelExport_UseWMVPosRot,
		false);

	pConfig->Read(_T("ModelExportLWPreserveDirs"), &modelExport_LW_PreserveDir,
		true);
	pConfig->Read(_T("ModelExportLWExportLights"),
		&modelExport_LW_ExportLights, true);
	pConfig->Read(_T("ModelExportLWExportDoodads"),
		&modelExport_LW_ExportDoodads, true);
	pConfig->Read(_T("ModelExportLWDoodadsAs"), &modelExport_LW_DoodadsAs, 0);

	// Data path and mpq archive stuff
	wxString archives;
	pConfig->Read(_T("MPQFiles"), &archives);

	wxStringTokenizer strToken(archives, _T(";"), wxTOKEN_DEFAULT);
	while (strToken.HasMoreTokens())
	{
		mpqArchives.Add(strToken.GetNextToken());
	}

	if (gamePath.IsEmpty() || !wxDirExists(gamePath))
	{
		getGamePath();
		mpqArchives.Clear();
	}

	if (gamePath.Last() != SLASH)
		gamePath.Append(SLASH, 1);

	if (!wxFileExists(gamePath + wxT("common.MPQ")))
	{
		wxLogMessage(
			_T("World of Warcraft Data Directory Not Found. Returned GamePath: %s"),
			gamePath.c_str());
		wxMessageDialog
			*dial =
				new wxMessageDialog(
					NULL,
					wxT("Fatal Error: Could not find your World of Warcraft Data folder."),
					wxT("World of Warcraft Not Found"), wxOK | wxICON_ERROR);
		dial->ShowModal();
		return true;
	}

	if (mpqArchives.GetCount() == 0)
	{
		//enUS(enGB), koKR, frFR, deDE, zhCN, zhTW, esES, ruRU
		const wxString locales[] = { _T("enUS"), _T("koKR"), _T("frFR"),
			_T("deDE"), _T("zhCN"), _T("zhTW"), _T("esES"), _T("ruRU") };
		const wxString locales2[] = { _T("enGB"), wxEmptyString, wxEmptyString,
			wxEmptyString, _T("enCN"), _T("enTW"), _T("esMX"), wxEmptyString };

		const wxString defaultArchives[] = { _T("patch-3.mpq"),
			_T("patch-2.mpq"), _T("patch.mpq"), _T("expansion3.mpq"),
			_T("expansion2.mpq"), _T("lichking.mpq"), _T("expansion.mpq"),
			_T("common-3.mpq"), _T("common-2.mpq"), _T("common.mpq") };
		const wxString localeArchives[] = { _T("patch-%s-3.mpq"),
			_T("patch-%s-2.mpq"), _T("patch-%s.mpq"),
			_T("expansion3-locale-%s.mpq"), _T("expansion2-locale-%s.mpq"),
			_T("lichking-locale-%s.mpq"), _T("expansion-locale-%s.mpq"),
			_T("locale-%s.mpq"), _T("base-%s.mpq") };

		wxArrayString baseMpqs;
		wxDir::GetAllFiles(gamePath, &baseMpqs, wxEmptyString, wxDIR_FILES);

		for (size_t i = 0; i < WXSIZEOF(defaultArchives); i++)
		{
			wxLogMessage(_T("Searching for MPQ archive %s..."),
				defaultArchives[i].c_str());

			for (size_t j = 0; j < baseMpqs.size(); j++)
			{
				wxString baseName = wxFileName(baseMpqs[j]).GetFullName();
				if (baseName.CmpNoCase(defaultArchives[i]) == 0)
				{
					mpqArchives.Add(baseMpqs[j]);

					wxLogMessage(_T("- Found MPQ archive: %s"),
						baseMpqs[j].c_str());
				}
			}
		}

		langID = traverseLocaleMPQs(locales, WXSIZEOF(locales), localeArchives,
			WXSIZEOF(localeArchives), gamePath);

		if (langID == -1)
		{
			langID = traverseLocaleMPQs(locales2, WXSIZEOF(locales2),
				localeArchives, WXSIZEOF(localeArchives), gamePath);
		}
	}
	// -------


	// Clear our ini file config object
	wxDELETE( pConfig );

	if (interfaceID == -1)
		interfaceID = langID;
	if (langOffset == -1)
	{
		if (gameVersion == 40000)
			langOffset = 0;
		else
			langOffset = langID;
	}
	return false;
}
