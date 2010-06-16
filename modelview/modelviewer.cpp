
#include "modelviewer.h"
#include "globalvars.h"
#include "mpq.h"

// default colour values
const static float def_ambience[4] = {1.0f, 1.0f, 1.0f, 1.0f};
const static float def_diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
const static float def_emission[4] = {0.0f, 0.0f, 0.0f, 1.0f};
const static float def_specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

//test comment ~ZT

// Class event handler/importer
IMPLEMENT_CLASS(ModelViewer, wxFrame)

BEGIN_EVENT_TABLE(ModelViewer, wxFrame)
	EVT_CLOSE(ModelViewer::OnClose)
	//EVT_SIZE(ModelViewer::OnSize)

	// File menu
	EVT_MENU(ID_VIEW_NPC, ModelViewer::OnCharToggle)
	EVT_MENU(ID_VIEW_ITEM, ModelViewer::OnCharToggle)
	EVT_MENU(ID_FILE_SCREENSHOT, ModelViewer::OnSave)
	EVT_MENU(ID_FILE_SCREENSHOTCONFIG, ModelViewer::OnSave)
	EVT_MENU(ID_FILE_EXPORTGIF, ModelViewer::OnSave)
	EVT_MENU(ID_FILE_EXPORTAVI, ModelViewer::OnSave)
	// --
	//EVT_MENU(ID_FILE_TEXIMPORT, ModelViewer::OnTex)
	EVT_MENU(ID_FILE_TEXEXPORT, ModelViewer::OnExport)
	EVT_MENU(ID_FILE_MODELEXPORT_MENU, ModelViewer::OnExport)
	EVT_MENU(ID_FILE_MODEL_INFO, ModelViewer::OnExportOther)
	EVT_MENU(ID_FILE_DISCOVERY_ITEM, ModelViewer::OnExportOther)
	EVT_MENU(ID_FILE_DISCOVERY_NPC, ModelViewer::OnExportOther)
	//--
	// Export Menu
	// To add your new exporter, simply copy the bottom line, and add your unique ID (specified in enums.h) as seen below.
	// Make sure to use this ID for your export command in the ModelViewer::OnExport function!
	EVT_MENU(ID_MODELEXPORT_BASE, ModelViewer::OnExport)
	EVT_MENU(ID_MODELEXPORT_OPTIONS, ModelViewer::OnToggleDock)
	EVT_MENU(ID_MODELEXPORT_INIT, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_MODELEXPORT_LWO, ModelViewer::OnExport)
	EVT_MENU(ID_MODELEXPORT_LWO2, ModelViewer::OnExport)
	EVT_MENU(ID_MODELEXPORT_OBJ, ModelViewer::OnExport)
	EVT_MENU(ID_MODELEXPORT_COLLADA, ModelViewer::OnExport)
	EVT_MENU(ID_MODELEXPORT_MS3D, ModelViewer::OnExport)
	EVT_MENU(ID_MODELEXPORT_3DS, ModelViewer::OnExport)
	EVT_MENU(ID_MODELEXPORT_X3D, ModelViewer::OnExport)
	EVT_MENU(ID_MODELEXPORT_XHTML, ModelViewer::OnExport)
	EVT_MENU(ID_MODELEXPORT_OGRE, ModelViewer::OnExport)
	// --
	EVT_MENU(ID_FILE_RESETLAYOUT, ModelViewer::OnToggleCommand)
	// --
	EVT_MENU(ID_FILE_EXIT, ModelViewer::OnExit)

	// view menu
	EVT_MENU(ID_SHOW_FILE_LIST, ModelViewer::OnToggleDock)
	EVT_MENU(ID_SHOW_ANIM, ModelViewer::OnToggleDock)
	EVT_MENU(ID_SHOW_CHAR, ModelViewer::OnToggleDock)
	EVT_MENU(ID_SHOW_LIGHT, ModelViewer::OnToggleDock)
	EVT_MENU(ID_SHOW_MODEL, ModelViewer::OnToggleDock)
	EVT_MENU(ID_SHOW_MODELBANK, ModelViewer::OnToggleDock)	
	EVT_MENU(ID_SHOW_MODELOPENED, ModelViewer::OnToggleDock)	
	// --
	EVT_MENU(ID_SHOW_MASK, ModelViewer::OnToggleCommand)
	//EVT_MENU(ID_SHOW_WIREFRAME, ModelViewer::OnToggleCommand)
	//EVT_MENU(ID_SHOW_BONES, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_SHOW_BOUNDS, ModelViewer::OnToggleCommand)
	//EVT_MENU(ID_SHOW_PARTICLES, ModelViewer::OnToggleCommand)

	EVT_MENU(ID_BACKGROUND, ModelViewer::OnBackground)
	EVT_MENU(ID_BG_COLOR, ModelViewer::OnSetColor)
	EVT_MENU(ID_SKYBOX, ModelViewer::OnBackground)
	EVT_MENU(ID_SHOW_GRID, ModelViewer::OnToggleCommand)

	EVT_MENU(ID_USE_CAMERA, ModelViewer::OnToggleCommand)

	// Cam
	EVT_MENU(ID_CAM_FRONT, ModelViewer::OnCamMenu)
	EVT_MENU(ID_CAM_SIDE, ModelViewer::OnCamMenu)
	EVT_MENU(ID_CAM_BACK, ModelViewer::OnCamMenu)
	EVT_MENU(ID_CAM_ISO, ModelViewer::OnCamMenu)

	EVT_MENU(ID_CANVAS120, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVAS512, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVAS640, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVAS800, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVAS1024, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVAS1152, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVAS1280, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVAS1600, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVASW720, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVASW1080, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVASW1200, ModelViewer::OnCanvasSize)

	// hidden hotkeys for zooming
	EVT_MENU(ID_ZOOM_IN, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_ZOOM_OUT, ModelViewer::OnToggleCommand)

	// Light Menu
	EVT_MENU(ID_LT_SAVE, ModelViewer::OnLightMenu)
	EVT_MENU(ID_LT_LOAD, ModelViewer::OnLightMenu)
	//EVT_MENU(ID_LT_COLOR, ModelViewer::OnSetColor)
	EVT_MENU(ID_LT_TRUE, ModelViewer::OnLightMenu)
	EVT_MENU(ID_LT_AMBIENT, ModelViewer::OnLightMenu)
	EVT_MENU(ID_LT_DIRECTIONAL, ModelViewer::OnLightMenu)
	EVT_MENU(ID_LT_MODEL, ModelViewer::OnLightMenu)
	EVT_MENU(ID_LT_DIRECTION, ModelViewer::OnLightMenu)
	
	// Effects
	EVT_MENU(ID_ENCHANTS, ModelViewer::OnEffects)
	EVT_MENU(ID_SPELLS, ModelViewer::OnEffects)
	EVT_MENU(ID_EQCREATURE_R, ModelViewer::OnEffects)
	EVT_MENU(ID_EQCREATURE_L, ModelViewer::OnEffects)
	EVT_MENU(ID_SHADER_DEATH, ModelViewer::OnEffects)
	EVT_MENU(ID_TEST, ModelViewer::OnTest)

	// Options
	EVT_MENU(ID_SAVE_CHAR, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_LOAD_CHAR, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_IMPORT_CHAR, ModelViewer::OnToggleCommand)

#ifndef	WotLK
	EVT_MENU(ID_USE_NPCSKINS, ModelViewer::OnCharToggle)
#endif
	EVT_MENU(ID_DEFAULT_DOODADS, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_USE_ANTIALIAS, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_USE_HWACC, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_USE_ENVMAP, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_SHOW_SETTINGS, ModelViewer::OnToggleDock)

	// char controls:
	EVT_MENU(ID_SAVE_EQUIPMENT, ModelViewer::OnSetEquipment)
	EVT_MENU(ID_LOAD_EQUIPMENT, ModelViewer::OnSetEquipment)
	EVT_MENU(ID_CLEAR_EQUIPMENT, ModelViewer::OnSetEquipment)

	EVT_MENU(ID_LOAD_SET, ModelViewer::OnSetEquipment)
	EVT_MENU(ID_LOAD_START, ModelViewer::OnSetEquipment)
	EVT_MENU(ID_LOAD_NPC_START, ModelViewer::OnSetEquipment)

	EVT_MENU(ID_SHOW_UNDERWEAR, ModelViewer::OnCharToggle)
	EVT_MENU(ID_SHOW_EARS, ModelViewer::OnCharToggle)
	EVT_MENU(ID_SHOW_HAIR, ModelViewer::OnCharToggle)
	EVT_MENU(ID_SHOW_FACIALHAIR, ModelViewer::OnCharToggle)
	EVT_MENU(ID_SHOW_FEET, ModelViewer::OnCharToggle)
	EVT_MENU(ID_SHEATHE, ModelViewer::OnCharToggle)

	EVT_MENU(ID_MOUNT_CHARACTER, ModelViewer::OnMount)
	EVT_MENU(ID_CHAR_RANDOMISE, ModelViewer::OnSetEquipment)

	// About menu
	EVT_MENU(ID_CHECKFORUPDATE, ModelViewer::OnCheckForUpdate)
	EVT_MENU(ID_LANGUAGE, ModelViewer::OnLanguage)
	EVT_MENU(ID_HELP, ModelViewer::OnAbout)
	EVT_MENU(ID_ABOUT, ModelViewer::OnAbout)

	// Hidden menu items
	// Temporary saves
	EVT_MENU(ID_SAVE_TEMP1, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_SAVE_TEMP2, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_SAVE_TEMP3, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_SAVE_TEMP4, ModelViewer::OnToggleCommand)

	// Temp loads
	EVT_MENU(ID_LOAD_TEMP1, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_LOAD_TEMP2, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_LOAD_TEMP3, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_LOAD_TEMP4, ModelViewer::OnToggleCommand)

END_EVENT_TABLE()


ModelViewer::ModelViewer()
{
	// our main class objects
	animControl = NULL;
	canvas = NULL;
	charControl = NULL;
	enchants = NULL;
	lightControl = NULL;
	modelControl = NULL;
	arrowControl = NULL;
	imageControl = NULL;
	settingsControl = NULL;
	modelbankControl = NULL;
	modelOpened = NULL;
	animExporter = NULL;
	fileControl = NULL;
	exportOptionsControl = NULL;

	//wxWidget objects
	menuBar = NULL;
	charMenu = NULL;
	viewMenu = NULL;
	optMenu = NULL;
	lightMenu = NULL;
	exportMenu = NULL;

	isModel = false;
	isWMO = false;
	isChar = false;

	//wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU
	// create our main frame
	if (Create(NULL, wxID_ANY, wxString(APP_TITLE _T(" ") APP_VERSION _T(" ") APP_PLATFORM APP_ISDEBUG), wxDefaultPosition, wxSize(1024, 768), wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN, _T("ModelViewerFrame"))) {
		SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
		SetBackgroundStyle(wxBG_STYLE_CUSTOM);

		InitObjects();  // create our canvas, anim control, character control, etc

		// Show our window
		Show(true);
		// Display the window
		Centre();

		// ------
		// Initialise our main window.
		// Load session settings
		LoadSession();

		// create our menu objects
		InitMenu();

		// GUI and Canvas Stuff
		InitDocking();

		// Are these really needed?
		interfaceManager.Update();
		Refresh();
		Update();

		// load our World of Warcraft mpq archives
		bool initbool = Init();
		if (initbool == false){
			wxString info = _T("Fatal Error: WoW Model Viewer does not support your version of WoW.\nPlease update your World of Warcraft client!");
			wxLogMessage(info);
			wxMessageDialog *dial = new wxMessageDialog(NULL, info, wxT("Version Mismatch"), wxOK | wxICON_ERROR);
			dial->ShowModal();
			Close(true);
			return;
		}

		InitDatabase();
		// --

		// Error check
		if (!initDB) {
			wxMessageBox(_T("Some DBC files could not be loaded.  These files are vital to being able to render models correctly.\nPlease make sure you are loading the 'Locale-xxxx.MPQ' file.\nFile list has been disabled until you are able to correct this problem."), _("DBC Error"));
			fileControl->Disable();
		}

	} else {
		wxLogMessage(_T("Critical Error: Unable to create the main window for the application."));
		Close(true);
	}
}

