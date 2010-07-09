#include <GL/glew.h>
#include "widgets/FileList.hpp"
#include "widgets/MainWindow.hpp"
#include "modelviewer.h"
#include "globalvars.h"
#include "mpq.hpp"
#include <boost/algorithm/string/predicate.hpp>

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
	EVT_MENU(ID_SHOW_MODELOPENED, ModelViewer::OnToggleDock)	
	// --
	EVT_MENU(ID_SHOW_MASK, ModelViewer::OnToggleCommand)
	//EVT_MENU(ID_SHOW_WIREFRAME, ModelViewer::OnToggleCommand)
	//EVT_MENU(ID_SHOW_BONES, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_SHOW_BOUNDS, ModelViewer::OnToggleCommand)
	//EVT_MENU(ID_SHOW_PARTICLES, ModelViewer::OnToggleCommand)

	EVT_MENU(ID_SHOW_GRID, ModelViewer::OnToggleCommand)

	EVT_MENU(ID_USE_CAMERA, ModelViewer::OnToggleCommand)

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

	// Effects
	EVT_MENU(ID_ENCHANTS, ModelViewer::OnEffects)
	EVT_MENU(ID_SPELLS, ModelViewer::OnEffects)
	EVT_MENU(ID_EQCREATURE_R, ModelViewer::OnEffects)
	EVT_MENU(ID_EQCREATURE_L, ModelViewer::OnEffects)
	EVT_MENU(ID_SHADER_DEATH, ModelViewer::OnEffects)
//	EVT_MENU(ID_TEST, ModelViewer::OnTest)

	// Options
	EVT_MENU(ID_SAVE_CHAR, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_LOAD_CHAR, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_IMPORT_CHAR, ModelViewer::OnToggleCommand)

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
	modelControl = NULL;
	modelOpened = NULL;

	//wxWidget objects
	menuBar = NULL;
	charMenu = NULL;
	viewMenu = NULL;
	optMenu = NULL;
	lightMenu = NULL;

	isModel = false;
	isWMO = false;
	isChar = false;

	//wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU
	// create our main frame
	if (Create(NULL, wxID_ANY, wxString(_T("WoW Viewer")), wxDefaultPosition, wxSize(1024, 768), wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN, _T("ModelViewerFrame"))) {
		SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
		SetBackgroundStyle(wxBG_STYLE_CUSTOM);

		InitObjects();  // create our canvas, anim control, character control, etc

		// Show our window
		Show(true);
		// Display the window
		Centre();

		// ------
		// Initialise our main window.

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
	fileMenu->Append(ID_FILE_SCREENSHOT, _("Save Screenshot\tF12"));
	fileMenu->Append(ID_FILE_SCREENSHOTCONFIG, _("Save Sized Screenshot\tCTRL+S"));

	// --== Continue regular menu ==--
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_FILE_DISCOVERY_ITEM, _("Discovery Item"));
	if (wxFileExists(_T("discoveryitems.csv")))
		fileMenu->Enable(ID_FILE_DISCOVERY_ITEM, false);
	fileMenu->Append(ID_FILE_DISCOVERY_NPC, _("Discovery NPC"));

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
	viewMenu->AppendSeparator();
	if (canvas) {
		viewMenu->Append(ID_BG_COLOR, _("Background Color..."));
		viewMenu->AppendCheckItem(ID_BACKGROUND, _("Load Background\tCTRL+L"));
		viewMenu->AppendCheckItem(ID_SKYBOX, _("Skybox"));
		viewMenu->AppendCheckItem(ID_SHOW_GRID, _("Show Grid"));

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

		// Options menu
		optMenu = new wxMenu;
		optMenu->AppendCheckItem(ID_DEFAULT_DOODADS, _("Always show default doodads in WMOs"));
		optMenu->Check(ID_DEFAULT_DOODADS, true);
		optMenu->AppendSeparator();
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

	FileList* file_list = new FileList;
	MainWindow::add_dock("File list", Qt::LeftDockWidgetArea, file_list);

	animControl = new AnimControl(this, ID_ANIM_FRAME);
	charControl = new CharControl(this, ID_CHAR_FRAME);
	modelControl = new ModelControl(this, ID_MODEL_FRAME);
//	settingsControl = new SettingsControl(this, ID_SETTINGS_FRAME);
//	settingsControl->Show(false);
	modelOpened = new ModelOpened(this, ID_MODELOPENED_FRAME);

	canvas = new ModelCanvas(this);

	g_modelViewer = this;
	g_animControl = animControl;
	g_canvas = canvas;

	modelControl->animControl = animControl;
	
	enchants = new EnchantsDialog(this, charControl);
}

