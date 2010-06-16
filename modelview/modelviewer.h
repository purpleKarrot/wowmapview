
#ifndef MODELVIEWER_H
#define MODELVIEWER_H

// wx
#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include <wx/filename.h>
#include <wx/fileconf.h>
#include <wx/treectrl.h>
#include <wx/colordlg.h>
#include <wx/msgdlg.h>
#include <wx/display.h>
#include <wx/aboutdlg.h>
#include <wx/url.h>
#include <wx/xml/xml.h>
#include <wx/wfstream.h>
#ifdef	PLAY_MUSIC
#include <wx/mediactrl.h>
#endif

//wxAUI
#include <wx/aui/aui.h>

// Our files
#include "modelcanvas.h"
#include "animcontrol.h"
#include "charcontrol.h"
#include "lightcontrol.h"
#include "modelcontrol.h"
#include "imagecontrol.h"
#include "util.h"
#include "AnimExporter.h"
#include "effects.h"
#include "arrows.h"
#include "modelexport.h"
#include "settings.h"
#include "modelbankcontrol.h"
#include "filecontrol.h"
#include "modelexportoptions.h"

#include "enums.h"

//#include "CShader.h"

// defines
#define APP_TITLE _T("World of Warcraft Model Viewer")
#define APP_VERSION _T("v0.7.0.1 r225")		// Revision number should match the revision number on GoogleCode.
#ifdef _DEBUG
	#define APP_ISDEBUG _T(" Debug")
#else
	#define APP_ISDEBUG _T("")
#endif

// This should only be touched when adding a new OS or platform.
#if defined (_WINDOWS)
	#if defined (_WIN64)
		#define APP_PLATFORM _T("Windows 64-bit")
	#elif defined (_WIN32)
		#define APP_PLATFORM _T("Windows 32-bit")
	#else
		#error _T("Your Windows platform is not defined. Please specify either _WIN64 or _WIN32.")
	#endif
#elif defined (_MAC)
	#if defined (_MAC_INTEL)
		#define APP_PLATFORM _T("Macintosh Intel")
	#elif defined (_MAC_PPC)
		#define APP_PLATFORM _T("Macintosh PowerPC")
	#else
		#error _T("Your Macintosh platform is not defined. Please specify either _MAC_INTEL or _MAC_PPC.")
	#endif
#elif defined (_LINUX)
	#if defined (_LINUX64)
		#define APP_PLATFORM _T("Linux 64-bit")
	#elif defined (_LINUX32)
		#define APP_PLATFORM _T("Linux 32-bit")
	#else
		#error _T("Your Linux platform is not defined. Please specify either _LINUX64 or _LINUX32.")
	#endif
#else
	#ifdef _DEBUG
		#error _T("You have not specified a valid Operating System for your Debug configuration.")
	#else
		#error _T("You have not specified a valid Operating System for your Release configuration.")
	#endif
#endif

class ModelViewer: public wxFrame
{    
    DECLARE_CLASS(ModelViewer)
    DECLARE_EVENT_TABLE()

	std::vector<MPQArchive*> archives;

public:
	// Constructor + Deconstructor
	ModelViewer();
	~ModelViewer();

	// our class objects
	AnimControl *animControl;
	ModelCanvas *canvas;
	CharControl *charControl;
	EnchantsDialog *enchants;
	LightControl *lightControl;
	ModelControl *modelControl;
	ArrowControl *arrowControl;
	ImageControl *imageControl;
	//SoundControl *soundControl;
	SettingsControl *settingsControl;
	ModelBankControl *modelbankControl;
	ModelOpened *modelOpened;
	ModelExportOptions_Control *exportOptionsControl;

	CAnimationExporter *animExporter;

	FileControl *fileControl;

	//wxWidget objects
	wxMenuBar *menuBar;
	wxMenu *fileMenu, *exportMenu, *camMenu, *charMenu, *viewMenu, *optMenu, *lightMenu;
	
	// wxAUI - new docking lib (now part of wxWidgets 2.8.0)
	wxAuiManager interfaceManager;

	// Boolean flags
	bool isModel;
	bool isChar;
	bool isWMO;
	bool isADT;
	bool initDB;

	// Initialising related functions
	void InitMenu();
	void InitObjects();
	bool Init();
	void InitDocking();
	void InitDatabase();
	bool InitMPQArchives();

	// Save and load various settings between sessions
	void LoadSession();
	void SaveSession();
	// Save and load the GUI layout
	void LoadLayout();
	void SaveLayout();
	void ResetLayout();
	// save + load character *.CHR files
	void LoadChar(const char *fn);
	void SaveChar(const char *fn);

	void LoadModel(const wxString fn);
	void LoadItem(unsigned int displayID);
	void LoadNPC(unsigned int modelid);

	// Window GUI event related functions
	//void OnIdle();
	void OnClose(wxCloseEvent &event);
	void OnSize(wxSizeEvent &event);
    void OnExit(wxCommandEvent &event);


    // menu commands
	void OnToggleDock(wxCommandEvent &event);
	void OnToggleCommand(wxCommandEvent &event);
	void OnSetColor(wxCommandEvent &event);
	void OnEffects(wxCommandEvent &event);
	void OnLightMenu(wxCommandEvent &event);
	void OnCamMenu(wxCommandEvent &event);

	// Wrapper function for character stuff (forwards events to charcontrol)
	void OnSetEquipment(wxCommandEvent &event);
	void OnCharToggle(wxCommandEvent &event);

	void OnMount(wxCommandEvent &event);
	void OnSave(wxCommandEvent &event);
	void OnBackground(wxCommandEvent &event);
	void OnLanguage(wxCommandEvent &event);
	void OnAbout(wxCommandEvent &event);
	void OnCheckForUpdate(wxCommandEvent &event);
	void OnCanvasSize(wxCommandEvent &event);
	void OnTest(wxCommandEvent &event);
	void OnExport(wxCommandEvent &event);
	void OnExportOther(wxCommandEvent &event);
	
	void UpdateControls();
   
	void ImportArmoury(wxString strURL);
	void ModelInfo();

	Vec3D DoSetColor(const Vec3D &defColor);
};

#endif