void ModelViewer::InitMenu()
{
	wxLogMessage(_T("Initiating File Menu.."));
	
	// MENU
	fileMenu = new wxMenu;
	fileMenu->Append(ID_MODELEXPORT_BASE, _("Save File..."));
	fileMenu->Enable(ID_MODELEXPORT_BASE, false);
	fileMenu->Append(ID_FILE_SCREENSHOT, _("Save Screenshot\tF12"));
	fileMenu->Append(ID_FILE_SCREENSHOTCONFIG, _("Save Sized Screenshot\tCTRL+S"));
	fileMenu->Append(ID_FILE_EXPORTGIF, _("GIF/Sequence Export"));
	fileMenu->Append(ID_FILE_EXPORTAVI, _("Export AVI"));
	fileMenu->AppendSeparator();
/*
	fileMenu->Append(ID_FILE_TEXIMPORT, _("Import Texture"));
	fileMenu->Enable(ID_FILE_TEXIMPORT, false);
	fileMenu->Append(ID_FILE_TEXEXPORT, _("Export Texture"));
	fileMenu->Enable(ID_FILE_TEXEXPORT, false);
*/
	fileMenu->Append(ID_SHOW_MODELOPENED, _("Export Textures..."));

	// --== New Model Export Menu! ==--
	//To add your exporter, simply copy the bottom line below, and change the nessicary information.

	exportMenu = new wxMenu;
	exportMenu->AppendCheckItem(ID_MODELEXPORT_INIT, _("Initial Pose Only"));
	exportMenu->Check(ID_MODELEXPORT_INIT, modelExportInitOnly);
	exportMenu->AppendSeparator();
	exportMenu->Append(ID_MODELEXPORT_OPTIONS, _("Export Options..."));
	exportMenu->AppendSeparator();
#ifdef _DEBUG
	exportMenu->Append(ID_MODELEXPORT_LWO2, _("Experimental Lightwave..."));
#endif
	exportMenu->Append(ID_MODELEXPORT_LWO, _("Lightwave..."));
	exportMenu->Append(ID_MODELEXPORT_OBJ, _("Wavefront OBJ..."));
	exportMenu->Append(ID_MODELEXPORT_COLLADA, _("Collada..."));
	exportMenu->Append(ID_MODELEXPORT_MS3D, _("Milkshape..."));
	exportMenu->Append(ID_MODELEXPORT_3DS, _("3DS..."));
	exportMenu->Append(ID_MODELEXPORT_X3D, _("X3D..."));
	exportMenu->Append(ID_MODELEXPORT_XHTML, _("X3D in XHTML..."));
#ifdef _DEBUG
	exportMenu->Append(ID_MODELEXPORT_OGRE, _("Ogre XML..."));
#endif

	// -= Enable/Disable Model Exporters =-
	// If you don't support WMOs or M2 files yet, you can disable export for that in filecontrol.cpp,
	// at the bottom of the FileControl::OnTreeSelect function.
	exportMenu->Enable(ID_MODELEXPORT_COLLADA, false);

	// -= Create Model Export Menu =-
	fileMenu->Append(ID_FILE_MODELEXPORT_MENU, _("Export Model"), exportMenu);

	// --== Continue regular menu ==--
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_FILE_DISCOVERY_ITEM, _("Discovery Item"));
	if (wxFileExists(_T("discoveryitems.csv")))
		fileMenu->Enable(ID_FILE_DISCOVERY_ITEM, false);
	fileMenu->Append(ID_FILE_DISCOVERY_NPC, _("Discovery NPC"));
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_FILE_MODEL_INFO, _("Export ModelInfo.xml"));

	fileMenu->AppendSeparator();
	fileMenu->Append(ID_FILE_RESETLAYOUT, _("Reset Layout"));
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_FILE_EXIT, _("E&xit\tCTRL+X"));

	viewMenu = new wxMenu;
	viewMenu->Append(ID_VIEW_NPC, _("View NPC"));
	viewMenu->Append(ID_VIEW_ITEM, _("View Item"));
	viewMenu->AppendSeparator();
	viewMenu->Append(ID_SHOW_FILE_LIST, _("Show file list"));
	viewMenu->Append(ID_SHOW_ANIM, _("Show animaton control"));
	viewMenu->Append(ID_SHOW_CHAR, _("Show character control"));
	viewMenu->Append(ID_SHOW_LIGHT, _("Show light control"));
	viewMenu->Append(ID_SHOW_MODEL, _("Show model control"));
	viewMenu->Append(ID_SHOW_MODELBANK, _("Show model bank"));
	viewMenu->AppendSeparator();
	if (canvas) {
		viewMenu->Append(ID_BG_COLOR, _("Background Color..."));
		viewMenu->AppendCheckItem(ID_BACKGROUND, _("Load Background\tCTRL+L"));
		viewMenu->Check(ID_BACKGROUND, canvas->drawBackground);
		viewMenu->AppendCheckItem(ID_SKYBOX, _("Skybox"));
		viewMenu->Check(ID_SKYBOX, canvas->drawSky);
		viewMenu->AppendCheckItem(ID_SHOW_GRID, _("Show Grid"));
		viewMenu->Check(ID_SHOW_GRID, canvas->drawGrid);

		viewMenu->AppendCheckItem(ID_SHOW_MASK, _("Show Mask"));
		viewMenu->Check(ID_SHOW_MASK, false);

		viewMenu->AppendSeparator();
	}
	
	

	try {
		
		// Camera Menu
		wxMenu *camMenu = new wxMenu;
		camMenu->AppendCheckItem(ID_USE_CAMERA, _("Use model camera"));
		camMenu->AppendSeparator();
		camMenu->Append(ID_CAM_FRONT, _("Front"));
		camMenu->Append(ID_CAM_BACK, _("Back"));
		camMenu->Append(ID_CAM_SIDE, _("Side"));
		camMenu->Append(ID_CAM_ISO, _("Perspective"));

		viewMenu->Append(ID_CAMERA, _("Camera"), camMenu);
		viewMenu->AppendSeparator();

		wxMenu *setSize = new wxMenu;
		setSize->AppendRadioItem(ID_CANVAS120, _T("120 x 120"));
		setSize->AppendRadioItem(ID_CANVAS512, _T("512 x 512"));
		setSize->AppendRadioItem(ID_CANVAS640, _T("640 x 480"));
		setSize->AppendRadioItem(ID_CANVAS800, _T("800 x 600"));
		setSize->AppendRadioItem(ID_CANVAS1024, _T("1024 x 768"));
		setSize->AppendRadioItem(ID_CANVAS1152, _T("1152 x 864"));
		setSize->AppendRadioItem(ID_CANVAS1280, _T("1280 x 768"));
		setSize->AppendRadioItem(ID_CANVAS1600, _T("1600 x 1200"));
		setSize->AppendRadioItem(ID_CANVASW720, _T("1280 x 720"));
		setSize->AppendRadioItem(ID_CANVASW1080, _T("1920 x 1080"));
		setSize->AppendRadioItem(ID_CANVASW1200, _T("1920 x 1200"));

		viewMenu->Append(ID_CANVASSIZE, _("Set Canvas Size"), setSize);
		
		//lightMenu->Append(ID_LT_COLOR, _("Lighting Color..."));

		lightMenu = new wxMenu;
		lightMenu->Append(ID_LT_SAVE, _("Save Lighting"));
		lightMenu->Append(ID_LT_LOAD, _("Load Lighting"));
		lightMenu->AppendSeparator();
		lightMenu->AppendCheckItem(ID_LT_DIRECTION, _("Render Light Objects"));
		lightMenu->AppendSeparator();
		lightMenu->AppendCheckItem(ID_LT_TRUE, _("Use true lighting"));
		lightMenu->Check(ID_LT_TRUE, false);
		lightMenu->AppendRadioItem(ID_LT_DIRECTIONAL, _("Use dynamic light"));
		lightMenu->Check(ID_LT_DIRECTIONAL, true);
		lightMenu->AppendRadioItem(ID_LT_AMBIENT, _("Use ambient light"));
		lightMenu->AppendRadioItem(ID_LT_MODEL, _("Model lights only"));

		charMenu = new wxMenu;
		charMenu->Append(ID_LOAD_CHAR, _("Load Character\tF8"));
		charMenu->Append(ID_IMPORT_CHAR, _("Import Armory Character"));
		charMenu->Append(ID_SAVE_CHAR, _("Save Character\tF7"));
		charMenu->AppendSeparator();
		charMenu->AppendCheckItem(ID_SHOW_UNDERWEAR, _("Show Underwear"));
		charMenu->Check(ID_SHOW_UNDERWEAR, true);
		charMenu->AppendCheckItem(ID_SHOW_EARS, _("Show Ears\tCTRL+E"));
		charMenu->Check(ID_SHOW_EARS, true);
		charMenu->AppendCheckItem(ID_SHOW_HAIR, _("Show Hair\tCTRL+H"));
		charMenu->Check(ID_SHOW_HAIR, true);
		charMenu->AppendCheckItem(ID_SHOW_FACIALHAIR, _("Show Facial Hair\tCTRL+F"));
		charMenu->Check(ID_SHOW_FACIALHAIR, true);
		charMenu->AppendCheckItem(ID_SHOW_FEET, _("Show Feet"));
		charMenu->Check(ID_SHOW_FEET, false);
		charMenu->AppendCheckItem(ID_SHEATHE, _("Sheathe Weapons\tCTRL+Z"));
		charMenu->Check(ID_SHEATHE, false);
		charMenu->AppendSeparator();
		charMenu->Append(ID_SAVE_EQUIPMENT, _("Save Equipment\tF5"));
		charMenu->Append(ID_LOAD_EQUIPMENT, _("Load Equipment\tF6"));
		charMenu->Append(ID_CLEAR_EQUIPMENT, _("Clear Equipment\tF9"));
		charMenu->AppendSeparator();
		charMenu->Append(ID_LOAD_SET, _("Load Item Set"));
		charMenu->Append(ID_LOAD_START, _("Load Start Outfit"));
		charMenu->Append(ID_LOAD_NPC_START, _("Load NPC Outfit"));
		charMenu->AppendSeparator();
		charMenu->Append(ID_MOUNT_CHARACTER, _("Mount a character..."));
		charMenu->Append(ID_CHAR_RANDOMISE, _("Randomise Character\tF10"));

		// Start out Disabled.
		charMenu->Enable(ID_SAVE_CHAR, false);
		charMenu->Enable(ID_SHOW_UNDERWEAR, false);
		charMenu->Enable(ID_SHOW_EARS, false);
		charMenu->Enable(ID_SHOW_HAIR, false);
		charMenu->Enable(ID_SHOW_FACIALHAIR, false);
		charMenu->Enable(ID_SHOW_FEET, false);
		charMenu->Enable(ID_SHEATHE, false);
		charMenu->Enable(ID_SAVE_EQUIPMENT, false);
		charMenu->Enable(ID_LOAD_EQUIPMENT, false);
		charMenu->Enable(ID_CLEAR_EQUIPMENT, false);
		charMenu->Enable(ID_LOAD_SET, false);
		charMenu->Enable(ID_LOAD_START, false);
		charMenu->Enable(ID_LOAD_NPC_START, false);
		charMenu->Enable(ID_MOUNT_CHARACTER, false);
		charMenu->Enable(ID_CHAR_RANDOMISE, false);

		wxMenu *effectsMenu = new wxMenu;
		effectsMenu->Append(ID_ENCHANTS, _("Apply Enchants"));
		effectsMenu->Append(ID_EQCREATURE_R, _("Creature Right-Hand"));
		effectsMenu->Append(ID_EQCREATURE_L, _("Creature Left-Hand"));
#ifndef	WotLK
		effectsMenu->Append(ID_SPELLS, _("Spell Effects"));
		effectsMenu->Enable(ID_SPELLS, false);
		effectsMenu->Append(ID_SHADER_DEATH, _("Death Effect"));
		effectsMenu->Enable(ID_SHADER_DEATH, false);
		effectsMenu->Append(ID_TEST, _T("TEST"));
#endif

		// Options menu
		optMenu = new wxMenu;
#ifndef	WotLK
		optMenu->AppendCheckItem(ID_USE_NPCSKINS, _("Use npc character skins"));
		optMenu->Check(ID_USE_NPCSKINS, false);
#endif
		optMenu->AppendCheckItem(ID_DEFAULT_DOODADS, _("Always show default doodads in WMOs"));
		optMenu->Check(ID_DEFAULT_DOODADS, true);
		optMenu->AppendSeparator();
		optMenu->Append(ID_MODELEXPORT_OPTIONS, _("Export Options..."));
		optMenu->Append(ID_SHOW_SETTINGS, _("Settings..."));


		wxMenu *aboutMenu = new wxMenu;
		aboutMenu->Append(ID_LANGUAGE, _("Language"));
		aboutMenu->Append(ID_HELP, _("Help"));
		aboutMenu->Enable(ID_HELP, false);
		aboutMenu->Append(ID_ABOUT, _("About"));
		aboutMenu->AppendSeparator();
		aboutMenu->Append(ID_CHECKFORUPDATE, _("Check for Update"));

		menuBar = new wxMenuBar();
		menuBar->Append(fileMenu, _("&File"));
		menuBar->Append(viewMenu, _("&View"));
		menuBar->Append(charMenu, _("&Character"));
		menuBar->Append(lightMenu, _("&Lighting"));
		menuBar->Append(optMenu, _("&Options"));
		menuBar->Append(effectsMenu, _("&Effects"));
		menuBar->Append(aboutMenu, _("&About"));
		SetMenuBar(menuBar);
	} catch(...) {};

	// Disable our "Character" menu, only accessible when a character model is being displayed
	// menuBar->EnableTop(2, false);
	
	// Hotkeys / shortcuts
	wxAcceleratorEntry entries[26];
	entries[0].Set(wxACCEL_NORMAL,  WXK_F5,     ID_SAVE_EQUIPMENT);
	entries[1].Set(wxACCEL_NORMAL,  WXK_F6,     ID_LOAD_EQUIPMENT);
	entries[2].Set(wxACCEL_NORMAL,  WXK_F7,     ID_SAVE_CHAR);
	entries[3].Set(wxACCEL_NORMAL,	WXK_F8,     ID_LOAD_CHAR);
	entries[4].Set(wxACCEL_CTRL,	(int)'b',	ID_SHOW_BOUNDS);
	//entries[5].Set(wxACCEL_NORMAL,	(int)'B',	ID_SHOW_BOUNDS);
	entries[6].Set(wxACCEL_CTRL,	(int)'X',	ID_FILE_EXIT);
	entries[7].Set(wxACCEL_NORMAL,	WXK_F12,	ID_FILE_SCREENSHOT);
	entries[8].Set(wxACCEL_CTRL,	(int)'e',	ID_SHOW_EARS);
	entries[9].Set(wxACCEL_CTRL,	(int)'h',	ID_SHOW_HAIR);
	entries[10].Set(wxACCEL_CTRL, (int)'f',	ID_SHOW_FACIALHAIR);
	entries[11].Set(wxACCEL_CTRL, (int)'z',	ID_SHEATHE);
	entries[12].Set(wxACCEL_CTRL, (int)'l',	ID_BACKGROUND);
	entries[13].Set(wxACCEL_CTRL, (int)'+',		ID_ZOOM_IN);
	entries[14].Set(wxACCEL_CTRL, (int)'-',		ID_ZOOM_OUT);
	entries[15].Set(wxACCEL_CTRL, (int)'s',		ID_FILE_SCREENSHOTCONFIG);
	entries[16].Set(wxACCEL_NORMAL, WXK_F9,		ID_CLEAR_EQUIPMENT);
	entries[17].Set(wxACCEL_NORMAL, WXK_F10,	ID_CHAR_RANDOMISE);

	// Temporary saves
	entries[18].Set(wxACCEL_NORMAL, WXK_F1,		ID_SAVE_TEMP1);
	entries[19].Set(wxACCEL_NORMAL, WXK_F2,		ID_SAVE_TEMP2);
	entries[20].Set(wxACCEL_NORMAL, WXK_F3,		ID_SAVE_TEMP3);
	entries[21].Set(wxACCEL_NORMAL, WXK_F4,		ID_SAVE_TEMP4);

	// Temp loads
	entries[22].Set(wxACCEL_CTRL,	WXK_F1,		ID_LOAD_TEMP1);
	entries[23].Set(wxACCEL_CTRL,	WXK_F2,		ID_LOAD_TEMP2);
	entries[24].Set(wxACCEL_CTRL,	WXK_F3,		ID_LOAD_TEMP3);
	entries[25].Set(wxACCEL_CTRL,	WXK_F4,		ID_LOAD_TEMP4);

	wxAcceleratorTable accel(26, entries);
	this->SetAcceleratorTable(accel);
}

void ModelViewer::InitObjects()
{
	wxLogMessage(_T("Initiating Objects..."));

	fileControl = new FileControl(this, ID_FILELIST_FRAME);

	animControl = new AnimControl(this, ID_ANIM_FRAME);
	charControl = new CharControl(this, ID_CHAR_FRAME);
	lightControl = new LightControl(this, ID_LIGHT_FRAME);
	modelControl = new ModelControl(this, ID_MODEL_FRAME);
	settingsControl = new SettingsControl(this, ID_SETTINGS_FRAME);
	settingsControl->Show(false);
	modelbankControl = new ModelBankControl(this, ID_MODELBANK_FRAME);
	modelOpened = new ModelOpened(this, ID_MODELOPENED_FRAME);
	exportOptionsControl = new ModelExportOptions_Control(this, ID_EXPORTOPTIONS_FRAME);
	exportOptionsControl->Show(false);

	canvas = new ModelCanvas(this);

	if (video.secondPass) {
		canvas->Destroy();
		video.Release();
		canvas = new ModelCanvas(this);
	}

	g_modelViewer = this;
	g_animControl = animControl;
	g_charControl = charControl;
	g_canvas = canvas;

	modelControl->animControl = animControl;
	
	enchants = new EnchantsDialog(this, charControl);

	animExporter = new CAnimationExporter(this, wxID_ANY, _T("Animation Exporter"), wxDefaultPosition, wxSize(350, 220), wxCAPTION|wxSTAY_ON_TOP|wxFRAME_NO_TASKBAR);
}

void ModelViewer::InitDatabase()
{
	wxLogMessage(_T("Initiating Databases..."));
	initDB = true;

	if (!itemdb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Item DB."));
	}
	wxString filename = locales[langID]+SLASH+_T("items.csv");
	if (!wxFile::Exists(filename))
		filename = locales[0]+SLASH+_T("items.csv");
	if (wxFile::Exists(filename)) {
		items.open(filename);
	} else {
		wxLogMessage(_T("Error: Could not find items.csv to load an item list from."));
	}

	if (!skyboxdb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the SkyBox DB."));
	}
	if (!spellitemenchantmentdb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the SkyBox DB."));
	}
	if (!itemvisualsdb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the SkyBox DB."));
	}
	if (!animdb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Animation DB."));
	}
	if (!modeldb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Creatures DB."));
	}
	if (!skindb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the CreatureDisplayInfo DB."));
	}
	if(!hairdb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Hair Geoset DB."));
	}
	if(!chardb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Character DB."));
	}
	if(!racedb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Char Races DB."));
	}
	if(!classdb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Char Classes DB."));
	}
	if(!facialhairdb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Char Facial Hair DB."));
	}
	if(!visualdb.open())
		wxLogMessage(_T("Error: Could not open the ItemVisuals DB."));
	if(!effectdb.open())
		wxLogMessage(_T("Error: Could not open the ItemVisualEffects DB."));
	if(!subclassdb.open())
		wxLogMessage(_T("Error: Could not open the Item Subclasses DB."));
	if(!startdb.open())
		wxLogMessage(_T("Error: Could not open the Start Outfit Sets DB."));
	//if(!helmetdb.open()) return false;
	if(!npcdb.open()) 
		wxLogMessage(_T("Error: Could not open the Start Outfit NPC DB."));
	if(!npctypedb.open())
		wxLogMessage(_T("Error: Could not open the Creature Type DB."));

	if(!itemdisplaydb.open())
		wxLogMessage(_T("Error: Could not open the ItemDisplayInfo DB."));
	else
		items.cleanup(itemdisplaydb);

	if(!setsdb.open())
		wxLogMessage(_T("Error: Could not open the Item Sets DB."));
	else
		setsdb.cleanup(items);

//	char filename[20];
	filename = locales[langID]+SLASH+_T("npcs.csv");
	if(!wxFile::Exists(filename))
		filename = locales[0]+SLASH+_T("npcs.csv");
	if(wxFile::Exists(filename))
		npcs.open(filename);
	else {
		NPCRecord rec("26499,24949,7,Arthas");
		if (rec.model > 0) {
			npcs.npcs.push_back(rec);
		}		
		wxLogMessage(_T("Error: Could not find npcs.csv, unable to create NPC list."));
	}

	if(spelleffectsdb.open())
		GetSpellEffects();
	else
		wxLogMessage(_T("Error: Could not open the SpellVisualEffects DB."));

	wxLogMessage(_T("Finished initiating database files."));
}

