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
#include "modelcontrol.h"
#include "util.h"
#include "effects.h"
#include "enums.h"

class ModelViewer: public wxFrame
{
DECLARE_CLASS(ModelViewer)
DECLARE_EVENT_TABLE()

public:
	ModelViewer();
	~ModelViewer();
	AnimControl *animControl;
	ModelCanvas *canvas;
	CharControl *charControl;
	EnchantsDialog *enchants;
	ModelControl *modelControl;
//	ImageControl *imageControl;
	ModelOpened *modelOpened;

	//wxWidget objects
	wxMenuBar *menuBar;
	wxMenu *fileMenu, *camMenu, *charMenu, *viewMenu, *optMenu, *lightMenu;

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

	void LoadModel(const std::string& fn);
	void LoadItem(unsigned int displayID);
	void LoadNPC(unsigned int modelid);

	// Window GUI event related functions
	void OnClose(wxCloseEvent &event);
	void OnSize(wxSizeEvent &event);
	void OnExit(wxCommandEvent &event);

	// menu commands
	void OnToggleDock(wxCommandEvent &event);
	void OnToggleCommand(wxCommandEvent &event);
	void OnEffects(wxCommandEvent &event);

	// Wrapper function for character stuff (forwards events to charcontrol)
	void OnSetEquipment(wxCommandEvent &event);
	void OnCharToggle(wxCommandEvent &event);

	void OnMount(wxCommandEvent &event);
	void OnAbout(wxCommandEvent &event);
	void OnCanvasSize(wxCommandEvent &event);

	void UpdateControls();
};

#endif
