#include "app.h"

/*	THIS IS OUR MAIN "START UP" FILE.
	App.cpp creates our wxApp class object.
	the wxApp initiates our program (takes over the role of main())
	When our wxApp loads,  it creates our ModelViewer class object,  
	which is a wxWindow.  From there ModelViewer object then creates
	our menu bar, character control, view control, filetree control, 
	animation control, and the canvas control (opengl).  Once those 
	controls are created it then loads saved variables from the config.ini
	file.  Then it proceeds	to create and open the MPQ archives,  creating
	a file list of the contents from all files within all of the opened mpq archives.

	I hope this gives some insight into the "program flow".
*/
/*
#ifdef _DEBUG
	#define new DEBUG_CLIENTBLOCK
#endif
*/

// tell wxwidgets which class is our app
IMPLEMENT_APP(WowModelViewApp)

//#include "globalvars.h"

bool WowModelViewApp::OnInit()
{
	frame = NULL;
	FILE *LogFile = NULL;


	// Error & Logging settings
#ifndef _DEBUG
	#if wxUSE_ON_FATAL_EXCEPTION
		wxHandleFatalExceptions(true);
	#endif
#endif

	wxFileName fname(argv[0]);
	wxString userPath = fname.GetPath(wxPATH_GET_VOLUME)+SLASH+wxT("userSettings");
	wxFileName::Mkdir(userPath, 0777, wxPATH_MKDIR_FULL);

	// set the log file path.
	wxString logPath = userPath+SLASH+wxT("log.txt");

	LogFile = fopen(logPath.mb_str(), "w+");
	if (LogFile) {
		wxLog *logger = new wxLogStderr(LogFile);
		delete wxLog::SetActiveTarget(logger);
		wxLog::SetVerbose(false);
	}

	// Application Info
	SetVendorName(_T("WoWModelViewer"));
	SetAppName(_T("WoWModelViewer"));

	// Just a little header to start off the log file.
	wxLogMessage(wxString(_T("Starting:\n") APP_TITLE _T(" ") APP_VERSION _T(" ") APP_PLATFORM APP_ISDEBUG _T("\n\n")));

	// set the config file path.
	cfgPath = userPath+SLASH+wxT("Config.ini");

	bool loadfail = LoadSettings();
	if (loadfail == true){
		return false;
	}

#ifdef _WINDOWS
	// This chunk of code is all related to locale translation (if a translation is available).
	// Only use locale for non-english?
	wxString fn = _T("mo");
	fn = fn+SLASH+locales[interfaceID]+_T(".mo");
	if (wxFileExists(fn))
	{
		locale.Init(langIds[interfaceID], wxLOCALE_CONV_ENCODING);
		
		wxLocale::AddCatalogLookupPathPrefix(_T("mo"));
		//wxLocale::AddCatalogLookupPathPrefix(_T(".."));

		//locale.AddCatalog(_T("wowmodelview")); // Initialize the catalogs we'll be using
		locale.AddCatalog(locales[interfaceID]);
	}
#endif

	// Now create our main frame.
    frame = new ModelViewer();
    
	if (!frame) {
		//this->Close();
		return false;
	}
	
	SetTopWindow(frame);

	// Set the icon, different source location for the icon under Linux & Mac
	wxIcon icon;
#if defined (_WINDOWS)
	if (icon.LoadFile(_T("mainicon"),wxBITMAP_TYPE_ICO_RESOURCE) == false)
		wxMessageBox(_T("Failed to load Icon"),_T("Failure"));
#elif defined (_LINUX)
	// This probably needs to be fixed...
	//if (icon.LoadFile(_T("../bin_support/icon/wmv_xpm")) == false)
	//	wxMessageBox(_T("Failed to load Icon"),_T("Failure"));
#elif defined (_MAC)
	// Dunno what to do about Macs...
	//if (icon.LoadFile(_T("../bin_support/icon/wmv.icns")) == false)
	//	wxMessageBox(_T("Failed to load Icon"),_T("Failure"));
#endif
	frame->SetIcon(icon);
	// --

	// Point our global vars at the correct memory location
	g_modelViewer = frame;
	g_canvas = frame->canvas;
	g_animControl = frame->animControl;
	g_charControl = frame->charControl;
	g_fileControl = frame->fileControl;

	frame->interfaceManager.Update();

	if (frame->canvas) {
		frame->canvas->Show(true);
		
		if (!frame->canvas->init)
			frame->canvas->InitGL();

		if (frame->lightControl)
			frame->lightControl->UpdateGL();
	}
	// --
	

	// TODO: Improve this feature and expand on it.
	// Command arguments
	wxString cmd;
	for (int i=0; i<argc; i++) {
		cmd = argv[i];

		if (cmd == _T("-m")) {
			if (i+1 < argc) {
				i++;
				wxString fn(argv[i]);

				// Error check
				if (fn.Last() != '2') // Its not an M2 file, exit
					break;

				// Load the model
				frame->LoadModel(fn);
			}
		} else if (cmd == _T("-mo")) {
			if (i+1 < argc) {
				i++;
				wxString fn(argv[i]);

				if (fn.Last() != '2') // Its not an M2 file, exit
					break;

				// If its a character model, give it some skin.
				// Load the model
				frame->LoadModel(fn);

				// Output the screenshot
				fn = fn.AfterLast('\\');
				fn = fn.BeforeLast('.');
				fn.Prepend(_T("ss_"));
				fn.Append(_T(".png"));
				frame->canvas->Screenshot(fn);
			}
		} else {
			wxString tmp = cmd.AfterLast('.');
			if (!tmp.IsNull() && !tmp.IsEmpty() && tmp.IsSameAs(wxT("chr"), false))
				frame->LoadChar(cmd.fn_str());
		}
	}
	// -------

	// Load previously saved layout
	frame->LoadLayout();

	wxLogMessage(_T("WoW Model Viewer successfully loaded!\n----\n"));
	
	return true;
}