void ModelViewer::InitDocking()
{
	wxLogMessage(_T("Initiating GUI Docking."));
	
	// wxAUI stuff
	//interfaceManager.SetFrame(this); 
	interfaceManager.SetManagedWindow(this);

	// OpenGL Canvas
	interfaceManager.AddPane(canvas, wxAuiPaneInfo().
				Name(wxT("canvas")).Caption(_("OpenGL Canvas")).
				CenterPane());
	
	// Tree list control
	interfaceManager.AddPane(fileControl, wxAuiPaneInfo().
				Name(wxT("fileControl")).Caption(_("File List")).
				BestSize(wxSize(170,700)).Left().Layer(2));

	// Animation frame
    interfaceManager.AddPane(animControl, wxAuiPaneInfo().
				Name(wxT("animControl")).Caption(_("Animation")).
				Bottom().Layer(1));

	// Character frame
	interfaceManager.AddPane(charControl, wxAuiPaneInfo().
                Name(wxT("charControl")).Caption(_("Character")).
                BestSize(wxSize(170,700)).Right().Layer(2).Show(isChar));

	// Lighting control
	interfaceManager.AddPane(lightControl, wxAuiPaneInfo().
		Name(wxT("Lighting")).Caption(_("Lighting")).
		FloatingSize(wxSize(170,430)).Float().Fixed().Show(false).
		DestroyOnClose(false)); //.FloatingPosition(GetStartPosition())

	// model control
	interfaceManager.AddPane(modelControl, wxAuiPaneInfo().
		Name(wxT("Models")).Caption(_("Models")).
		FloatingSize(wxSize(160,460)).Float().Show(false).
		DestroyOnClose(false));

	// model bank control
	interfaceManager.AddPane(modelbankControl, wxAuiPaneInfo().
		Name(_T("ModelBank")).Caption(_("ModelBank")).
		FloatingSize(wxSize(300,320)).Float().Fixed().Show(false).
		DestroyOnClose(false));

	// model opened
	interfaceManager.AddPane(modelOpened, wxAuiPaneInfo().
		Name(wxT("ModelOpened")).Caption(_("ModelOpened")).
		FloatingSize(wxSize(700,90)).Float().Fixed().Show(false).
		DestroyOnClose(false));

	// settings frame
	interfaceManager.AddPane(settingsControl, wxAuiPaneInfo().
		Name(wxT("Settings")).Caption(_("Settings")).
		FloatingSize(wxSize(400,440)).Float().TopDockable(false).LeftDockable(false).
		RightDockable(false).BottomDockable(false).Fixed().Show(false));

	interfaceManager.AddPane(exportOptionsControl, wxAuiPaneInfo().
		Name(wxT("ExportOptions")).Caption(_("Model Export Options")).
		FloatingSize(wxSize(400,440)).Float().TopDockable(false).LeftDockable(false).
		RightDockable(false).BottomDockable(false).Fixed().Show(false));

    // tell the manager to "commit" all the changes just made
    //interfaceManager.Update();
}

void ModelViewer::ResetLayout()
{
	interfaceManager.DetachPane(fileControl);
	interfaceManager.DetachPane(animControl);
	interfaceManager.DetachPane(charControl);
	interfaceManager.DetachPane(lightControl);
	interfaceManager.DetachPane(modelControl);
	interfaceManager.DetachPane(settingsControl);
	interfaceManager.DetachPane(exportOptionsControl);
	interfaceManager.DetachPane(canvas);
	
	// OpenGL Canvas
	interfaceManager.AddPane(canvas, wxAuiPaneInfo().
				Name(wxT("canvas")).Caption(_("OpenGL Canvas")).
				CenterPane());
	
	// Tree list control
	interfaceManager.AddPane(fileControl, wxAuiPaneInfo().
				Name(wxT("fileControl")).Caption(_("File List")).
				BestSize(wxSize(170,700)).Left().Layer(2));

	// Animation frame
    interfaceManager.AddPane(animControl, wxAuiPaneInfo().
				Name(wxT("animControl")).Caption(_("Animation")).
				Bottom().Layer(1));

	// Character frame
	interfaceManager.AddPane(charControl, wxAuiPaneInfo().
                Name(wxT("charControl")).Caption(_("Character")).
                BestSize(wxSize(170,700)).Right().Layer(2).Show(isChar));

	interfaceManager.AddPane(lightControl, wxAuiPaneInfo().
		Name(wxT("Lighting")).Caption(_("Lighting")).
		FloatingSize(wxSize(170,430)).Float().Fixed().Show(false).
		DestroyOnClose(false)); //.FloatingPosition(GetStartPosition())

	interfaceManager.AddPane(modelControl, wxAuiPaneInfo().
		Name(wxT("Models")).Caption(_("Models")).
		FloatingSize(wxSize(160,460)).Float().Show(false).
		DestroyOnClose(false));

	interfaceManager.AddPane(settingsControl, wxAuiPaneInfo().
		Name(wxT("Settings")).Caption(_("Settings")).
		FloatingSize(wxSize(400,440)).Float().TopDockable(false).LeftDockable(false).
		RightDockable(false).BottomDockable(false).Show(false));

	interfaceManager.AddPane(exportOptionsControl, wxAuiPaneInfo().
		Name(wxT("ExportOptions")).Caption(_("Model Export Options")).
		FloatingSize(wxSize(400,440)).Float().TopDockable(false).LeftDockable(false).
		RightDockable(false).BottomDockable(false).Show(false));

    // tell the manager to "commit" all the changes just made
    interfaceManager.Update();
}