void ModelViewer::InitDatabase()
{
	wxLogMessage(_T("Initiating Databases..."));
	initDB = true;

	itemdb.open();

	wxString filename = _T("enUS/items.csv");
	if (wxFile::Exists(filename)) {
		items.open(filename);
	} else {
		wxLogMessage(_T("Error: Could not find items.csv to load an item list from."));
	}

	skyboxdb.open();
	spellitemenchantmentdb.open();
	itemvisualsdb.open();
	animdb.open();
	modeldb.open();
	skindb.open();
	hairdb.open();
	chardb.open();
	racedb.open();
	classdb.open();
	facialhairdb.open();
	visualdb.open();
	effectdb.open();
	subclassdb.open();
	startdb.open();
	npcdb.open();
	npctypedb.open();
	itemdisplaydb.open();
	items.cleanup(itemdisplaydb);

	setsdb.open();
	setsdb.cleanup(items);

	spelleffectsdb.open();
	GetSpellEffects();

	wxLogMessage(_T("Finished initiating database files."));
}

void ModelViewer::InitDocking()
{
	wxLogMessage(_T("Initiating GUI Docking."));
	
	// wxAUI stuff
	//interfaceManager.SetFrame(this); 
	interfaceManager.SetManagedWindow(this);

	// Animation frame
    interfaceManager.AddPane(animControl, wxAuiPaneInfo().
				Name(wxT("animControl")).Caption(_("Animation")).
				Bottom().Layer(1));

	// Character frame
	interfaceManager.AddPane(charControl, wxAuiPaneInfo().
                Name(wxT("charControl")).Caption(_("Character")).
                BestSize(wxSize(170,700)).Right().Layer(2).Show(isChar));

	// model control
	interfaceManager.AddPane(modelControl, wxAuiPaneInfo().
		Name(wxT("Models")).Caption(_("Models")).
		FloatingSize(wxSize(160,460)).Float().Show(false).
		DestroyOnClose(false));

	// model opened
	interfaceManager.AddPane(modelOpened, wxAuiPaneInfo().
		Name(wxT("ModelOpened")).Caption(_("ModelOpened")).
		FloatingSize(wxSize(700,90)).Float().Fixed().Show(false).
		DestroyOnClose(false));
}