void WowModelViewApp::OnFatalException()
{
	//wxApp::SetExitOnFrameDelete(false);

	/*
	wxDebugReport report;
    wxDebugReportPreviewStd preview;

	report.AddAll(wxDebugReport::Context_Exception);

    if (preview.Show(report))
        report.Process();
	*/

	if (frame != NULL) {
		frame->Destroy();
		frame = NULL;
	}
}

int WowModelViewApp::OnExit()
{
	SaveSettings();
	
	//if (frame != NULL)
	//	frame->Destroy();

//#ifdef _DEBUG
	//delete wxLog::SetActiveTarget(NULL);
	if (LogFile) {
		fclose(LogFile);
		//wxDELETE(LogFile);
		LogFile = NULL;
	}
//#endif

	CleanUp();

	//_CrtMemDumpAllObjectsSince( NULL );

	return 0;
}

/*
void WowModelViewApp::HandleEvent(wxEvtHandler *handler, wxEventFunction func, wxEvent& event) const 
{ 
    try 
    {        
        HandleEvent(handler, func, event); 
	} 
	catch(...) 
	{ 
		wxMessageBox(_T("An error occured while handling an application event."), _T("Execption in event handling"), wxOK | wxICON_ERROR); 
		throw; 
	} 
}
*/

void WowModelViewApp::OnUnhandledException() 
{ 
    //wxMessageBox(_T("An unhandled exception was caught, the program will now terminate."), _T("Unhandled Exception"), wxOK | wxICON_ERROR); 
	wxLogFatalError(_T("An unhandled exception error has occured."));
}