void ModelViewer::LoadSession()
{
	wxLogMessage(_T("Loading Session settings from: %s\n"), cfgPath.c_str());

	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig(_T("Global"),wxEmptyString, cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

	// Other session settings
	if (canvas) {
		pConfig->SetPath(_T("/Session"));
		double c;
		// Background Colour
		pConfig->Read(_T("bgR"), &c, 71.0/255);
		canvas->vecBGColor.x = c;
		pConfig->Read(_T("bgG"), &c, 95.0/255);
		canvas->vecBGColor.y = c;
		pConfig->Read(_T("bgB"), &c, 121.0/255);
		canvas->vecBGColor.z = c;
		
		// boolean vars
		pConfig->Read(_T("RandomLooks"), &useRandomLooks, true);
		pConfig->Read(_T("HideHelmet"), &bHideHelmet, false);
		pConfig->Read(_T("ShowParticle"), &bShowParticle, true);
		pConfig->Read(_T("ZeroParticle"), &bZeroParticle, true);
		pConfig->Read(_T("KnightEyeGlow"), &bKnightEyeGlow, true);
		pConfig->Read(_T("DBackground"), &canvas->drawBackground, false);
		pConfig->Read(_T("BackgroundImage"), &bgImagePath, wxEmptyString);
		if (!bgImagePath.IsEmpty()) {
			canvas->LoadBackground(bgImagePath);
			//viewMenu->Check(ID_BACKGROUND, canvas->drawBackground);
		}
		

		// model file
		/*wxString modelfn;
		pConfig->Read(_T("Model"), &modelfn);
		if (modelfn) {
			LoadModel(modelfn);
		}*/
	}

	wxDELETE(pConfig);
}

void ModelViewer::SaveSession()
{
	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig(_T("Global"), wxEmptyString, cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);


	// Graphic / Video display settings
	pConfig->SetPath(_T("/Graphics"));
	pConfig->Write(_T("FSAA"), video.curCap.aaSamples);
	pConfig->Write(_T("AccumulationBuffer"), video.curCap.accum);
	pConfig->Write(_T("AlphaBits"), video.curCap.alpha);
	pConfig->Write(_T("ColourBits"), video.curCap.colour);
	pConfig->Write(_T("DoubleBuffer"), video.curCap.doubleBuffer);
	pConfig->Write(_T("HWAcceleration"), video.curCap.hwAcc);
	pConfig->Write(_T("SampleBuffer"), video.curCap.sampleBuffer);
	pConfig->Write(_T("StencilBuffer"), video.curCap.stencil);
	pConfig->Write(_T("ZBuffer"), video.curCap.zBuffer);
	


	pConfig->SetPath(_T("/Session"));
	// Attempt at saving colour values as 3 byte hex - loss of accuracy from float
	//wxString temp(Vec3DToString(canvas->vecBGColor));

	if (canvas) {
		pConfig->Write(_T("bgR"), (double)canvas->vecBGColor.x);
		pConfig->Write(_T("bgG"), (double)canvas->vecBGColor.y);
		pConfig->Write(_T("bgB"), (double)canvas->vecBGColor.z);
		
		// boolean vars
		pConfig->Write(_T("RandomLooks"), useRandomLooks);
		pConfig->Write(_T("HideHelmet"), bHideHelmet);
		pConfig->Write(_T("ShowParticle"), bShowParticle);
		pConfig->Write(_T("ZeroParticle"), bZeroParticle);
		pConfig->Write(_T("KnightEyeGlow"), bKnightEyeGlow);

		pConfig->Write(_T("DBackground"), canvas->drawBackground);
		if (canvas->drawBackground)
			pConfig->Write(_T("BackgroundImage"), bgImagePath);
		else
			pConfig->Write(_T("BackgroundImage"), wxEmptyString);

		// model file
		if (canvas->model)
			pConfig->Write(_T("Model"), wxString(canvas->model->name.c_str(), wxConvUTF8));
	}

	// character details
	// equipment

	wxDELETE(pConfig);
}

void ModelViewer::LoadLayout()
{
	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig(_T("Global"), wxEmptyString, cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

	wxString layout;

	// Get layout data
	pConfig->SetPath(_T("/Session"));
	pConfig->Read(_T("Layout"), &layout);

	// if the layout data exists,  load it.
	if (!layout.IsNull() && !layout.IsEmpty()) {
		if (!interfaceManager.LoadPerspective(layout, false))
			wxLogMessage(_T("Error: Could not load the layout."));
		else {
			// No need to display these windows on startup
			interfaceManager.GetPane(modelControl).Show(false);
			interfaceManager.GetPane(modelOpened).Show(false);
			interfaceManager.GetPane(settingsControl).Show(false);
			interfaceManager.GetPane(exportOptionsControl).Show(false);

			// If character panel is showing,  hide it
			interfaceManager.GetPane(charControl).Show(isChar);

			interfaceManager.Update();

			wxLogMessage(_T("Info: GUI Layout loaded from previous session."));
		}
	}

	wxDELETE(pConfig);
}

void ModelViewer::SaveLayout()
{
	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig(_T("Global"), wxEmptyString, cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

	pConfig->SetPath(_T("/Session"));
	
	// Save GUI layout data
	wxString layout = interfaceManager.SavePerspective();
	pConfig->Write(_T("Layout"), layout);

	wxLogMessage(_T("Info: GUI Layout was saved."));

	wxDELETE(pConfig);
}


void ModelViewer::LoadModel(const wxString fn)
{
	if (!canvas || fn.IsEmpty())
		return;

	isModel = true;

	// check if this is a character model
	isChar = (fn.StartsWith(_T("Char"), false));

	Attachment *modelAtt = NULL;

	if (isChar) {
		modelAtt = canvas->LoadCharModel(fn.fn_str());

		// error check
		if (!modelAtt) {
			wxLogMessage(_T("Error: Failed to load the model - %s"), fn.c_str());
			return;
		}

		canvas->model->modelType = MT_CHAR;

	} else {
		modelAtt = canvas->LoadCharModel(fn.fn_str()); //  change it from LoadModel, don't sure it's right or not.

		// error check
		if (!modelAtt) {
			wxLogMessage(_T("Error: Failed to load the model - %s"), fn.c_str());
			return;
		}

		canvas->model->modelType = MT_NORMAL;
	}

	// Error check,  make sure the model was actually loaded and set to canvas->model
	if (!canvas->model) {
		wxLogMessage(_T("Error: [ModelViewer::LoadModel()]  Model* Canvas::model is null!"));
		return;
	}

	canvas->model->charModelDetails.isChar = isChar;
	
	viewMenu->Enable(ID_USE_CAMERA, canvas->model->hasCamera);
	if (canvas->useCamera && !canvas->model->hasCamera) {
		canvas->useCamera = false;
		viewMenu->Check(ID_USE_CAMERA, false);
	}
	
	// wxAUI
	interfaceManager.GetPane(charControl).Show(isChar);
	if (isChar) {
		charMenu->Check(ID_SHOW_UNDERWEAR, true);
		charMenu->Check(ID_SHOW_EARS, true);
		charMenu->Check(ID_SHOW_HAIR, true);
		charMenu->Check(ID_SHOW_FACIALHAIR, true);

		charMenu->Enable(ID_SAVE_CHAR, true);
		charMenu->Enable(ID_SHOW_UNDERWEAR, true);
		charMenu->Enable(ID_SHOW_EARS, true);
		charMenu->Enable(ID_SHOW_HAIR, true);
		charMenu->Enable(ID_SHOW_FACIALHAIR, true);
		charMenu->Enable(ID_SHOW_FEET, true);
		charMenu->Enable(ID_SHEATHE, true);
		charMenu->Enable(ID_SAVE_EQUIPMENT, true);
		charMenu->Enable(ID_LOAD_EQUIPMENT, true);
		charMenu->Enable(ID_CLEAR_EQUIPMENT, true);
		charMenu->Enable(ID_LOAD_SET, true);
		charMenu->Enable(ID_LOAD_START, true);
		charMenu->Enable(ID_LOAD_NPC_START, true);
		charMenu->Enable(ID_MOUNT_CHARACTER, true);
		charMenu->Enable(ID_CHAR_RANDOMISE, true);

		charControl->UpdateModel(modelAtt);
	} else {
		charControl->charAtt = modelAtt;
		charControl->model = (Model*)modelAtt->model;

		charMenu->Enable(ID_SAVE_CHAR, false);
		charMenu->Enable(ID_SHOW_UNDERWEAR, false);
		charMenu->Enable(ID_SHOW_EARS, false);
		charMenu->Enable(ID_SHOW_HAIR, false);
		charMenu->Enable(ID_SHOW_FACIALHAIR, false);
		charMenu->Enable(ID_SHOW_FEET, false);
		charMenu->Enable(ID_SHEATHE, false);
		charMenu->Enable(ID_SAVE_EQUIPMENT, false);
		charMenu->Enable(ID_LOAD_EQUIPMENT, false);
		charMenu->Enable(ID_CLEAR_EQUIPMENT, false);
		charMenu->Enable(ID_LOAD_SET, false);
		charMenu->Enable(ID_LOAD_START, false);
		charMenu->Enable(ID_LOAD_NPC_START, false);
		charMenu->Enable(ID_MOUNT_CHARACTER, false);
		charMenu->Enable(ID_CHAR_RANDOMISE, false);
	}

	// Update the model control
	modelControl->UpdateModel(modelAtt);
	modelControl->RefreshModel(canvas->root);

	// Update the animations / skins
	animControl->UpdateModel(canvas->model);

	interfaceManager.Update();
}

// Load an NPC model
void ModelViewer::LoadNPC(unsigned int modelid)
{
	canvas->clearAttachments();
	//if (!isChar) // may memory leak
	//	wxDELETE(canvas->model);
	canvas->model = NULL;
	
	isModel = true;
	isChar = false;
	isWMO = false;

	try {
		CreatureSkinDB::Record modelRec = skindb.getBySkinID(modelid);
		int displayID = modelRec.getUInt(CreatureSkinDB::NPCID);

		// if the creature ID ISN'T a "NPC",  then load the creature model and skin it.
		if (displayID == 0) {
			
			unsigned int modelID = modelRec.getUInt(CreatureSkinDB::ModelID);
			CreatureModelDB::Record creatureModelRec = modeldb.getByID(modelID);
			
			wxString name(creatureModelRec.getString(CreatureModelDB::Filename));
			name = name.BeforeLast('.');
			name.Append(_T(".m2"));

			LoadModel(name);
			canvas->model->modelType = MT_NORMAL;

			TextureGroup grp;
			int count = 0;
			for (int i=0; i<TextureGroup::num; i++) {
				wxString skin(modelRec.getString(CreatureSkinDB::Skin + i));
				
				grp.tex[i] = skin.mb_str();
				if (skin.length() > 0)
					count++;
			}
			grp.base = 11;
			grp.count = count;
			if (grp.tex[0].length() > 0) 
				animControl->AddSkin(grp);

		} else {
			isChar = true;
			NPCDB::Record rec = npcdb.getByNPCID(displayID);
			CharRacesDB::Record rec2 = racedb.getById(rec.getUInt(NPCDB::RaceID));
			
			wxString retval = rec.getString(NPCDB::Gender);
			wxString strModel = _T("Character\\");

			if (gameVersion == 30100) {
				if (!retval.IsEmpty()) {
					strModel.append(rec2.getString(CharRacesDB::NameV310));
					strModel.append(_T("\\Female\\"));
					strModel.append(rec2.getString(CharRacesDB::NameV310));
					strModel.append(_T("Female.m2"));
				} else {
					strModel.append(rec2.getString(CharRacesDB::NameV310));
					strModel.append(_T("\\Male\\"));
					strModel.append(rec2.getString(CharRacesDB::NameV310));
					strModel.append(_T("Male.m2"));
				}
			} else {
				if (!retval.IsEmpty()) {
					strModel.append(rec2.getString(CharRacesDB::Name));
					strModel.append(_T("\\Female\\"));
					strModel.append(rec2.getString(CharRacesDB::Name));
					strModel.append(_T("Female.m2"));
				} else {
					strModel.append(rec2.getString(CharRacesDB::Name));
					strModel.append(_T("\\Male\\"));
					strModel.append(rec2.getString(CharRacesDB::Name));
					strModel.append(_T("Male.m2"));
				}
			}
			
			//const char *newName = strModel.c_str();

			Attachment *modelAtt;
			modelAtt = canvas->LoadCharModel(strModel.mb_str());
			canvas->model->modelType = MT_NPC;

			wxString fn(_T("Textures\\Bakednpctextures\\"));
			fn.Append(rec.getString(NPCDB::Filename));
			charControl->UpdateNPCModel(modelAtt, displayID);
			charControl->customSkin = fn;

			charControl->RefreshNPCModel(); // rec.getUInt(NPCDB::NPCID
			charControl->RefreshEquipment();

			menuBar->EnableTop(2, true);
			charMenu->Check(ID_SHOW_UNDERWEAR, true);
			charMenu->Check(ID_SHOW_EARS, true);
			charMenu->Check(ID_SHOW_HAIR, true);
			charMenu->Check(ID_SHOW_FACIALHAIR, true);
			// ---

			animControl->UpdateModel(canvas->model);
			canvas->ResetView();
		}
	} catch (...) {}

	// wxAUI
	interfaceManager.GetPane(charControl).Show(isChar);
	interfaceManager.Update();
}

void ModelViewer::LoadItem(unsigned int displayID)
{
	canvas->clearAttachments();
	//if (!isChar) // may memory leak
	//	wxDELETE(canvas->model);
	canvas->model = NULL;
	
	isModel = true;
	isChar = false;
	isWMO = false;

	try {
		ItemDisplayDB::Record modelRec = itemdisplaydb.getById(displayID);
		wxString name = modelRec.getString(ItemDisplayDB::Model);
		name = name.BeforeLast('.');
		name.Append(_T(".M2"));
		//wxLogMessage(_T("LoadItem %d %s"), displayID, name.c_str());

		wxString fns[] = { _T("Item\\ObjectComponents\\Head\\"),
			_T("Item\\ObjectComponents\\Shoulder\\"),
			_T("Item\\ObjectComponents\\Quiver\\"),
			_T("Item\\ObjectComponents\\Shield\\"),
			_T("Item\\ObjectComponents\\Weapon\\") };
		wxString fn;
		for(int i=0; i<5; i++) {
			fn = fns[i]+name;
			if (MPQFile::getSize(fn.fn_str()) > 0) {
				LoadModel(fn);
				break;
			}
		}
		charMenu->Enable(ID_SAVE_CHAR, false);
		charMenu->Enable(ID_SHOW_UNDERWEAR, false);
		charMenu->Enable(ID_SHOW_EARS, false);
		charMenu->Enable(ID_SHOW_HAIR, false);
		charMenu->Enable(ID_SHOW_FACIALHAIR, false);
		charMenu->Enable(ID_SHOW_FEET, false);
		charMenu->Enable(ID_SHEATHE, false);
		charMenu->Enable(ID_SAVE_EQUIPMENT, false);
		charMenu->Enable(ID_LOAD_EQUIPMENT, false);
		charMenu->Enable(ID_CLEAR_EQUIPMENT, false);
		charMenu->Enable(ID_LOAD_SET, false);
		charMenu->Enable(ID_LOAD_START, false);
		charMenu->Enable(ID_LOAD_NPC_START, false);
		charMenu->Enable(ID_MOUNT_CHARACTER, false);
		charMenu->Enable(ID_CHAR_RANDOMISE, false);

	} catch (...) {}

	// wxAUI
	interfaceManager.GetPane(charControl).Show(isChar);
	interfaceManager.Update();
}

// This is called when the user goes to File->Exit
void ModelViewer::OnExit(wxCommandEvent &event)
{
	if (event.GetId() == ID_FILE_EXIT) {
		video.render = false;
		//canvas->timer.Stop();
		canvas->Disable();
		Close(false);
	}
}

// This is called when the window is closing
void ModelViewer::OnClose(wxCloseEvent &event)
{
	Destroy();
}

// Called when the window is resized, minimised, etc
void ModelViewer::OnSize(wxSizeEvent &event)
{
	/* // wxIFM stuff
	if(!interfaceManager)
		event.Skip();
	else
        interfaceManager->Update(IFM_DEFAULT_RECT);
	*/

	// wxAUI
	//interfaceManager.Update(); // causes an error?
}

ModelViewer::~ModelViewer()
{
	wxLogMessage(_T("Shuting down the program...\n"));

	video.render = false;

	// If we have a canvas (which we always should)
	// Stop rendering, give more power back to the CPU to close this sucker down!
	//if (canvas)
	//	canvas->timer.Stop();

	// Save current layout
	SaveLayout();

	// wxAUI stuff
	interfaceManager.UnInit();

	// Clear the MPQ archives.
	for (std::vector<MPQArchive*>::iterator it = archives.begin(); it != archives.end(); ++it) {
        (*it)->close();
		//archives.erase(it);
	}
	archives.clear();

	// Save our session and layout info
	SaveSession();

	if (animExporter) {
		animExporter->Destroy();
		wxDELETE(animExporter);
	}

	if (canvas) {
		canvas->Disable();
		canvas->Destroy(); 
		canvas = NULL;
	}
	
	if (fileControl) {
		fileControl->Destroy();
		fileControl = NULL;
	}

	if (animControl) {
		animControl->Destroy();
		animControl = NULL;
	}

	if (charControl) {
		charControl->Destroy();
		charControl = NULL;
	}

	if (lightControl) {
		lightControl->Destroy();
		lightControl = NULL;
	}

	if (settingsControl) {
		settingsControl->Destroy();
		settingsControl = NULL;
	}

	if (exportOptionsControl) {
		exportOptionsControl->Destroy();
		exportOptionsControl = NULL;
	}
	
	if (modelControl) {
		modelControl->Destroy();
		modelControl = NULL;
	}

	if (modelOpened) {
		modelOpened->Destroy();
		modelOpened = NULL;
	}

	if (enchants) {
		enchants->Destroy();
		enchants = NULL;
	}
}

bool ModelViewer::InitMPQArchives()
{
	wxString path;

	for (size_t i=0; i<mpqArchives.GetCount(); i++) {
		if (wxFileName::FileExists(mpqArchives[i]))
			archives.push_back(new MPQArchive(mpqArchives[i].mb_str()));
	}

	// Checks and logs the "TOC" version of the interface files that were loaded
	MPQFile f("Interface\\FrameXML\\FrameXML.TOC");
	if (f.isEof()) {
		f.close();
		wxLogMessage(_T("Unable to gather TOC data."));
		return false;
	}
	f.seek(51); // offset to "## Interface: "
	unsigned char toc[6];
	memset(toc,'\0', 6);
	f.read(toc, 5);
	f.close();
	wxLogMessage(_T("Loaded Content TOC: v%c.%c%c.%c%c"), toc[0], toc[1], toc[2], toc[3], toc[4]);

	wxString info = _T("Notice: WMV is designed for WoW 3.3.x, but your version is supported.\nYou may experience diminished capacity while working with WoW Model Viewer.\nYou should update your World of Warcraft client soon.");
	// If we support more than 1 TOC version, place the others here.
	if (strncmp((char*)toc, "30100", 5) == 0){
		if (gameVersion != 30100){
			wxMessageBox(info,_T("Compatible Version Found."),wxOK);
			gameVersion = 30100;
		}
	}else if (strncmp((char*)toc, "30200", 5) == 0){
		wxLogMessage(info);
		if (gameVersion != 30200){
			wxMessageBox(info,_T("Compatible Version Found."),wxOK);
			gameVersion = 30200;
		}
	}else if (strncmp((char*)toc, "40000", 5) == 0) {
		wxLogMessage(info);
		if (gameVersion != 40000){
			wxString info = _T("Notice: Cataclysm is still under development.\nMissing or incorrect data may not be a WMV issue.");
			wxMessageBox(info,_T("Cataclysm Detected!"),wxOK);
			gameVersion = 40000;
		}
		langOffset = 0;
	// else if not our primary supported edition...
	}else if (strncmp((char*)toc, "30300", 5) != 0){
		wxString info = _T("Notice: WoW Model Viewer does not support your version of WoW.\nPlease update your World of Warcraft client!");
		wxLogMessage(info);

		return false;
	}else{
		gameVersion = 30300;
	}

	const char *component = "component.wow-data.txt";
	MPQFile f2(component);
	if (!f2.isEof()) {
		f2.save(component);
		f2.close();
		
		wxXmlDocument xmlDoc;
		if (xmlDoc.Load(wxString(component, wxConvUTF8), _T("UTF-8"))) {
			wxXmlNode *child = xmlDoc.GetRoot()->GetChildren(); // componentinfo->component.version
			if (child && child->GetName() == _T("component")) {
				wxString version = child->GetPropVal(_T("version"), _T("0"));
				if (version != _T("0")) {
					wxLogMessage(_T("Loaded Content Version: %s"), version.c_str());
				}
			}
		}
		
		wxRemoveFile(wxString(component, wxConvUTF8));
	}
	return true;
}

bool ModelViewer::Init()
{
	/*
	// Set our display mode	
	//if (video.GetCompatibleWinMode(video.curCap)) {
		video.SetMode();
		if (!video.render) // Something bad must of happened - find a new working display mode
			video.GetAvailableMode();
	/*
	} else {
		wxLogMessage(_T("Error: Failed to find a compatible graphics mode.  Finding first available display mode..."));
		video.GetAvailableMode(); // Get first available display mode that supports the current desktop colour bitdepth
	}
	*/
	
	wxLogMessage(_T("Setting OpenGL render state..."));
	video.InitGL();

	// Initiate other stuff
	wxLogMessage(_T("Initiating Archives...\n"));

	// more detail logging, this is so when someone has a problem and they send their log info
	wxLogMessage(_T("Game Data Path: %s"), wxString(gamePath.fn_str(), wxConvUTF8).c_str());
	wxLogMessage(_T("Use Local Files: %s\n"), useLocalFiles ? _T("true") : _T("false"));
	
	isChar = false;
	isModel = false;
	bool mpqarch = true;

	// Load the games MPQ files into memory
	mpqarch = InitMPQArchives();
	wxLogMessage(_T("InitMPQArchives result: %s"),mpqarch ? _T("true") : _T("false"));

	if (mpqarch == false){
		return false;
	}

	fileControl->Init(this);

	charControl->Init();

	return true;
}

// Menu button press events
void ModelViewer::OnToggleDock(wxCommandEvent &event)
{
	int id = event.GetId();

	// wxAUI Stuff
	if (id==ID_SHOW_FILE_LIST)
		interfaceManager.GetPane(fileControl).Show(true);
	else if (id==ID_SHOW_ANIM)
		interfaceManager.GetPane(animControl).Show(true);
	else if (id==ID_SHOW_CHAR && isChar)
		interfaceManager.GetPane(charControl).Show(true);
	else if (id==ID_SHOW_LIGHT)
		interfaceManager.GetPane(lightControl).Show(true);
	else if (id==ID_SHOW_MODEL)
		interfaceManager.GetPane(modelControl).Show(true);
	else if (id==ID_SHOW_SETTINGS) {
		interfaceManager.GetPane(settingsControl).Show(true);
		settingsControl->Open();
	} else if (id==ID_SHOW_MODELBANK) {
		interfaceManager.GetPane(modelbankControl).Show(true);
	} else if (id==ID_SHOW_MODELOPENED) {
		interfaceManager.GetPane(modelOpened).Show(true);
	}else if(id==ID_MODELEXPORT_OPTIONS){
		interfaceManager.GetPane(exportOptionsControl).Show(true);
		exportOptionsControl->Open();
	}
	interfaceManager.Update();
}

// Menu button press events
void ModelViewer::OnToggleCommand(wxCommandEvent &event)
{
	int id = event.GetId();

	//switch 
	switch(id) {
	case ID_FILE_RESETLAYOUT:
		ResetLayout();
		break;

	/*
	case ID_USE_ANTIALIAS:
		useAntiAlias = event.IsChecked();
		break;


	case ID_USE_HWACC:
		if (event.IsChecked() == true)
			disableHWAcc = false;
		else
			disableHWAcc = true;
		break;
	*/

	case ID_USE_ENVMAP:
		video.useEnvMapping = event.IsChecked();
		break;

	case ID_SHOW_MASK:
		video.useMasking = !video.useMasking;

	case ID_SHOW_BOUNDS:
		canvas->model->showBounds = !canvas->model->showBounds;
		break;

	case ID_SHOW_GRID:
		canvas->drawGrid = event.IsChecked();
		break;

	case ID_USE_CAMERA:
		canvas->useCamera = event.IsChecked();
		break;

	case ID_DEFAULT_DOODADS:
		// if we have a model...
		if (canvas->wmo) {
			canvas->wmo->includeDefaultDoodads = event.IsChecked();
			canvas->wmo->updateModels();
		}
		animControl->defaultDoodads = event.IsChecked();
		break;

	case ID_SAVE_CHAR:
		{
			wxFileDialog saveDialog(this, _("Save character"), wxEmptyString, wxEmptyString, _T("Character files (*.chr)|*.chr"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (saveDialog.ShowModal()==wxID_OK) {
				SaveChar(saveDialog.GetPath().mb_str());
			}
		}
		break;
	case ID_LOAD_CHAR:
		{
			wxFileDialog loadDialog(this, _("Load character"), wxEmptyString, wxEmptyString, _T("Character files (*.chr)|*.chr"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
			if (loadDialog.ShowModal()==wxID_OK) {
				for (int i=0; i<NUM_CHAR_SLOTS; i++)
					charControl->cd.equipment[i] = 0;
				
				LoadChar(loadDialog.GetPath().mb_str());
			}
		}
		break;

	case ID_IMPORT_CHAR:
		{
			wxTextEntryDialog dialog(this, _T("Please paste in the URL to the character you wish to import."), _T("Please enter text"), wxEmptyString, wxOK | wxCANCEL | wxCENTRE, wxDefaultPosition);
			if (dialog.ShowModal() == wxID_OK)
				ImportArmoury(dialog.GetValue());
		}
		break;

	case ID_ZOOM_IN:
		canvas->Zoom(0.5f, false);
		break;

	case ID_ZOOM_OUT:
		canvas->Zoom(-0.5f, false);
		break;

	case ID_SAVE_TEMP1:
		canvas->SaveSceneState(1);
		break;
	case ID_SAVE_TEMP2:
		canvas->SaveSceneState(2);
		break;
	case ID_SAVE_TEMP3:
		canvas->SaveSceneState(3);
		break;
	case ID_SAVE_TEMP4:
		canvas->SaveSceneState(4);
		break;
	case ID_LOAD_TEMP1:
		canvas->LoadSceneState(1);
		break;
	case ID_LOAD_TEMP2:
		canvas->LoadSceneState(2);
		break;
	case ID_LOAD_TEMP3:
		canvas->LoadSceneState(3);
		break;
	case ID_LOAD_TEMP4:
		canvas->LoadSceneState(4);
		break;
	case ID_MODELEXPORT_INIT:
		modelExportInitOnly = exportMenu->IsChecked(ID_MODELEXPORT_INIT);
		break;
	}
}

void ModelViewer::OnLightMenu(wxCommandEvent &event)
{
	int id = event.GetId();

	switch (id) {
		case ID_LT_SAVE:
		{
			wxFileDialog dialog(this, _("Save Lighting"), wxEmptyString, wxEmptyString, _T("Scene Lighting (*.lit)|*.lit"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			
			if (dialog.ShowModal()==wxID_OK) {
				wxString fn = dialog.GetFilename();

				ofstream f(fn.fn_str(), ios_base::out|ios_base::trunc);
				f << lightMenu->IsChecked(ID_LT_DIRECTION) << " " << lightMenu->IsChecked(ID_LT_TRUE) << " " << lightMenu->IsChecked(ID_LT_DIRECTIONAL) << " " << lightMenu->IsChecked(ID_LT_AMBIENT) << " " << lightMenu->IsChecked(ID_LT_MODEL) << endl;
				for (int i=0; i<MAX_LIGHTS; i++) {
					f << lightControl->lights[i].ambience.x << " " << lightControl->lights[i].ambience.y << " " << lightControl->lights[i].ambience.z << " " << lightControl->lights[i].arc << " " << lightControl->lights[i].constant_int << " " << lightControl->lights[i].diffuse.x << " " << lightControl->lights[i].diffuse.y << " " << lightControl->lights[i].diffuse.z << " " << lightControl->lights[i].enabled << " " << lightControl->lights[i].linear_int << " " << lightControl->lights[i].pos.x << " " << lightControl->lights[i].pos.y << " " << lightControl->lights[i].pos.z << " " << lightControl->lights[i].quadradic_int << " " << lightControl->lights[i].relative << " " << lightControl->lights[i].specular.x << " " << lightControl->lights[i].specular.y << " " << lightControl->lights[i].specular.z << " " << lightControl->lights[i].target.x << " " << lightControl->lights[i].target.y << " " << lightControl->lights[i].target.z << " " << lightControl->lights[i].type << endl;
				}
				f.close();
			}

			return;

		} case ID_LT_LOAD: {
			wxFileDialog dialog(this, _("Load Lighting"), wxEmptyString, wxEmptyString, _T("Scene Lighting (*.lit)|*.lit"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
			
			if (dialog.ShowModal()==wxID_OK) {
				wxString fn = dialog.GetFilename();
				ifstream f(fn.fn_str());
				
				bool lightObj, lightTrue, lightDir, lightAmb, lightModel;

				//lightMenu->IsChecked(ID_LT_AMBIENT)
				f >> lightObj >> lightTrue >> lightDir >> lightAmb >> lightModel;

				lightMenu->Check(ID_LT_DIRECTION, lightObj);
				lightMenu->Check(ID_LT_TRUE, lightTrue);
				lightMenu->Check(ID_LT_DIRECTIONAL, lightDir);
				lightMenu->Check(ID_LT_AMBIENT, lightAmb);
				lightMenu->Check(ID_LT_MODEL, lightModel);

				for (int i=0; i<MAX_LIGHTS; i++) {
					f >> lightControl->lights[i].ambience.x >> lightControl->lights[i].ambience.y >> lightControl->lights[i].ambience.z >> lightControl->lights[i].arc >> lightControl->lights[i].constant_int >> lightControl->lights[i].diffuse.x >> lightControl->lights[i].diffuse.y >> lightControl->lights[i].diffuse.z >> lightControl->lights[i].enabled >> lightControl->lights[i].linear_int >> lightControl->lights[i].pos.x >> lightControl->lights[i].pos.y >> lightControl->lights[i].pos.z >> lightControl->lights[i].quadradic_int >> lightControl->lights[i].relative >> lightControl->lights[i].specular.x >> lightControl->lights[i].specular.y >> lightControl->lights[i].specular.z >> lightControl->lights[i].target.x >> lightControl->lights[i].target.y >> lightControl->lights[i].target.z >> lightControl->lights[i].type;
				}
				f.close();

				if (lightObj) 
					canvas->drawLightDir = true;

				if (lightDir) {
					canvas->lightType = LIGHT_DYNAMIC; //LT_DIRECTIONAL;
					
					/*
					if (lightTrue) {
						if (event.IsChecked()){
							// Need to reset all our colour, lighting, material back to 'default'
							//GLfloat b[] = {0.5f, 0.4f, 0.4f, 1.0f};
							//glColor4fv(b);
							glDisable(GL_COLOR_MATERIAL);

							glMaterialfv(GL_FRONT, GL_EMISSION, def_emission);
							
							glMaterialfv(GL_FRONT, GL_AMBIENT, def_ambience);
							//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, def_ambience);
							
							glMaterialfv(GL_FRONT, GL_DIFFUSE, def_diffuse);
							glMaterialfv(GL_FRONT, GL_SPECULAR, def_specular);
						} else {
							glEnable(GL_COLOR_MATERIAL);
						}
					}
					*/
				} else if (lightAmb) {
					//glEnable(GL_COLOR_MATERIAL);
					canvas->lightType = LIGHT_AMBIENT;
				} else if (lightModel) {
					canvas->lightType = LIGHT_MODEL_ONLY;
				}

				lightControl->UpdateGL();
				lightControl->Update();
			}
			
			return;
		}
		/* case ID_USE_LIGHTS:
			canvas->useLights = event.IsChecked();
			return;
		*/
		case ID_LT_DIRECTION:
			canvas->drawLightDir = event.IsChecked();
			return;
		case ID_LT_TRUE:
			if (event.IsChecked()){
				// Need to reset all our colour, lighting, material back to 'default'
				//GLfloat b[] = {0.5f, 0.4f, 0.4f, 1.0f};
				//glColor4fv(b);
				glDisable(GL_COLOR_MATERIAL);

				glMaterialfv(GL_FRONT, GL_EMISSION, def_emission);
				glMaterialfv(GL_FRONT, GL_AMBIENT, def_ambience);
				//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, def_ambience);
				
				glMaterialfv(GL_FRONT, GL_DIFFUSE, def_diffuse);
				glMaterialfv(GL_FRONT, GL_SPECULAR, def_specular);				
			} else {
				glEnable(GL_COLOR_MATERIAL);
				//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Vec4D(0.4f,0.4f,0.4f,1.0f));
			}
			
			lightControl->Update();

			return;

		// Ambient lighting
		case ID_LT_AMBIENT:
			//glEnable(GL_COLOR_MATERIAL);
			canvas->lightType = LIGHT_AMBIENT;
			return;

		// Dynamic lighting
		case ID_LT_DIRECTIONAL:
			//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, def_ambience);
			canvas->lightType = LIGHT_DYNAMIC;
			return;

		// Model's ambient lighting
		case ID_LT_MODEL:
			canvas->lightType = LIGHT_MODEL_ONLY;
			return;
	}
}

void ModelViewer::OnCamMenu(wxCommandEvent &event)
{
	int id = event.GetId();
	if (id==ID_CAM_FRONT)
		canvas->model->rot.y = -90.0f;
	else if (id==ID_CAM_BACK)
		canvas->model->rot.y = 90.0f;
	else if (id==ID_CAM_SIDE)
		canvas->model->rot.y = 0.0f;
	else if (id==ID_CAM_ISO) {
		canvas->model->rot.y = -40.0f;
		canvas->model->rot.x = 20.0f;
	}

	//viewControl->Update();	
}

// Menu button press events
void ModelViewer::OnSetColor(wxCommandEvent &event)
{
	int id = event.GetId();
	if (id==ID_BG_COLOR) {
		canvas->vecBGColor = DoSetColor(canvas->vecBGColor);
		canvas->drawBackground = false;
	//} else if (id==ID_LT_COLOR) {
	//	canvas->ltColor = DoSetColor(canvas->ltColor);
	}
}

// Menu button press events
void ModelViewer::OnEffects(wxCommandEvent &event)
{
	int id = event.GetId();

	if (id == ID_ENCHANTS) {
		// Currently, only support enchanting character weapons
		if (isChar)
			enchants->Display();

	} else if (id == ID_SPELLS) {
		wxSingleChoiceDialog spellsDialog(this, _T("Choose"), _T("Select a Spell Effect"), spelleffects);

		if (spellsDialog.ShowModal() == wxID_OK) {
			// TODO: Finish adding support for spells.
		}

	} else if (id == ID_EQCREATURE_R) { // R for righthand
		Model *m = static_cast<Model*>(canvas->root->model);
		if (!m)
			m = static_cast<Model*>(canvas->model);

		// make sure m is a valid pointer to a model
		if (m) {
			// This is an error check to make sure the creature can be equipped.
			for(int k=0; k<m->ATT_MAX; k++){
				if (m->attLookup[k] == PS_RIGHT_PALM) {
					SelectCreatureItem(CS_HAND_RIGHT, 0, charControl, canvas);
					break;
				}
			}
		}

	} else if (id == ID_EQCREATURE_L) { // L for lefthand
		Model *m = static_cast<Model*>(canvas->root->model);
		if (!m)
			m = static_cast<Model*>(canvas->model);

		// make sure m is a valid pointer to a model
		if (m) {
			// This is an error check to make sure the creature can be equipped.
			for(int k=0; k<m->ATT_MAX; k++){
				if (m->attLookup[k] == PS_LEFT_PALM) {
					SelectCreatureItem(CS_HAND_LEFT, 0, charControl, canvas);
					break;
				}
			}
		}
	} else if (id==ID_SHADER_DEATH) {
		//Shader test("Shaders\\Pixel\\FFXDeath.bls");
		//test.Load();
	}
}

Vec3D ModelViewer::DoSetColor(const Vec3D &defColor)
{
	wxColourData data;
	wxColour dcol((unsigned char)(defColor.x*255.0f), (unsigned char)(defColor.y*255.0f), (unsigned char)(defColor.z*255.0f));
	data.SetChooseFull(true);
	data.SetColour(dcol);
	   
	wxColourDialog dialog(this, &data);

	if (dialog.ShowModal() == wxID_OK) {
		wxColour col = dialog.GetColourData().GetColour();
		return Vec3D(col.Red()/255.0f, col.Green()/255.0f, col.Blue()/255.0f);
	}
	return defColor;
}

void ModelViewer::OnSetEquipment(wxCommandEvent &event)
{
	if (isChar) 
		charControl->OnButton(event);

	UpdateControls();
}

void ModelViewer::OnCharToggle(wxCommandEvent &event)
{
	if (event.GetId() == ID_VIEW_NPC)
		charControl->selectNPC(UPDATE_NPC);
	if (event.GetId() == ID_VIEW_ITEM)
		charControl->selectItem(UPDATE_SINGLE_ITEM, -1, -1);
	else if (isChar) 
		charControl->OnCheck(event);
	
}

void ModelViewer::OnMount(wxCommandEvent &event)
{
	/*
	const unsigned int mountSlot = 0;

	// check if it's mountable
	if (!canvas->viewingModel) return;
	Model *root = (Model*)canvas->root->model;
	if (!root) return;
	if (root->name.substr(0,8)!="Creature") return;
	bool mountable = (root->header.nAttachLookup > mountSlot) && (root->attLookup[mountSlot]!=-1);
	if (!mountable) return;

	std::string fn = charControl->selectCharModel();
	if (fn.length()==0) return;

	canvas->root->delChildren();
	Attachment *att = canvas->root->addChild(fn.c_str(), mountSlot, -1);

	wxHostInfo hi;
	hi = layoutManager->GetDockHost(wxDEFAULT_RIGHT_HOST);
	if (!charControlDockWindow->IsDocked()) {
		layoutManager->DockWindow(charControlDockWindow, hi);
		charControlDockWindow->Show(TRUE);
	}
	charMenu->Check(ID_SHOW_UNDERWEAR, true);
	charMenu->Check(ID_SHOW_EARS, true);
	charMenu->Check(ID_SHOW_HAIR, true);
	charMenu->Check(ID_SHOW_FACIALHAIR, true);

	Model *m = (Model*)att->model;
	charControl->UpdateModel(att);

	menuBar->EnableTop(2, true);
	isChar = true;

	// find a Mount animation (id = 91, let's hope this doesn't change)
	for (size_t i=0; i<m->header.nAnimations; i++) {
		if (m->anims[i].animID == 91) {
			m->currentAnim = (int)i;
			break;
		}
	}
	*/

	charControl->selectMount();
}

void ModelViewer::OnSave(wxCommandEvent &event)
{
	static wxFileName dir = cfgPath;
		
	if (!canvas || (!canvas->model && !canvas->wmo))
		return;

	if (event.GetId() == ID_FILE_SCREENSHOT) {
		wxString tmp = _T("screenshot_");
		tmp << ssCounter;
		wxFileDialog dialog(this, _("Save screenshot"), dir.GetPath(wxPATH_GET_VOLUME), tmp, _T("Bitmap Images (*.bmp)|*.bmp|TGA Images (*.tga)|*.tga|JPEG Images (*.jpg)|*.jpg|PNG Images (*.png)|*.png"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
		dialog.SetFilterIndex(imgFormat);

		if (dialog.ShowModal()==wxID_OK) {
			imgFormat = dialog.GetFilterIndex();
			tmp = dialog.GetPath();
			dialog.Show(false);
			canvas->Screenshot(tmp);
			dir.SetPath(tmp);
			ssCounter++;
		}

		//canvas->InitView();

	} else if (event.GetId() == ID_FILE_EXPORTGIF) {
		if (canvas->wmo)
			return;

		if (!canvas->model)
			return;

		if (!video.supportFBO && !video.supportPBO) {
			wxMessageBox(_T("This function is currently disabled for video cards that don't\nsupport the FrameBufferObject or PixelBufferObject OpenGL extensions"), _T("Error"));
			return;
		}
		
		wxFileDialog dialog(this, _T("Save Animation"), dir.GetPath(wxPATH_GET_VOLUME), _T("filename"), _T("Animation"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
		
		if (dialog.ShowModal()==wxID_OK) {
			// Save the folder location for next time
			dir.SetPath(dialog.GetPath());

			// Show our exporter window			
			animExporter->Init(dialog.GetPath());
			animExporter->Show(true);
		}

	} else if (event.GetId() == ID_FILE_EXPORTAVI) {
		if (canvas->wmo && !canvas->model)
			return;

		if (!video.supportFBO && !video.supportPBO) {
			wxMessageBox(_T("This function is currently disabled for video cards that don't\nsupport the FrameBufferObject or PixelBufferObject OpenGL extensions"), _T("Error"));
			return;
		}
		
		wxFileDialog dialog(this, _T("Save AVI"), dir.GetPath(wxPATH_GET_VOLUME), _T("animation.avi"), _T("animation (*.avi)|*.avi"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
		
		if (dialog.ShowModal()==wxID_OK) {
			animExporter->CreateAvi(dialog.GetPath());
		}

	} else if (event.GetId() == ID_FILE_SCREENSHOTCONFIG) {
		if (!imageControl) {
			imageControl = new ImageControl(this, ID_IMAGE_FRAME, canvas);

			interfaceManager.AddPane(imageControl, wxAuiPaneInfo().
				Name(wxT("Screenshot")).Caption(_("Screenshot")).
				FloatingSize(wxSize(295,145)).Float().Fixed().
				Dockable(false)); //.FloatingPosition(GetStartPosition())
		}

		imageControl->OnShow(&interfaceManager);
	}
}

void ModelViewer::OnBackground(wxCommandEvent &event)
{
	static wxFileName dir = cfgPath;
	
	int id = event.GetId();

	if (id == ID_BACKGROUND) {
		if (event.IsChecked()) {
			wxFileDialog dialog(this, _("Load Background"), dir.GetPath(wxPATH_GET_VOLUME), wxEmptyString, _("Bitmap Images (*.bmp)|*.bmp|TGA Images (*.tga)|*.tga|Jpeg Images (*.jpg)|*.jpg|PNG Images (*.png)|*.png|AVI Video file(*.avi)|*.avi"));
			if (dialog.ShowModal() == wxID_OK) {
				canvas->LoadBackground(dialog.GetPath());
				dir.SetPath(dialog.GetPath());
				viewMenu->Check(ID_BACKGROUND, canvas->drawBackground);
			} else {
				viewMenu->Check(ID_BACKGROUND, false);
			}
		} else {
			canvas->drawBackground = false;
		}
	} else if (id == ID_SKYBOX) {
		if (canvas->skyModel) {
			wxDELETE(canvas->skyModel);
			canvas->sky->delChildren();
			
		} else {
			// List of skybox models, LightSkybox.dbc
			wxArrayString skyboxes;

			for (LightSkyBoxDB::Iterator it=skyboxdb.begin();  it!=skyboxdb.end(); ++it) {
				wxString str(it->getString(LightSkyBoxDB::Name));
				str = str.BeforeLast('.');
				str.Append(_T(".m2"));

				if (skyboxes.Index(str, false) == wxNOT_FOUND)
					skyboxes.Add(str);
			}
			skyboxes.Add(_T("World\\Outland\\PassiveDoodads\\SkyBox\\OutlandSkyBox.m2"));
			skyboxes.Sort();


			wxSingleChoiceDialog skyDialog(this, _("Choose"), _("Select a Sky Box"), skyboxes);
			if (skyDialog.ShowModal() == wxID_OK && skyDialog.GetStringSelection() != wxEmptyString) {
				canvas->skyModel = new Model(std::string(skyDialog.GetStringSelection().mb_str()), false);
				canvas->sky->model = canvas->skyModel;
			}
		}
		
		canvas->drawSky = event.IsChecked();
	}
}

void ModelViewer::SaveChar(const char *fn)
{
	ofstream f(fn, ios_base::out|ios_base::trunc);
	f << canvas->model->name << endl;
	f << charControl->cd.race << " " << charControl->cd.gender << endl;
	f << charControl->cd.skinColor << " " << charControl->cd.faceType << " " << charControl->cd.hairColor << " " << charControl->cd.hairStyle << " " << charControl->cd.facialHair << " " << charControl->cd.facialColor << endl;
	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		f << charControl->cd.equipment[i] << endl;
	}

	// 5976 is the ID value for "Guild Tabard"
	if (charControl->cd.equipment[CS_TABARD] == 5976) {
		f << charControl->td.Background << " " << charControl->td.Border << " " << charControl->td.BorderColor << " " << charControl->td.Icon << " " << charControl->td.IconColor << endl;
	}

	f << endl;
	f.close();
}

void ModelViewer::LoadChar(const char *fn)
{
	std::string modelname;
	ifstream f(fn);
	
	f >> modelname; // model name

	// Clear the existing model
	if (isWMO) {
		//canvas->clearAttachments();
		wxDELETE(canvas->wmo);
		canvas->wmo = NULL;
	} else if (isModel) {
		canvas->clearAttachments();
		//if (!isChar) // may memory leak
		//	wxDELETE(canvas->model);
		canvas->model = NULL;
	}

	// Load the model
	LoadModel(wxString(modelname.c_str(), wxConvUTF8));
	canvas->model->modelType = MT_CHAR;

	f >> charControl->cd.race >> charControl->cd.gender; // race and gender
	f >> charControl->cd.skinColor >> charControl->cd.faceType >> charControl->cd.hairColor >> charControl->cd.hairStyle >> charControl->cd.facialHair >> charControl->cd.facialColor;

	while (!f.eof()) {
		for (int i=0; i<NUM_CHAR_SLOTS; i++) {
			f >> charControl->cd.equipment[i];
		}
		break;
	}

	// 5976 is the ID value for "Guild Tabard"
	if (charControl->cd.equipment[CS_TABARD] == 5976 && !f.eof()) {
		f >> charControl->td.Background >> charControl->td.Border >> charControl->td.BorderColor >> charControl->td.Icon >> charControl->td.IconColor;
		charControl->td.showCustom = true;
	}

	f.close();

	charControl->RefreshModel();
	charControl->RefreshEquipment();

	charMenu->Enable(ID_SAVE_CHAR, true);
	charMenu->Enable(ID_SHOW_UNDERWEAR, true);
	charMenu->Enable(ID_SHOW_EARS, true);
	charMenu->Enable(ID_SHOW_HAIR, true);
	charMenu->Enable(ID_SHOW_FACIALHAIR, true);
	charMenu->Enable(ID_SHOW_FEET, true);
	charMenu->Enable(ID_SHEATHE, true);
	charMenu->Enable(ID_SAVE_EQUIPMENT, true);
	charMenu->Enable(ID_LOAD_EQUIPMENT, true);
	charMenu->Enable(ID_CLEAR_EQUIPMENT, true);
	charMenu->Enable(ID_LOAD_SET, true);
	charMenu->Enable(ID_LOAD_START, true);
	charMenu->Enable(ID_LOAD_NPC_START, true);
	charMenu->Enable(ID_MOUNT_CHARACTER, true);
	charMenu->Enable(ID_CHAR_RANDOMISE, true);

	// Update interface docking components
	interfaceManager.Update();
}

void ModelViewer::OnLanguage(wxCommandEvent &event)
{
	if (event.GetId() == ID_LANGUAGE) {
		/*
		static const wxLanguage langIds[] =
		{
			wxLANGUAGE_ENGLISH,
			wxLANGUAGE_KOREAN,
			wxLANGUAGE_FRENCH,
			wxLANGUAGE_GERMAN,
			wxLANGUAGE_CHINESE_SIMPLIFIED,
			wxLANGUAGE_CHINESE_TRADITIONAL,
			wxLANGUAGE_SPANISH,
		};
		*/
		
		wxString langNames[] =
		{
			_T("English"),
			_T("Korean"),
			_T("French"),
			_T("German"),
			_T("Chinese (Simplified)"),
			_T("Chinese (Traditional)"),
			_T("Spanish"),
		};

		// the arrays should be in sync
		//wxCOMPILE_TIME_ASSERT(WXSIZEOF(langNames) == WXSIZEOF(langIds), LangArraysMismatch);

		long lng = wxGetSingleChoiceIndex(_("Please select a language:"), _("Language"), WXSIZEOF(langNames), langNames);

		if (lng != -1) {
			interfaceID = lng;
			wxMessageBox(_T("You will need to reload WoW Model Viewer for changes to take effect."), _T("Language Changed"), wxOK | wxICON_INFORMATION);
		}
	}
}

void ModelViewer::OnAbout(wxCommandEvent &event)
{
/*
text = new wxStaticText(this, wxID_ANY, _T("Developers:		UfoZ, Darjk\n\n\
Pioneers:		UfoZ,  Linghuye,  nSzAbolcs\n\n\
Translators:	Culhag (French), Die_Backe (Deutsch)\n\n\
Developed Using:\n\
wxWidgets(2.6.3), wxAUI(0.9.2), OpenGL, zlib,\n\
CxImage(5.99c), MPQLib, DDSLib, GLEW(1.3.3)\n\n\
Build Info:\nVersion 0.5 compiled using Visual C++ 7.1 (2003) for\n\
Windows 98\\ME\\2000\\XP on 17th December 2006\n\n\
*/

	wxAboutDialogInfo info;
    info.SetName(APP_TITLE);
    info.SetVersion(APP_VERSION _T(" ") APP_PLATFORM APP_ISDEBUG);
	info.AddDeveloper(_T("Ufo_Z"));
	info.AddDeveloper(_T("Darjk"));
	info.AddDeveloper(_T("Chuanhsing"));
	info.AddDeveloper(_T("Kjasi (A.K.A. Sephiroth3D)"));
	info.AddDeveloper(_T("Tob.Franke"));
	info.AddTranslator(_T("MadSquirrel (French)"));
	info.AddTranslator(_T("Tigurius (Deutsch)"));
	info.AddTranslator(_T("Kurax (Chinese)"));

	info.SetWebSite(_T("http://www.wowmodelviewer.org/"));
    info.SetCopyright(
wxString(_T("World of Warcraft(R) is a Registered trademark of\n\
Blizzard Entertainment(R). All game assets and content\n\
is (C)2006 Blizzard Entertainment(R). All rights reserved.")));

	info.SetLicence(_T("WoWmodelview is released under the GNU General Public License."));

	info.SetDescription(_T("WoWmodelview is a 3D model viewer for World of Warcraft.\nIt uses the data files included with the game to display\nthe 3D models from the game: creatures, characters, spell\neffects, objects and so forth.\n\nCredits To: Linghuye,  nSzAbolcs,  Sailesh, Terran and Cryect\nfor their contributions either directly or indirectly."));
	//info.SetArtists();
	//info.SetDocWriters();

	wxIcon icon(_T("mainicon"),wxBITMAP_TYPE_ICO_RESOURCE,128,128);
#if defined (_LINUX)
	//icon.LoadFile(_T("../bin_support/icon/wmv_xpm"));
#elif defined (_MAC)
	//icon.LoadFile(_T("../bin_support/icon/wmv.icns"));
#endif
	icon.SetHeight(128);
	icon.SetWidth(128);
	info.SetIcon(icon);

#ifndef _MAC
    // FIXME: Doesn't link on OSX
    wxAboutBox(info);
#endif
}

void ModelViewer::OnCheckForUpdate(wxCommandEvent &event)
{
	wxURL url(_T("http://wowmodelviewer.googlecode.com/svn/trunk/latest.txt"));

	if(url.GetError() == wxURL_NOERR)   {
		wxInputStream *stream = url.GetInputStream();
		
		// here, just for example, I read 1024 bytes. You should read what you need...
		char buffer[1024];
		stream->Read(&buffer, 1024);
		
		// Sort the data
		wxString data(wxString(buffer, wxConvUTF8));
		wxString version = data.BeforeFirst(10);
		wxString downloadURL = data.AfterLast(10);
		int Compare = (int)version.find(wxString(APP_VERSION));
#ifdef _DEBUG
		wxLogMessage("Update Data:\nCurrent Version: \"%s\"\nRecorded Version \"%s\"\nURL Download: \"%s\"\nComparison Result: %i",wxString(APP_VERSION), version, downloadURL, Compare);
#endif

		if (Compare == 0) {
			wxMessageBox(_T("You have the most up-to-date version."), _T("Update Check"));
		} else {
			wxString msg = _T("The most current version is: ");
			msg.Append(version);
			msg.Append(_T("\nWould you like to go to the download page?"));
			int answer = wxMessageBox(msg, _("Update Check"), wxYES_NO, this);
			if (answer == wxYES)
				wxLaunchDefaultBrowser(wxString(downloadURL.ToUTF8(), wxConvUTF8));
		}

		// Create a string from the data that was received... (?)
		//wxString webversion;
		//wxMessageBox(wxString::Format("%s",buffer));

		delete stream;
	}else{
		wxMessageBox(_T("Error retrieving update information.\nPlease try again later."),_T("Update Error"));
	}
}

void ModelViewer::OnCanvasSize(wxCommandEvent &event)
{
	int id = event.GetId();
	unsigned int sizex = 0;
	unsigned int sizey = 0;
	
	if (id == ID_CANVAS512) {
		sizex = 512;
		sizey = 512;
	} else if (id == ID_CANVAS640) {
		sizex = 640;
		sizey = 480;
	} else if (id == ID_CANVAS800) {
		sizex = 800;
		sizey = 600;
	} else if (id == ID_CANVAS1024) {
		sizex = 1024;
		sizey = 768;
	} else if (id == ID_CANVAS1152) {
		sizex = 1152;
		sizey = 864;
	} else if (id == ID_CANVAS1280) {
		sizex = 1280;
		sizey = 768;
	} else if (id == ID_CANVAS120) {
		sizex = 120;
		sizey = 120;
	} else if (id == ID_CANVAS1600) {
		sizex = 1600;
		sizey = 1200;
	} else if (id == ID_CANVASW720) {
		sizex = 1280;
		sizey = 720;
	} else if (id == ID_CANVASW1080) {
		sizex = 1920;
		sizey = 1080;
	} else if (id == ID_CANVASW1200) {
		sizex = 1900;
		sizey = 1200;
	}

	if (sizex > 0 && sizey > 0) {
		int curx=0, cury=0;
		int difx=0, dify=0;

		canvas->GetClientSize(&curx, &cury);
		difx = sizex - curx;
		dify = sizey - cury;
		// if the window is already large enough,  just shrink the canvas
		//if (difx <= 0 && dify <= 0 && sizex < 800 && sizey < 600) {
		//	canvas->SetClientSize(sizex, sizey);
		//} else {
			GetClientSize(&curx, &cury);
			SetSize((curx + difx), (cury + dify));
		//}
	}
}

void ModelViewer::ModelInfo()
{
	if (!canvas->model)
		return;
	Model *m = canvas->model;
	const char *fn="ModelInfo.xml";
	ofstream xml(fn, ios_base::out | ios_base::trunc);

	if (!xml.is_open()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	MPQFile f((char *)m->modelname.c_str());
	if (f.isEof() || (f.getSize() < sizeof(ModelHeader))) {
		wxLogMessage(_T("Error: Unable to load model: [%s]"), m->modelname.c_str());
		// delete this; //?
		xml.close();
		f.close();
		return;
	}

	MPQFile g((char *)m->lodname.c_str());
	if (g.isEof() || (g.getSize() < sizeof(ModelView))) {
		wxLogMessage(_T("Error: Unable to load Lod: [%s]"), m->lodname.c_str());
		// delete this; //?
		xml.close();
		f.close();
		g.close();
		return;
	}
	ModelView *view = (ModelView*)(g.getBuffer());

	xml << "<m2>" << endl;
	xml << "  <info>" << endl;
	xml << "    <fullname>" <<m->fullname.c_str()  << "</fullname>" << endl;
	xml << "    <modelname>" <<m->modelname.c_str()  << "</modelname>" << endl;
	xml << "  </info>" << endl;
	xml << "  <header>" << endl;
//	xml << "    <id>" << m->header.id << "</id>" << endl;
	xml << "    <nameLength>" << m->header.nameLength << "</nameLength>" << endl;
	xml << "    <nameOfs>" << m->header.nameOfs << "</nameOfs>" << endl;
	xml << "    <name>" << f.getBuffer()+m->header.nameOfs << "</name>" << endl;
	xml << "    <GlobalModelFlags>" << m->header.GlobalModelFlags << "</GlobalModelFlags>" << endl;
	xml << "    <nGlobalSequences>" << m->header.nGlobalSequences << "</nGlobalSequences>" << endl;
	xml << "    <ofsGlobalSequences>" << m->header.ofsGlobalSequences << "</ofsGlobalSequences>" << endl;
	xml << "    <nAnimations>" << m->header.nAnimations << "</nAnimations>" << endl;
	xml << "    <ofsAnimations>" << m->header.ofsAnimations << "</ofsAnimations>" << endl;
	xml << "    <nAnimationLookup>" << m->header.nAnimationLookup << "</nAnimationLookup>" << endl;
	xml << "    <ofsAnimationLookup>" << m->header.ofsAnimationLookup << "</ofsAnimationLookup>" << endl;
	xml << "    <nBones>" << m->header.nBones << "</nBones>" << endl;
	xml << "    <ofsBones>" << m->header.ofsBones << "</ofsBones>" << endl;
	xml << "    <nKeyBoneLookup>" << m->header.nKeyBoneLookup << "</nKeyBoneLookup>" << endl;
	xml << "    <ofsKeyBoneLookup>" << m->header.ofsKeyBoneLookup << "</ofsKeyBoneLookup>" << endl;
	xml << "    <nVertices>" << m->header.nVertices << "</nVertices>" << endl;
	xml << "    <ofsVertices>" << m->header.ofsVertices << "</ofsVertices>" << endl;
	xml << "    <nViews>" << m->header.nViews << "</nViews>" << endl;
	xml << "    <lodname>" <<m->lodname.c_str()  << "</lodname>" << endl;
	xml << "    <nColors>" << m->header.nColors << "</nColors>" << endl;
	xml << "    <ofsColors>" << m->header.ofsColors << "</ofsColors>" << endl;
	xml << "    <nTextures>" << m->header.nTextures << "</nTextures>" << endl;
	xml << "    <ofsTextures>" << m->header.ofsTextures << "</ofsTextures>" << endl;
	xml << "    <nTransparency>" << m->header.nTransparency << "</nTransparency>" << endl;
	xml << "    <ofsTransparency>" << m->header.ofsTransparency << "</ofsTransparency>" << endl;
	xml << "    <nTexAnims>" << m->header.nTexAnims << "</nTexAnims>" << endl;
	xml << "    <ofsTexAnims>" << m->header.ofsTexAnims << "</ofsTexAnims>" << endl;
	xml << "    <nTexReplace>" << m->header.nTexReplace << "</nTexReplace>" << endl;
	xml << "    <ofsTexReplace>" << m->header.ofsTexReplace << "</ofsTexReplace>" << endl;
	xml << "    <nTexFlags>" << m->header.nTexFlags << "</nTexFlags>" << endl;
	xml << "    <ofsTexFlags>" << m->header.ofsTexFlags << "</ofsTexFlags>" << endl;
	xml << "    <nBoneLookup>" << m->header.nBoneLookup << "</nBoneLookup>" << endl;
	xml << "    <ofsBoneLookup>" << m->header.ofsBoneLookup << "</ofsBoneLookup>" << endl;
	xml << "    <nTexLookup>" << m->header.nTexLookup << "</nTexLookup>" << endl;
	xml << "    <ofsTexLookup>" << m->header.ofsTexLookup << "</ofsTexLookup>" << endl;
	xml << "    <nTexUnitLookup>" << m->header.nTexUnitLookup << "</nTexUnitLookup>" << endl;
	xml << "    <ofsTexUnitLookup>" << m->header.ofsTexUnitLookup << "</ofsTexUnitLookup>" << endl;
	xml << "    <nTransparencyLookup>" << m->header.nTransparencyLookup << "</nTransparencyLookup>" << endl;
	xml << "    <ofsTransparencyLookup>" << m->header.ofsTransparencyLookup << "</ofsTransparencyLookup>" << endl;
	xml << "    <nTexAnimLookup>" << m->header.nTexAnimLookup << "</nTexAnimLookup>" << endl;
	xml << "    <ofsTexAnimLookup>" << m->header.ofsTexAnimLookup << "</ofsTexAnimLookup>" << endl;
    xml << "    <PhysicsSettings>" << endl;
	xml << "      <VertexBox0>" << m->header.ps.VertexBox[0] << "</VertexBox0>" <<  endl;
	xml << "      <VertexBox1>" << m->header.ps.VertexBox[1] << "</VertexBox1>" <<  endl;
	xml << "      <VertexRadius>" << m->header.ps.VertexRadius << "</VertexRadius>" << endl;
	xml << "      <BoundingBox0>" << m->header.ps.BoundingBox[0] << "</BoundingBox0>" << endl;
	xml << "      <BoundingBox1>" << m->header.ps.BoundingBox[1] << "</BoundingBox1>" << endl;
	xml << "      <BoundingRadius>" << m->header.ps.BoundingRadius << "</BoundingRadius>" << endl;
	xml << "    </PhysicsSettings>" << endl;
	xml << "    <nBoundingTriangles>" << m->header.nBoundingTriangles << "</nBoundingTriangles>" << endl;
	xml << "    <ofsBoundingTriangles>" << m->header.ofsBoundingTriangles << "</ofsBoundingTriangles>" << endl;
	xml << "    <nBoundingVertices>" << m->header.nBoundingVertices << "</nBoundingVertices>" << endl;
	xml << "    <ofsBoundingVertices>" << m->header.ofsBoundingVertices << "</ofsBoundingVertices>" << endl;
	xml << "    <nBoundingNormals>" << m->header.nBoundingNormals << "</nBoundingNormals>" << endl;
	xml << "    <ofsBoundingNormals>" << m->header.ofsBoundingNormals << "</ofsBoundingNormals>" << endl;
	xml << "    <nAttachments>" << m->header.nAttachments << "</nAttachments>" << endl;
	xml << "    <ofsAttachments>" << m->header.ofsAttachments << "</ofsAttachments>" << endl;
	xml << "    <nAttachLookup>" << m->header.nAttachLookup << "</nAttachLookup>" << endl;
	xml << "    <ofsAttachLookup>" << m->header.ofsAttachLookup << "</ofsAttachLookup>" << endl;
	xml << "    <nEvents>" << m->header.nEvents << "</nEvents>" << endl;
	xml << "    <ofsEvents>" << m->header.ofsEvents << "</ofsEvents>" << endl;
	xml << "    <nLights>" << m->header.nLights << "</nLights>" << endl;
	xml << "    <ofsLights>" << m->header.ofsLights << "</ofsLights>" << endl;
	xml << "    <nCameras>" << m->header.nCameras << "</nCameras>" << endl;
	xml << "    <ofsCameras>" << m->header.ofsCameras << "</ofsCameras>" << endl;
	xml << "    <nCameraLookup>" << m->header.nCameraLookup << "</nCameraLookup>" << endl;
	xml << "    <ofsCameraLookup>" << m->header.ofsCameraLookup << "</ofsCameraLookup>" << endl;
	xml << "    <nRibbonEmitters>" << m->header.nRibbonEmitters << "</nRibbonEmitters>" << endl;
	xml << "    <ofsRibbonEmitters>" << m->header.ofsRibbonEmitters << "</ofsRibbonEmitters>" << endl;
	xml << "    <nParticleEmitters>" << m->header.nParticleEmitters << "</nParticleEmitters>" << endl;
	xml << "    <ofsParticleEmitters>" << m->header.ofsParticleEmitters << "</ofsParticleEmitters>" << endl;
	xml << "  </header>" << endl;

	xml << "  <SkeletonAndAnimation>" << endl;

	xml << "  <GlobalSequences>" << endl;
	for(size_t i=0; i<m->header.nGlobalSequences; i++)
		xml << "<Sequence>" << m->globalSequences[i] << "</Sequence>" << endl;
	xml << "  </GlobalSequences>" << endl;

	xml << "  <Animations>" << endl;
	for(size_t i=0; i<m->header.nAnimations; i++) {
		xml << "    <Animation id=\"" << i << "\">" << endl;
		xml << "      <animID>"<< m->anims[i].animID << "</animID>" << endl;
		// subAnimID
		xml << "      <length>"<< m->anims[i].timeEnd << "</length>" << endl;
		xml << "      <moveSpeed>"<< m->anims[i].moveSpeed << "</moveSpeed>" << endl;
		xml << "      <flags>"<< m->anims[i].flags << "</flags>" << endl;
		xml << "      <probability>"<< m->anims[i].probability << "</probability>" << endl;
		xml << "      <d1>"<< m->anims[i].d1 << "</d1>" << endl;
		xml << "      <d2>"<< m->anims[i].d2 << "</d2>" << endl;
		xml << "      <playSpeed>"<< m->anims[i].playSpeed << "</playSpeed>" << endl;
		xml << "      <boxA>"<< m->anims[i].boxA << "</boxA>" << endl;
		xml << "      <boxB>"<< m->anims[i].boxB << "</boxB>" << endl;
		xml << "      <rad>"<< m->anims[i].rad << "</rad>" << endl;
		xml << "      <NextAnimation>"<< m->anims[i].NextAnimation << "</NextAnimation>" << endl;
		xml << "      <Index>"<< m->anims[i].Index << "</Index>" << endl;
		xml << "    </Animation>" << endl;

	}
	xml << "  </Animations>" << endl;

	xml << "  <AnimationLookups>" << endl;
	for(size_t i=0; i<m->header.nAnimationLookup; i++)
		xml << "    <AnimationLookup id=\"" << i << "\">" << m->animLookups[i] << "</AnimationLookup>" << endl;
	xml << "  </AnimationLookups>" << endl;

	xml << "  <Bones>" << endl;
	for(size_t i=0; i<m->header.nBones; i++) {
		xml << "    <Bone id=\"" << i << "\">" << endl;
		xml << "      <keyboneid>"<< m->bones[i].boneDef.keyboneid << "</keyboneid>" << endl;
		xml << "      <billboard>"<< m->bones[i].billboard << "</billboard>" << endl;
		xml << "      <parent>"<< m->bones[i].boneDef.parent << "</parent>" << endl;
		xml << "      <geoid>"<< m->bones[i].boneDef.geoid << "</geoid>" << endl;
		xml << "      <unknown>"<< m->bones[i].boneDef.unknown << "</unknown>" << endl;
#if 0 // too huge
		// AB translation
		xml << "      <trans>" << endl;
		xml << m->bones[i].trans;
		xml << "      </trans>" << endl;
		// AB rotation
		xml << "      <rot>" << endl;
		xml << m->bones[i].rot;
		xml << "      </rot>" << endl;
		// AB scaling
		xml << "      <scale>" << endl;
		xml << m->bones[i].scale;
		xml << "      </scale>" << endl;
#endif
		xml << "      <pivot>"<< m->bones[i].boneDef.pivot << "</pivot>" << endl;
		xml << "    </Bone>" << endl;
	}
	xml << "  </Bones>" << endl;

	xml << "  <BoneLookups></BoneLookups>" << endl;

	xml << "  <KeyBoneLookups>" << endl;
	for(size_t i=0; i<m->header.nKeyBoneLookup; i++)
		xml << "<KeyBoneLookup id=\"" << i << "\">" << m->keyBoneLookup[i] << "</KeyBoneLookup>" << endl;
	xml << "  </KeyBoneLookups>" << endl;

	xml << "  </SkeletonAndAnimation>" << endl;

	xml << "  <GeometryAndRendering>" << endl;

	xml << "  <Vertices>" << m->header.nVertices << "</Vertices>" << endl;
	xml << "  <Views>" << endl;

	xml << "  <Indices>" << view->nIndex << "</Indices>" << endl;
	xml << "  <Triangles>" << view->nTris << "</Triangles>" << endl;
	xml << "  <Properties>" << view->nProps << "</Properties>" << endl;
	xml << "  <Subs>" << view->nSub << "</Subs>" << endl;
	xml << "  <Texs>" << view->nTex << "</Texs>" << endl;

	
	xml << "	<RenderPasses>" << endl;
	for (size_t i=0; i<m->passes.size(); i++) {
		xml << "	  <RenderPass id=\"" << i << "\">" << endl;
		ModelRenderPass &p = m->passes[i];
		xml << "      <indexStart>" << p.indexStart << "</indexStart>" << endl;
		xml << "      <indexCount>" << p.indexCount << "</indexCount>" << endl;
		xml << "      <vertexStart>" << p.vertexStart << "</vertexStart>" << endl;
		xml << "      <vertexEnd>" << p.vertexEnd << "</vertexEnd>" << endl;
		xml << "      <tex>" << p.tex << "</tex>" << endl;
		xml << "      <useTex2>" << p.useTex2 << "</useTex2>" << endl;
		xml << "      <useEnvMap>" << p.useEnvMap << "</useEnvMap>" << endl;
		xml << "      <cull>" << p.cull << "</cull>" << endl;
		xml << "      <trans>" << p.trans << "</trans>" << endl;
		xml << "      <unlit>" << p.unlit << "</unlit>" << endl;
		xml << "      <noZWrite>" << p.noZWrite << "</noZWrite>" << endl;
		xml << "      <billboard>" << p.billboard << "</billboard>" << endl;
		xml << "      <p>" << p.p << "</p>" << endl;
		xml << "      <texanim>" << p.texanim << "</texanim>" << endl;
		xml << "      <color>" << p.color << "</color>" << endl;
		xml << "      <opacity>" << p.opacity << "</opacity>" << endl;
		xml << "      <blendmode>" << p.blendmode << "</blendmode>" << endl;
		xml << "      <order>" << p.order << "</order>" << endl;
		xml << "      <geoset>" << p.geoset << "</geoset>" << endl;
		xml << "      <swrap>" << p.swrap << "</swrap>" << endl;
		xml << "      <twrap>" << p.twrap << "</twrap>" << endl;
		xml << "      <ocol>" << p.ocol << "</ocol>" << endl;
		xml << "      <ecol>" << p.ecol << "</ecol>" << endl;
		xml << "	  </RenderPass>" << endl;
	}
	xml << "	</RenderPasses>" << endl;

	xml << "	<Geosets>" << endl;
	for (size_t i=0; i<m->geosets.size(); i++) {
		xml << "	  <Geoset id=\"" << i << "\">" << endl;
		xml << "      <id>" << m->geosets[i].id << "</id>" << endl;
		xml << "      <vstart>" << m->geosets[i].vstart << "</vstart>" << endl;
		xml << "      <vcount>" << m->geosets[i].vcount << "</vcount>" << endl;
		xml << "      <istart>" << m->geosets[i].istart << "</istart>" << endl;
		xml << "      <icount>" << m->geosets[i].icount << "</icount>" << endl;
		xml << "      <nBones>" << m->geosets[i].nBones << "</nBones>" << endl;
		xml << "      <StartBones>" << m->geosets[i].StartBones << "</StartBones>" << endl;
		xml << "      <BoundingBox>" << m->geosets[i].BoundingBox[0] << "</BoundingBox>" << endl;
		xml << "      <BoundingBox>" << m->geosets[i].BoundingBox[1] << "</BoundingBox>" << endl;
		xml << "      <radius>" << m->geosets[i].radius << "</radius>" << endl;
		xml << "	  </Geoset>" << endl;
	}
	xml << "	</Geosets>" << endl;

	ModelTexUnit *tex = (ModelTexUnit*)(g.getBuffer() + view->ofsTex);
	xml << "	<TexUnits>" << endl;
	for (size_t i=0; i<view->nTex; i++) {
		xml << "	  <TexUnit id=\"" << i << "\">" << endl;
		xml << "      <flags>" << tex[i].flags << "</flags>" << endl;
		xml << "      <shading>" << tex[i].shading << "</shading>" << endl;
		xml << "      <op>" << tex[i].op << "</op>" << endl;
		xml << "      <op2>" << tex[i].op2 << "</op2>" << endl;
		xml << "      <colorIndex>" << tex[i].colorIndex << "</colorIndex>" << endl;
		xml << "      <flagsIndex>" << tex[i].flagsIndex << "</flagsIndex>" << endl;
		xml << "      <texunit>" << tex[i].texunit << "</texunit>" << endl;
		xml << "      <mode>" << tex[i].mode << "</mode>" << endl;
		xml << "      <textureid>" << tex[i].textureid << "</textureid>" << endl;
		xml << "      <texunit2>" << tex[i].texunit2 << "</texunit2>" << endl;
		xml << "      <transid>" << tex[i].transid << "</transid>" << endl;
		xml << "      <texanimid>" << tex[i].texanimid << "</texanimid>" << endl;
		xml << "	  </TexUnit>" << endl;
	}
	xml << "	</TexUnits>" << endl;

	xml << "  </Views>" << endl;

	xml << "  <RenderFlags></RenderFlags>" << endl;

	xml << "	<Colors>" << endl;
	for(size_t i=0; i<m->header.nColors; i++) {
		xml << "    <Color id=\"" << i << "\">" << endl;
		// AB color
		xml << "    <color>" << endl;
		xml << m->colors[i].color;
		xml << "    </color>" << endl;
		// AB opacity
		xml << "    <opacity>" << endl;
		xml << m->colors[i].opacity;
		xml << "    </opacity>" << endl;
		xml << "    </Color>" << endl;
	}
	xml << "	</Colors>" << endl;

	xml << "	<Transparency>" << endl;
	for(size_t i=0; i<m->header.nTransparency; i++) {
		xml << "    <Tran id=\"" << i << "\">" << endl;
		// AB trans
		xml << "    <trans>" << endl;
		xml << m->transparency[i].trans;
		xml << "    </trans>" << endl;
		xml << "    </Tran>" << endl;
	}
	xml << "	</Transparency>" << endl;

	xml << "  <TransparencyLookup></TransparencyLookup>" << endl;

	ModelTextureDef *texdef = (ModelTextureDef*)(f.getBuffer() + m->header.ofsTextures);
	xml << "	<Textures>" << endl;
	for(size_t i=0; i<m->header.nTextures; i++) {
		xml << "	  <Texture id=\"" << i << "\">" << endl;
		xml << "      <type>" << texdef[i].type << "</type>" << endl;
		xml << "      <flags>" << texdef[i].flags << "</flags>" << endl;
		//xml << "      <nameLen>" << texdef[i].nameLen << "</nameLen>" << endl;
		//xml << "      <nameOfs>" << texdef[i].nameOfs << "</nameOfs>" << endl;
		if (texdef[i].type == TEXTURE_FILENAME)
			xml << "		<name>" << f.getBuffer()+texdef[i].nameOfs  << "</name>" << endl;
		xml << "	  </Texture>" << endl;
	}
	xml << "	</Textures>" << endl;

	xml << "	<TextureLookup></TextureLookup>" << endl;
	xml << "	<ReplacableTextureLookup></ReplacableTextureLookup>" << endl;

	xml << "  </GeometryAndRendering>" << endl;

	xml << "  <Effects>" << endl;

	xml << "	<TexAnims>" << endl;
	for(size_t i=0; i<m->header.nTexAnims; i++) {
		xml << "	  <TexAnim id=\"" << i << "\">" << endl;
		// AB trans
		xml << "    <trans>" << endl;
		xml << m->texAnims[i].trans;
		xml << "    </trans>" << endl;
		// AB rot
		xml << "    <rot>" << endl;
		xml << m->texAnims[i].rot;
		xml << "    </rot>" << endl;
		// AB scale
		xml << "    <scale>" << endl;
		xml << m->texAnims[i].scale;
		xml << "    </scale>" << endl;
		xml << "	  </TexAnim>" << endl;
	}
	xml << "	</TexAnims>" << endl;

	xml << "	<RibbonEmitters></RibbonEmitters>" << endl;

	xml << "	<Particles>" << endl;
	for (size_t i=0; i<m->header.nParticleEmitters; i++) {
		xml << "	  <Particle id=\"" << i << "\">" << endl;
		xml << m->particleSystems[i];
		xml << "	  </Particle>" << endl;
	}
	xml << "	</Particles>" << endl;

	xml << "  </Effects>" << endl;

	xml << "	<Miscellaneous>" << endl;

	xml << "	<BoundingVolumes></BoundingVolumes>" << endl;
	xml << "	<Lights></Lights>" << endl;
	xml << "	<Cameras></Cameras>" << endl;

	xml << "	<Attachments>" << endl;
	for (size_t i=0; i<m->header.nAttachments; i++) {
		xml << "	  <Attachment id=\"" << i << "\">" << endl;
		xml << "      <id>" << m->atts[i].id << "</id>" << endl;
		xml << "      <bone>" << m->atts[i].bone << "</bone>" << endl;
		xml << "      <pos>" << m->atts[i].pos << "</pos>" << endl;
		xml << "	  </Attachment>" << endl;
	}
	xml << "	</Attachments>" << endl;

	xml << "	<Events>" << endl;
	for (size_t i=0; i<m->header.nEvents; i++) {
		xml << "	  <Event id=\"" << i << "\">" << endl;
		xml << m->events[i];
		xml << "	  </Event>" << endl;
	}
	xml << "	</Events>" << endl;


	xml << "	</Miscellaneous>" << endl;

//	xml << "    <>" << m->header. << "</>" << endl;
	xml << "</m2>" << endl;
	xml.close();
	f.close();
	g.close();
}

void DiscoveryNPC()
{
	wxString name, ret;
	// 1. from creaturedisplayinfo.dbc
	for (CreatureSkinDB::Iterator it = skindb.begin(); it != skindb.end(); ++it) {
		int npcid = it->getUInt(CreatureSkinDB::NPCID);
		int id = it->getUInt(CreatureSkinDB::SkinID);
		if (npcid == 0)
			continue;
		if (!npcs.avaiable(id)) {
			name.Printf(_T("Skin%d"), id);
			ret = npcs.addDiscoveryId(id, name);
		}
	}
	// 2. from creaturedisplayinfoextra.dbc
}

void DiscoveryItem()
{
	wxString name, ret;
	items.cleanupDiscovery();
	ofstream f("discoveryitems.csv", ios_base::out | ios_base::trunc);

	// 1. from itemsets.dbc
	for (ItemSetDB::Iterator it = setsdb.begin(); it != setsdb.end(); ++it) {
		for(size_t i=0; i<ItemSetDB::NumItems; i++) {
			int id;
			if (gameVersion == 40000)
				id = it->getUInt(ItemSetDB::ItemIDBaseV400+i);
			else
				id = it->getUInt(ItemSetDB::ItemIDBase+i);
			if (id == 0)
				continue;
			if (!items.avaiable(id)) {
				if (langID == 0)
					name = it->getString(ItemSetDB::Name);
				else
					name.Printf(_T("Set%d"), it->getUInt(ItemSetDB::SetID));
				ret = items.addDiscoveryId(id, name);
				if (f.is_open() && !ret.IsEmpty())
					f << ret.mb_str() << endl;
			}
		}
	}
	// 2. from item.dbc
	for (ItemDB::Iterator it = itemdb.begin(); it != itemdb.end(); ++it) {
		int id = it->getUInt(ItemDB::ID);
		if (!items.avaiable(id)) {
			name.Printf(_T("Item%d"), id);
			ret = items.addDiscoveryId(id, name);
			if (f.is_open() && !ret.IsEmpty())
				f << ret.mb_str() << endl;
		}
	}
	// 3. from creaturedisplayinfoextra.dbc
	int slots[11] = {1, 3, 4, 5, 6, 7, 8, 9, 10, 19, 16};
	for (NPCDB::Iterator it = npcdb.begin(); it != npcdb.end(); ++it) {
		for(size_t i=0; i<11; i++) {
			int id = it->getUInt(NPCDB::HelmID+i);
			if (id == 0)
				continue;
			try {
				ItemDB::Record r = itemdb.getByDisplayId(id);
			} catch (ItemDB::NotFound) {
				if (!items.avaiable(id+ItemDB::MaxItem)) {
					int type = slots[i];
					name.Printf(_T("NPC%d"), it->getUInt(NPCDB::NPCID));
					ret = items.addDiscoveryDisplayId(id, name, type);
					if (f.is_open() && !ret.IsEmpty())
						f << ret.mb_str() << endl;
				}
			}
		}
	}
	// 4. from model dir
	// 5. from blp dir
	wxLogMessage(_T("Discovery done."));
	if (f.is_open())
		f.close();
	items.cleanup(itemdisplaydb);
}

void ModelViewer::OnExport(wxCommandEvent &event)
{
	// If nothing's on the canvas, stop.
	if ((!canvas->model) && (!canvas->wmo) && (!canvas->adt))
		return;

	int id = event.GetId();

	// Determine if this model should be in the initial pose or not.
	bool init = false;
	if (exportMenu->IsChecked(ID_MODELEXPORT_INIT) == true){
		init = true;
	}

	// Set Default filename
	wxString newfilename;
	if (canvas->wmo) {
		newfilename << wxString(canvas->wmo->name.c_str(), wxConvUTF8).AfterLast('\\').BeforeLast('.');
	}else if (canvas->model) {
		newfilename << wxString(canvas->model->name.c_str(), wxConvUTF8).AfterLast('\\').BeforeLast('.');
	}else if (canvas->adt) {
		newfilename << wxString(canvas->adt->name.c_str(), wxConvUTF8).AfterLast('\\').BeforeLast('.');
	}

	// Identifies the ID for this export option, and does the nessicary functions.
	if (id == ID_MODELEXPORT_LWO) {
		// Adds the proper extention to our default filename.
		newfilename << _T(".lwo");
		// For M2 Models
		if (canvas->model) {
			wxFileDialog dialog(this, _T("Export Model..."), wxEmptyString, newfilename, _T("Lightwave (*.lwo)|*.lwo"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting M2 model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				// Your M2 export function goes here.
				ExportM2toLWO(canvas->root,canvas->model,dialog.GetPath().fn_str(),init);
			}
		// For WMO Models. You don't need to include this if the exporter doesn't support it.
		} else if (canvas->wmo) {
			wxFileDialog dialog(this, _T("Export World Model Object..."), wxEmptyString, newfilename, _T("Lightwave (*.lwo)|*.lwo"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting WMO model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				// Your WMO export function goes here.
				ExportWMOtoLWO(canvas->wmo, dialog.GetPath().fn_str());	
			}
		// ADT support. Same as WMOs.
		} else if (canvas->adt) {
			wxFileDialog dialog(this, _T("Export MapTile..."), wxEmptyString, newfilename, _T("Lightwave (*.lwo)|*.lwo"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting MapTile model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				// Your ADT export function goes here.
				ExportADTtoLWO(canvas->adt, dialog.GetPath().fn_str());	
			}
		}
	// That's all folks!

	}else if (id == ID_MODELEXPORT_BASE){
		SaveBaseFile();

#ifdef _DEBUG
	// Experimental new LWO Exporter!!
	} else if (id == ID_MODELEXPORT_LWO2) {
		newfilename << _T(".lwo");
		if (canvas->model) {
			wxFileDialog dialog(this, _T("Export Model..."), wxEmptyString, newfilename, _T("Lightwave (*.lwo)|*.lwo"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				ExportM2toLWO(canvas->root, canvas->model, dialog.GetPath().fn_str(), init);
			}
		} else if (canvas->wmo) {
			wxFileDialog dialog(this, _T("Export World Model Object..."), wxEmptyString, newfilename, _T("Lightwave (*.lwo)|*.lwo"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				ExportWMOtoLWO2(canvas->wmo, dialog.GetPath().fn_str());	
			}
		}
#endif
	} else if (id == ID_MODELEXPORT_OBJ) {
		newfilename << _T(".obj");
		if (canvas->model) {
			wxFileDialog dialog(this, _T("Export Model..."), wxEmptyString, newfilename, _T("Wavefront (*.obj)|*.obj"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				ExportM2toOBJ(canvas->root, canvas->model, dialog.GetPath().fn_str(), init);
			}
		} else if (canvas->wmo) {
			wxFileDialog dialog(this, _T("Export World Model Object..."), wxEmptyString, newfilename, _T("Wavefront (*.obj)|*.obj"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				ExportWMOtoOBJ(canvas->wmo, dialog.GetPath().fn_str());
			}
		}
	} else if (id == ID_MODELEXPORT_COLLADA) {
		newfilename << _T(".dae");
		if (canvas->model) {
			wxFileDialog dialog(this, _T("Export Model..."), wxEmptyString, newfilename, _T("Collada (*.dae)|*.dae"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				ExportM2toCOLLADA(canvas->root, canvas->model, dialog.GetPath().fn_str(), init);
			}
		} else if (canvas->wmo) {
			wxFileDialog dialog(this, _T("Export World Model Object..."), wxEmptyString, newfilename, _T("Collada (*.dae)|*.dae"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				ExportWMOtoCOLLADA(canvas->wmo, dialog.GetPath().fn_str());	
			}
		}
	} else if (id == ID_MODELEXPORT_MS3D) {
		newfilename << _T(".ms3d");
		if (canvas->model) {
			wxFileDialog dialog(this, _T("Export Model..."), wxEmptyString, newfilename, _T("Milkshape 3D (*.ms3d)|*.ms3d"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				ExportM2toMS3D(canvas->root, canvas->model, dialog.GetPath().fn_str(), init);
			}
		}
	} else if (id == ID_MODELEXPORT_3DS) {
		newfilename << _T(".3ds");
		if (canvas->model) {
			wxFileDialog dialog(this, _T("Export Model..."), wxEmptyString, newfilename, _T("3D Studio Max (*.3ds)|*.3ds"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				ExportM2to3DS(canvas->root, canvas->model, dialog.GetPath().fn_str(), init);
			}
		}
	} else if (id == ID_MODELEXPORT_X3D) {
		newfilename << _T(".x3d");
		if (canvas->model) {
			wxFileDialog dialog(this, _T("Export Model..."), wxEmptyString, newfilename, _T("X3D (*.x3d)|*.x3d"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				ExportM2toX3D(canvas->model, dialog.GetPath().fn_str(), init);
			}
		}
	} else if (id == ID_MODELEXPORT_XHTML) {
		newfilename << _T(".xhtml");
		if (canvas->model) {
			wxFileDialog dialog(this, _T("Export Model..."), wxEmptyString, newfilename, _T("Embedded X3D in XHTML (*.xhtml)|*.xhtml"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				ExportM2toXHTML(canvas->model, dialog.GetPath().fn_str(), init);
			}
		}
#ifdef _DEBUG
	} else if (id == ID_MODELEXPORT_OGRE) {
		// TODO: export to dotScene format, not simple mesh
		newfilename << _T(".mesh.xml");
		if (canvas->model) {
			wxFileDialog dialog(this, _T("Export Model..."), wxEmptyString, newfilename, _T("Ogre XML (*.mesh.xml)|*.mesh.xml"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				ExportM2toOgreXml(canvas->model, dialog.GetPath().fn_str(), init);
			}
		} else if (canvas->wmo) {
			wxFileDialog dialog(this, _T("Export World Model Object..."), wxEmptyString, newfilename, _T("Ogre XML (*.mesh.xml)|*.mesh.xml"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (dialog.ShowModal()==wxID_OK) {
				wxLogMessage(_T("Info: Exporting model to %s..."), wxString(dialog.GetPath().fn_str(), wxConvUTF8).c_str());

				ExportWMOtoOgreXml(canvas->wmo, dialog.GetPath().fn_str());
			}
		}
#endif
	}
}



// Other things to export...
void ModelViewer::OnExportOther(wxCommandEvent &event)
{
	int id = event.GetId();
	if (id == ID_FILE_MODEL_INFO) {
		ModelInfo();
	} else if (id == ID_FILE_DISCOVERY_ITEM) {
		DiscoveryItem();
		fileMenu->Enable(ID_FILE_DISCOVERY_ITEM, false);
	} else if (id == ID_FILE_DISCOVERY_NPC) {
		DiscoveryNPC();
		fileMenu->Enable(ID_FILE_DISCOVERY_NPC, false);
	}
}

void ModelViewer::OnTest(wxCommandEvent &event)
{
		if (!charControl->charAtt)
			return;

		if (arrowControl) {
			arrowControl->Show(true);
		} else {
			arrowControl = new ArrowControl(this, wxID_ANY, wxDefaultPosition, charControl->charAtt);
			/* // wxIFM stuff
			arrowControlPanel = new wxIFMDefaultChildData(arrowControl, IFM_CHILD_GENERIC, wxDefaultPosition, wxSize(140, 300), true, _("Arrow Control"));
			arrowControlPanel->m_orientation = IFM_ORIENTATION_FLOAT;
			interfaceManager->AddChild(arrowControlPanel);
			interfaceManager->Update(IFM_DEFAULT_RECT,true);

			interfaceManager->ShowChild(arrowControl,true,true);
			*/
			// wxAUI
			interfaceManager.AddPane(arrowControl, wxAuiPaneInfo().
			Name(wxT("Arrows")).Caption(_("Arrows")).
			FloatingSize(wxSize(150,300)).Float().Show(true)); //.FloatingPosition(GetStartPosition())
			interfaceManager.Update();
		}
}

void ModelViewer::UpdateControls()
{
	if (!canvas || !canvas->model || !canvas->root)
		return;

	if (canvas->model->modelType == MT_NPC)
		charControl->RefreshNPCModel();
	else if (canvas->model->modelType == MT_CHAR)
		charControl->RefreshModel();
	
	modelControl->RefreshModel(canvas->root);
}

void ModelViewer::ImportArmoury(wxString strURL)
{
	// Format the URL
	wxString strDomain = strURL.BeforeLast(_T('/')).AfterLast(_T('/')); // "armory.worldofwarcraft.com"
	wxString strParam = strURL.AfterLast(_T('/'));
	int pos = strParam.Find(_T("?r="));
	wxString strFile = strParam.Mid(0, pos);
	strParam = strParam.Mid(pos+3);
	pos = strParam.Find(_T("&cn=")); // newer version change from 'n=' to 'cn='
	wxString strRealm, strChar;
	if (pos >= 0) {
		strRealm = strParam.Mid(0, pos);
		strChar = strParam.Mid(pos+4);
	} else {
		pos = strParam.Find(_T("&n="));
		strRealm = strParam.Mid(0, pos);
		strChar = strParam.Mid(pos+3);
	}

	// Char Name Corrections
	// Done so names like Daïmhôndrùs will get the proper page...
	strChar = wxString(strChar.ToUTF8(), wxConvUTF8);

	// Build Page file
	// "/character-sheet.xml?r=%s&n=%s"
	wxString strPage = _T('/') + strFile;
	strPage.Append(_T("?r=")).Append(strRealm).Append(_T("&cn=")).Append(strChar);
	//http://armory.wow-europe.com/character-sheet.xml?r=Spinebreaker&cn=Nostrum

	wxLogMessage(_T("Attemping to access WoWArmory Page: %s"), wxString(strDomain + strPage).c_str());

	// Get the page from the armoury website
	wxHTTP http;

	// set the headers
	http.SetHeader(_T("User-Agent"), _T("Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-TW;rv:1.8.1.2) Gecko/20070219 Firefox/2.0.0.12")); 
	http.SetHeader(_T("Accept"), _T("text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5"));
	http.SetHeader(_T("Accept-Language"), _T("en-us,en;q=0.5"));
	http.SetHeader(_T("Accept-Charset"), _T("ISO-8859-1,utf-8;q=0.7,*;q=0.7"));
	http.SetHeader(_T("Host"), strDomain);

	if (http.Connect(strDomain))
	{ 
		// Success
		wxInputStream *stream = http.GetInputStream(strPage); 
		if (!stream || !stream->IsOk())
			return;

		// Make sure there was no error retrieving the page
		if(http.GetError() == wxPROTO_NOERR) {
			wxFileOutputStream output(_T("temp.xml"));
            stream->Read(output); 
			output.Close();
			
			wxXmlDocument xmlDoc;
			if (!xmlDoc.Load(_T("temp.xml"), _T("UTF-8")))
				return;
			
			// character-model.xml?r=Realm&cn=CharacterName
			// Model Details, to be decoded later

			// start processing the XML file
			if (xmlDoc.GetRoot()->GetName() != wxT("page"))
				return;

			wxXmlNode *child = xmlDoc.GetRoot()->GetChildren(); // page->tabInfo
			while(child) {
				if (child->GetName() == _T("characterInfo"))
					break;
				child = child->GetNext(); // page->characterInfo
			}

			if (!child)
				return;

			child = child->GetChildren();

			while (child) {

				if (child->GetName() == _T("character")) { // page->characterInfo->character

					// process text enclosed by <tag1></tag1>
					//wxString content = child->GetNodeContent();

					// process properties of <tag1>, raceId will better?
					int raceId = wxAtoi(child->GetPropVal(_T("raceId"), _T("1")));
					CharRacesDB::Record racer = racedb.getById(raceId);
					wxString race;
					if (gameVersion == 30100)
						race = racer.getString(CharRacesDB::NameV310);
					else
						race = racer.getString(CharRacesDB::Name);
					//race = race.MakeLower();
					wxString gender = child->GetPropVal(_T("gender"), _T("Male"));
					//gender = gender.MakeLower();

					wxString strModel = _T("Character\\") + race + _T("\\") + gender + _T("\\") + race + gender + _T(".m2");

					LoadModel(strModel);

					if (!g_canvas->model)
						return;

				} else if (child->GetName() == _T("characterTab")) { // page->characterInfo->characterTab
					child = child->GetChildren();

				} else if (child->GetName() == _T("items")) { // page->characterInfo->characterTab->items

					wxXmlNode *itemNode = child->GetChildren();
					while (itemNode) { // // page->characterInfo->characterTab->items->item
						if (itemNode->GetName() == _T("item")) {
							wxString id = itemNode->GetPropVal(_T("id"), _T("0"));
							wxString slot = itemNode->GetPropVal(_T("slot"), _T("0"));
							
							// Item ID
							long itemID;
							id.ToLong(&itemID);

							// Equipment Slot
							long slotID;
							slot.ToLong(&slotID);

							// Manual correction for slot ID values
							if (slotID == 4) // Chest
								slotID = CS_CHEST;
							else if (slotID == 5) // Waist
								slotID = CS_BELT;
							else if (slotID == 7) // Feet
								slotID = CS_BOOTS;
							else if (slotID == 14) // Back
								slotID = CS_CAPE;
							else if (slotID == 15) // Right Hand
								slotID = CS_HAND_RIGHT;
							else if (slotID == 16) // Left Hand
								slotID = CS_HAND_LEFT;

							g_charControl->cd.equipment[slotID] = itemID;
						}

						itemNode = itemNode->GetNext();
					}
					
					// Update the model
					g_charControl->RefreshModel();
					g_charControl->RefreshEquipment();
					break;
				}

				child = child->GetNext();
			}

			//wxRemoveFile(_T("temp.xml"));
		}
		delete stream;
	}
}

// --