void ModelViewer::LoadModel(const std::string& fn)
{
	if (!canvas || fn.empty())
		return;

	isModel = true;

	// check if this is a character model
	isChar = boost::algorithm::starts_with(fn, "character");

	Attachment *modelAtt = canvas->LoadCharModel(fn.c_str());
	if (!modelAtt)
	{
		wxLogMessage(_T("Error: Failed to load the model - %s"), fn.c_str());
		return;
	}

	if (isChar)
	{
		canvas->model->modelType = MT_CHAR;
	}
	else
	{
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
		int displayID = modelRec.Get<unsigned int>(CreatureSkinDB::NPCID);

		// if the creature ID ISN'T a "NPC",  then load the creature model and skin it.
		if (displayID == 0) {
			
			unsigned int modelID = modelRec.Get<unsigned int>(CreatureSkinDB::ModelID);
			CreatureModelDB::Record creatureModelRec = modeldb.getByID(modelID);
			
			wxString name(creatureModelRec.model_path(),wxConvUTF8);
			name = name.BeforeLast('.');
			name.Append(_T(".m2"));

			LoadModel(std::string(name.mb_str()));
			canvas->model->modelType = MT_NORMAL;

			TextureGroup grp;
			int count = 0;
			for (int i=0; i<TextureGroup::num; i++) {
				wxString skin(modelRec.getString(CreatureSkinDB::Skin + i),wxConvUTF8);
				
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
			CharRacesDB::Record rec2 = getByID(racedb,rec.Get<unsigned int>(NPCDB::RaceID));
			
			wxString retval ( rec.getString(NPCDB::Gender),wxConvUTF8);
			wxString strModel = _T("Character\\");

			if (gameVersion == 30100) {
				if (!retval.IsEmpty()) {
					strModel.append(wxString(rec2.getString(CharRacesDB::NameV310),wxConvUTF8));
					strModel.append(_T("\\Female\\"));
					strModel.append(wxString(rec2.getString(CharRacesDB::NameV310),wxConvUTF8));
					strModel.append(_T("Female.m2"));
				} else {
					strModel.append(wxString(rec2.getString(CharRacesDB::NameV310),wxConvUTF8));
					strModel.append(_T("\\Male\\"));
					strModel.append(wxString(rec2.getString(CharRacesDB::NameV310),wxConvUTF8));
					strModel.append(_T("Male.m2"));
				}
			} else {
				if (!retval.IsEmpty()) {
					strModel.append(wxString(rec2.getString(CharRacesDB::Name),wxConvUTF8));
					strModel.append(_T("\\Female\\"));
					strModel.append(wxString(rec2.getString(CharRacesDB::Name),wxConvUTF8));
					strModel.append(_T("Female.m2"));
				} else {
					strModel.append(wxString(rec2.getString(CharRacesDB::Name),wxConvUTF8));
					strModel.append(_T("\\Male\\"));
					strModel.append(wxString(rec2.getString(CharRacesDB::Name),wxConvUTF8));
					strModel.append(_T("Male.m2"));
				}
			}
			
			//const char *newName = strModel.c_str();

			Attachment *modelAtt;
			modelAtt = canvas->LoadCharModel(strModel.mb_str());
			canvas->model->modelType = MT_NPC;

			wxString fn(_T("Textures\\Bakednpctextures\\"));
			fn.Append(wxString(rec.getString(NPCDB::Filename),wxConvUTF8));
			charControl->UpdateNPCModel(modelAtt, displayID);
			charControl->customSkin = fn;

			charControl->RefreshNPCModel(); // rec.Get<unsigned int>(NPCDB::NPCID
			charControl->RefreshEquipment();

			menuBar->EnableTop(2, true);
			charMenu->Check(ID_SHOW_UNDERWEAR, true);
			charMenu->Check(ID_SHOW_EARS, true);
			charMenu->Check(ID_SHOW_HAIR, true);
			charMenu->Check(ID_SHOW_FACIALHAIR, true);
			// ---

			animControl->UpdateModel(canvas->model);
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
		ItemDisplayDB::Record modelRec = get_by_ID(itemdisplaydb,displayID);
		wxString name ( modelRec.model(0),wxConvUTF8);
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
			if (FS().getSize(fn.fn_str()) > 0) {
				LoadModel(std::string(fn.fn_str()));
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

	// wxAUI stuff
	interfaceManager.UnInit();

	if (canvas) {
		canvas->Disable();
		canvas->Destroy(); 
		canvas = NULL;
	}

	if (animControl) {
		animControl->Destroy();
		animControl = NULL;
	}

	if (charControl) {
		charControl->Destroy();
		charControl = NULL;
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

	return true;
}

bool ModelViewer::Init()
{
	wxLogMessage(_T("Setting OpenGL render state..."));
	video.InitGL();

	// Initiate other stuff
	wxLogMessage(_T("Initiating Archives...\n"));

	// more detail logging, this is so when someone has a problem and they send their log info
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

	charControl->Init();

	return true;
}

// Menu button press events
void ModelViewer::OnToggleDock(wxCommandEvent &event)
{
	int id = event.GetId();

	// wxAUI Stuff
	if (id==ID_SHOW_ANIM)
		interfaceManager.GetPane(animControl).Show(true);
	else if (id==ID_SHOW_CHAR && isChar)
		interfaceManager.GetPane(charControl).Show(true);
	else if (id==ID_SHOW_MODEL)
		interfaceManager.GetPane(modelControl).Show(true);
	else if (id==ID_SHOW_SETTINGS) {
//		interfaceManager.GetPane(settingsControl).Show(true);
//		settingsControl->Open();
	} else if (id==ID_SHOW_MODELOPENED) {
		interfaceManager.GetPane(modelOpened).Show(true);
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
		break;

	case ID_USE_ENVMAP:
		video.useEnvMapping = event.IsChecked();
		break;

	case ID_SHOW_MASK:
		break;

	case ID_SHOW_BOUNDS:
		canvas->model->showBounds = !canvas->model->showBounds;
		break;

	case ID_SHOW_GRID:
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
		break;
	case ID_LOAD_CHAR:
		break;
	case ID_IMPORT_CHAR:
		break;
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

void ModelViewer::OnSetEquipment(wxCommandEvent &event)
{
	if (isChar) 
		charControl->OnButton(event);

	UpdateControls();
}

void ModelViewer::OnCharToggle(wxCommandEvent &event)
{
//	if (event.GetId() == ID_VIEW_NPC)
//		charControl->selectNPC(UPDATE_NPC);
	if (event.GetId() == ID_VIEW_ITEM)
		charControl->selectItem(UPDATE_SINGLE_ITEM, -1, -1);
	else if (isChar) 
		charControl->OnCheck(event);
	
}

void ModelViewer::OnMount(wxCommandEvent &event)
{
	charControl->selectMount();
}

void ModelViewer::OnAbout(wxCommandEvent &event)
{
	wxAboutDialogInfo info;
    info.SetName(_T("WoW View"));
    info.SetVersion(_T("0.8.15"));
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