namespace {
	long traverseLocaleMPQs(const wxString locales[], size_t localeCount, const wxString localeArchives[], size_t archiveCount, const wxString& gamePath)
	{
		long lngID = -1;

		for (size_t i = 0; i < localeCount; i++) {
			if (locales[i].IsEmpty())
				continue;
			wxArrayString localeMpqs;
			wxString localePath = gamePath;

			localePath.Append(locales[i]);
			localePath.Append(_T("/"));
			if (wxDir::Exists(localePath)) {
				wxArrayString localeMpqs;
				wxDir::GetAllFiles(localePath, &localeMpqs, wxEmptyString, wxDIR_FILES);

				for (size_t j = 0; j < archiveCount; j++) {
					for (size_t k = 0; k < localeMpqs.size(); k++) {
						wxString baseName = wxFileName(localeMpqs[k]).GetFullName();
						wxString neededMpq = wxString::Format(localeArchives[j], locales[i].c_str());

						if(baseName.CmpNoCase(neededMpq) == 0) {
							mpqArchives.Add(localeMpqs[k]);
						}
					}
				}

				lngID = (long)i;
			}
		}

		return lngID;
	}
}

bool WowModelViewApp::LoadSettings()
{
	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig(_T("Global"), wxEmptyString, cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
	
	// Graphic / Video display settings
	pConfig->SetPath(_T("/Graphics"));
	pConfig->Read(_T("FSAA"), &video.curCap.aaSamples, 0);
	pConfig->Read(_T("AccumulationBuffer"), &video.curCap.accum, 0);
	pConfig->Read(_T("AlphaBits"), &video.curCap.alpha, 0);
	pConfig->Read(_T("ColourBits"), &video.curCap.colour, 24);
	pConfig->Read(_T("DoubleBuffer"), (bool*)&video.curCap.doubleBuffer, 1);	// True
#ifdef _WINDOWS
	pConfig->Read(_T("HWAcceleration"), &video.curCap.hwAcc, WGL_FULL_ACCELERATION_ARB);
#endif
	pConfig->Read(_T("SampleBuffer"), (bool*)&video.curCap.sampleBuffer, 0);	// False
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
	pConfig->Read(_T("ModelExportUseWMVPosRot"), &modelExport_UseWMVPosRot, false);

	pConfig->Read(_T("ModelExportLWPreserveDirs"), &modelExport_LW_PreserveDir, true);
	pConfig->Read(_T("ModelExportLWExportLights"), &modelExport_LW_ExportLights, true);
	pConfig->Read(_T("ModelExportLWExportDoodads"), &modelExport_LW_ExportDoodads, true);
	pConfig->Read(_T("ModelExportLWDoodadsAs"), &modelExport_LW_DoodadsAs, 0);


	// Data path and mpq archive stuff
	wxString archives;
	pConfig->Read(_T("MPQFiles"), &archives);
	
	wxStringTokenizer strToken(archives, _T(";"), wxTOKEN_DEFAULT);
	while (strToken.HasMoreTokens()) {
		mpqArchives.Add(strToken.GetNextToken());
	}

	if (gamePath.IsEmpty() || !wxDirExists(gamePath)) {
		getGamePath();
		mpqArchives.Clear();
	}

	if (gamePath.Last() != SLASH)
		gamePath.Append(SLASH, 1);

	if (!wxFileExists(gamePath + wxT("common.MPQ"))){
		wxLogMessage(_T("World of Warcraft Data Directory Not Found. Returned GamePath: %s"),gamePath.c_str());
		wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("Fatal Error: Could not find your World of Warcraft Data folder."), wxT("World of Warcraft Not Found"), wxOK | wxICON_ERROR);
		dial->ShowModal();
		return true;
	}

	
	if (mpqArchives.GetCount()==0) {
		//enUS(enGB), koKR, frFR, deDE, zhCN, zhTW, esES, ruRU
		const wxString locales[] = {_T("enUS"), _T("koKR"), _T("frFR"), _T("deDE"), _T("zhCN"),  _T("zhTW"),  _T("esES"),  _T("ruRU")};
		const wxString locales2[] = {_T("enGB"), wxEmptyString, wxEmptyString, wxEmptyString, _T("enCN"), _T("enTW"), _T("esMX"), wxEmptyString};

		const wxString defaultArchives[] = {_T("patch-3.mpq"),_T("patch-2.mpq"),_T("patch.mpq"),_T("expansion3.mpq"),_T("expansion2.mpq"),_T("lichking.mpq"),_T("expansion.mpq"),_T("common-3.mpq"),_T("common-2.mpq"), _T("common.mpq")};
		const wxString localeArchives[] = {_T("patch-%s-3.mpq"), _T("patch-%s-2.mpq"), _T("patch-%s.mpq"), _T("expansion3-locale-%s.mpq"), _T("expansion2-locale-%s.mpq"), _T("lichking-locale-%s.mpq"), _T("expansion-locale-%s.mpq"), _T("locale-%s.mpq"), _T("base-%s.mpq")};

		wxArrayString baseMpqs;
		wxDir::GetAllFiles(gamePath, &baseMpqs, wxEmptyString, wxDIR_FILES);

		for (size_t i = 0; i < WXSIZEOF(defaultArchives); i++) {
			wxLogMessage(_T("Searching for MPQ archive %s..."), defaultArchives[i].c_str());

			for (size_t j = 0; j < baseMpqs.size(); j++) {
				wxString baseName = wxFileName(baseMpqs[j]).GetFullName();
				if(baseName.CmpNoCase(defaultArchives[i]) == 0) {
					mpqArchives.Add(baseMpqs[j]);

					wxLogMessage(_T("- Found MPQ archive: %s"), baseMpqs[j].c_str());
				}
			}
		}

		langID = traverseLocaleMPQs(locales, WXSIZEOF(locales), localeArchives, WXSIZEOF(localeArchives), gamePath);

		if (langID == -1) {
			langID = traverseLocaleMPQs(locales2, WXSIZEOF(locales2), localeArchives, WXSIZEOF(localeArchives), gamePath);
		}
	}
	// -------
	

	// Clear our ini file config object
	wxDELETE( pConfig );

    if (langID == -1) {
        // the arrays should be in sync
        wxCOMPILE_TIME_ASSERT(WXSIZEOF(langNames) == WXSIZEOF(langIds), LangArraysMismatch);
        langID = wxGetSingleChoiceIndex(_T("Please select a language:"), _T("Language"), WXSIZEOF(langNames), langNames);

		SaveSettings();
	}
	if (interfaceID == -1)
		interfaceID = langID;
	if (langOffset == -1) {
		if (gameVersion == 40000)
			langOffset = 0;
		else
			langOffset = langID;
	}
	return false;
}

void WowModelViewApp::SaveSettings()
{
	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig(_T("Global"), wxEmptyString, cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
	
	pConfig->SetPath(_T("/Locale"));
	pConfig->Write(_T("LanguageID"), langID);
	pConfig->Write(_T("InterfaceID"), interfaceID);

	pConfig->SetPath(_T("/Settings"));
	pConfig->Write(_T("Path"), gamePath);
	pConfig->Write(_T("TOCVersion"), gameVersion);
	pConfig->Write(_T("UseLocalFiles"), useLocalFiles);
	pConfig->Write(_T("SSCounter"), ssCounter);
	//pConfig->Write(_T("AntiAlias"), useAntiAlias);
	//pConfig->Write(_T("DisableHWAcc"), disableHWAcc);
	pConfig->Write(_T("DefaultFormat"), imgFormat);
	pConfig->Write(_T("ModelExportInitOnly"), modelExportInitOnly);
	pConfig->Write(_T("ModelExportPreserveDirs"), modelExport_PreserveDir);
	pConfig->Write(_T("ModelExportUseWMVPosRot"), modelExport_UseWMVPosRot);

	pConfig->Write(_T("ModelExportLWPreserveDirs"), modelExport_LW_PreserveDir);
	pConfig->Write(_T("ModelExportLWExportLights"), modelExport_LW_ExportLights);
	pConfig->Write(_T("ModelExportLWExportDoodads"), modelExport_LW_ExportDoodads);
	pConfig->Write(_T("ModelExportLWDoodadsAs"), modelExport_LW_DoodadsAs);

	wxString archives;

	for (size_t i=0; i<mpqArchives.GetCount(); i++) {
		archives.Append(mpqArchives[i]);
		archives.Append(_T(";"));
	}

	pConfig->Write(_T("MPQFiles"), archives);

	// Clear our ini file config object
	wxDELETE( pConfig );
}


