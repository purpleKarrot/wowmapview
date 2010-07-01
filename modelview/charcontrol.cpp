// our headers
#include "widgets/CharacterControl.hpp"
#include "widgets/EquipmentControl.hpp"
#include "widgets/MainWindow.hpp"
#include "util.h"
#include "charcontrol.h"
#include "modelviewer.h"
#include "itemselection.h"
#include "mpq.hpp"
#include "globalvars.h"

int slotOrder[] = {	
	CS_SHIRT,
	CS_HEAD,
//	CS_NECK,
	CS_SHOULDER,
	CS_PANTS,
	CS_BOOTS,
	CS_CHEST,
	CS_TABARD,
	CS_BELT,
	CS_BRACERS,
	CS_GLOVES,
	CS_HAND_RIGHT,
	CS_HAND_LEFT,
	CS_CAPE,
	CS_QUIVER
};

int slotOrderWithRobe[] = {
	CS_SHIRT,
	CS_HEAD,
//	CS_NECK,
	CS_SHOULDER,
	CS_BOOTS,
	CS_PANTS,
	CS_BRACERS,
	CS_CHEST,
	CS_GLOVES,
	CS_TABARD,
	CS_BELT,
	CS_HAND_RIGHT,
	CS_HAND_LEFT,
	CS_CAPE,
	CS_QUIVER
};

IMPLEMENT_CLASS(CharControl, wxWindow)

BEGIN_EVENT_TABLE(CharControl, wxWindow)
	EVT_SPIN(ID_TABARD_ICON, CharControl::OnTabardSpin)
	EVT_SPIN(ID_TABARD_ICONCOLOR, CharControl::OnTabardSpin)
	EVT_SPIN(ID_TABARD_BORDER, CharControl::OnTabardSpin)
	EVT_SPIN(ID_TABARD_BORDERCOLOR, CharControl::OnTabardSpin)
	EVT_SPIN(ID_TABARD_BACKGROUND, CharControl::OnTabardSpin)

	EVT_BUTTON(ID_EQUIPMENT + CS_HEAD, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_SHOULDER, CharControl::OnButton)
	
	EVT_BUTTON(ID_EQUIPMENT + CS_SHIRT, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_CHEST, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_BELT, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_PANTS, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_BOOTS, CharControl::OnButton)

	EVT_BUTTON(ID_EQUIPMENT + CS_BRACERS, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_GLOVES, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_CAPE, CharControl::OnButton)

	EVT_BUTTON(ID_EQUIPMENT + CS_HAND_RIGHT, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_HAND_LEFT, CharControl::OnButton)

	EVT_BUTTON(ID_EQUIPMENT + CS_QUIVER, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_TABARD, CharControl::OnButton)
END_EVENT_TABLE()

CharControl::CharControl(wxWindow* parent, wxWindowID id)
{
	wxLogMessage(_T("Creating Char Control..."));

	new_cc = new CharacterControl(*this);
	MainWindow::add_dock("Character control", Qt::RightDockWidgetArea, new_cc);

	new_ec = new EquipmentControl(*this);
	MainWindow::add_dock("Character control", Qt::RightDockWidgetArea, new_ec);

	if(Create(parent, id, wxDefaultPosition, wxSize(100,700), 0, _T("CharControl")) == false) {
		wxLogMessage(_T("GUI Error: Failed to create a window frame for the Character Control!"));
		return;
	}

	wxFlexGridSizer *top = new wxFlexGridSizer(1);
	top->AddGrowableCol(0);

	wxFlexGridSizer *gs = new wxFlexGridSizer(3, 5, 4);

	top->Add(gs,wxSizerFlags().Proportion(1).Expand().Border(wxALL, 10));

	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		buttons[i] = NULL;
		labels[i] = NULL;
	}
	
	top->Add(new wxStaticText(this, -1, _("Equipment"), wxDefaultPosition, wxSize(200,20), wxALIGN_CENTRE), wxSizerFlags().Border(wxTOP, 10));
	wxFlexGridSizer *gs2 = new wxFlexGridSizer(2, 5, 5);
	gs2->AddGrowableCol(1);

	#define ADD_CONTROLS(type, caption) \
	gs2->Add(buttons[type]=new wxButton(this, ID_EQUIPMENT + type, caption), wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL)); \
	gs2->Add(labels[type]=new wxStaticText(this, -1, _("---- None ----")), wxSizerFlags().Proportion(1).Expand().Align(wxALIGN_CENTER_VERTICAL).Border(wxRIGHT, 10));
	
	ADD_CONTROLS(CS_HEAD, _("Head"))
	ADD_CONTROLS(CS_SHOULDER, _("Shoulder"))

	ADD_CONTROLS(CS_SHIRT, _("Shirt"))
	ADD_CONTROLS(CS_CHEST, _("Chest"))
	ADD_CONTROLS(CS_BELT, _("Belt"))
	ADD_CONTROLS(CS_PANTS, _("Legs"))
	ADD_CONTROLS(CS_BOOTS, _("Boots"))

	ADD_CONTROLS(CS_BRACERS, _("Bracers"))
	ADD_CONTROLS(CS_GLOVES, _("Gloves"))
	ADD_CONTROLS(CS_CAPE, _("Cape"))

	ADD_CONTROLS(CS_HAND_RIGHT, _("Right hand"))
	ADD_CONTROLS(CS_HAND_LEFT, _("Left hand"))

	ADD_CONTROLS(CS_QUIVER, _("Quiver"))
	ADD_CONTROLS(CS_TABARD, _("Tabard"))
	#undef ADD_CONTROLS
	
	top->Add(gs2, wxEXPAND);

	// Create our tabard customisation spin buttons
	wxGridSizer *gs3 = new wxGridSizer(2, 5, 5);
	#define ADD_CONTROLS(type, id, caption) \
	gs3->Add(new wxStaticText(this, -1, caption), wxSizerFlags().Align(wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL)); \
	gs3->Add(tabardSpins[type]=new wxSpinButton(this, id, wxDefaultPosition, wxDefaultSize, wxSP_HORIZONTAL|wxSP_WRAP), wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL));

	ADD_CONTROLS(SPIN_TABARD_ICON, ID_TABARD_ICON, _("Icon"))
	ADD_CONTROLS(SPIN_TABARD_ICONCOLOR, ID_TABARD_ICONCOLOR, _("Icon Color"))
	ADD_CONTROLS(SPIN_TABARD_BORDER, ID_TABARD_BORDER, _("Border"))
	ADD_CONTROLS(SPIN_TABARD_BORDERCOLOR, ID_TABARD_BORDERCOLOR, _("Border Color"))
	ADD_CONTROLS(SPIN_TABARD_BACKGROUND, ID_TABARD_BACKGROUND, _("Background Color"))

	#undef ADD_CONTROLS

	top->Add(new wxStaticText(this, -1, _("Tabard details")), wxSizerFlags().Align(wxALIGN_CENTRE).Border(wxALL, 1));
	top->Add(gs3, wxEXPAND);

	//p->SetSizer(top);
	
	top->SetSizeHints(this);
	Show(true);
	SetAutoLayout(true);
	SetSizer(top);
	Layout();
	
	choosingSlot = 0;
	itemDialog = 0;
	model = 0;
	charAtt = 0;
}

CharControl::~CharControl()
{
	
}

bool CharControl::Show(bool show)
{
	new_cc->setVisible(show);
	new_ec->setVisible(show);

	return wxWindow::Show(show);
}

bool CharControl::Init()
{
	charTex = 0;
	hairTex = 0;
	furTex = 0;
	capeTex = 0;
	gobTex = 0;

	td.showCustom = false;
	bSheathe = false;

	cd.useNPC = 0;
	cd.showEars = true;
	cd.showHair = true;
	cd.showFacialHair = true;
	cd.showUnderwear = true;
	
	// set max values for custom tabard
	td.maxBackground = 50;
	td.maxBorder = 9;
	td.maxBorderColor = 16;
	td.maxIcon = 169;
	td.maxIconColor = 16;

	return true;
}

//void CharControl::UpdateModel(Model *m)
void CharControl::UpdateModel(Attachment *a)
{
	if (!a)
		return;

	charAtt = a;
	model = (Model*)charAtt->model;

	// The following isn't actually needed, 
	// pretty sure all this gets taken care of by TextureManager and CharTexture
	charTex = 0;
	if (charTex==0) 
		glGenTextures(1, &charTex);

	cd.reset();
	td.showCustom = false;

	// hide most geosets
	for (size_t i=0; i<model->geosets.size(); i++) {
		model->showGeosets[i] = (model->geosets[i].id==0);
	}

	size_t p1 = model->name.find_first_of('\\', 0);
	size_t p2 = model->name.find_first_of('\\', p1+1);
	size_t p3 = model->name.find_first_of('\\', p2+1);

	std::string raceName = model->name.substr(p1+1,p2-p1-1);
	std::string genderName = model->name.substr(p2+1,p3-p2-1);

	unsigned int race, gender;

	try {
		// Okay for some reason Blizzard have removed the full racial names
		// out of the ChrRaces.dbc.  Going to have to hardcode the values.
		CharRacesDB::Record raceRec = racedb.getByName(wxString(raceName.c_str(), wxConvUTF8));
		race = raceRec.Get<unsigned int>(CharRacesDB::RaceID);
		gender = (genderName == "female" || genderName == "Female" || genderName == "FEMALE") ? 1 : 0;
	
	} catch (std::exception&) {
		// wtf
		race = 0;
		gender = 0;
	}

	// Enable the use of NPC skins if its  a goblin.
	if (race == 9 && gameVersion < 40000)
		cd.useNPC=1;
	else
		cd.useNPC=0;

	if (race==6 || race==8 || race==11 || race==13 || race==14) // If its a troll/tauren/dranei/naga/broken, show the feet (dont wear boots)
		cd.showFeet = true;
	else
		cd.showFeet = false;

	// get max values
	cd.maxSkinColor = chardb.getColorsFor(race, gender, CharSectionsDB::SkinType, 0, cd.useNPC);
	if (cd.maxSkinColor==0 && cd.useNPC==1) {
		wxMessageBox(_T("The selected character does not have any NPC skins!\nSwitching back to normal character skins."));
		cd.useNPC = 0;
		cd.maxSkinColor = chardb.getColorsFor(race, gender, CharSectionsDB::SkinType, 0, cd.useNPC);
	}
	cd.maxFaceType  = chardb.getSectionsFor(race, gender, CharSectionsDB::FaceType, 0, cd.useNPC);
	cd.maxHairColor = chardb.getColorsFor(race, gender, CharSectionsDB::HairType, 0, 0);
	cd.maxFacialHair = facialhairdb.getStylesFor(race, gender);

#ifndef	WotLK
	// Re-set the menu
	if (cd.useNPC)
		g_modelViewer->optMenu->Check(ID_USE_NPCSKINS, 1);
	else
		g_modelViewer->optMenu->Check(ID_USE_NPCSKINS, 0);
#endif

	g_modelViewer->charMenu->Check(ID_SHOW_FEET, 0);
	// ----

	cd.race = race;
	cd.gender = gender;

	std::set<int> styles;
	for (CharHairGeosetsDB::Iterator it = hairdb.begin(); it != hairdb.end(); ++it) {
		if (it->Get<unsigned int>(CharHairGeosetsDB::Race)==race && it->Get<unsigned int>(CharHairGeosetsDB::Gender)==gender) {
			styles.insert(it->Get<unsigned int>(CharHairGeosetsDB::Section));
		}
	}
	cd.maxHairStyle = (int)styles.size();
#if 1 // for worgen female
	if (gameVersion >= 40000 && cd.race == 22 && cd.gender == 1) { // female worgen 
		cd.maxHairStyle = 21;
	}
#endif // for worgen female

	if (cd.maxFaceType==0) cd.maxFaceType = 1;
	if (cd.maxSkinColor==0) cd.maxSkinColor = 1;
	if (cd.maxHairColor==0) cd.maxHairColor = 1;
	if (cd.maxHairStyle==0) cd.maxHairStyle = 1;
	if (cd.maxFacialHair==0) cd.maxFacialHair = 1;
//	spins[0]->SetRange(0, cd.maxSkinColor-1);
//	spins[1]->SetRange(0, cd.maxFaceType-1);
//	spins[2]->SetRange(0, cd.maxHairColor-1);
//	spins[3]->SetRange(0, cd.maxHairStyle-1);
//	spins[4]->SetRange(0, cd.maxFacialHair-1);
//	spins[5]->SetRange(0, cd.maxHairColor-1);

	td.Icon = randint(0, td.maxIcon);
	td.IconColor = randint(0, td.maxIconColor);
	td.Border = randint(0, td.maxBorder);
	td.BorderColor = 0;
	td.Background = randint(0, td.maxBackground);

//	spins[SPIN_SKIN_COLOR]->SetValue(cd.skinColor);
//	spins[SPIN_FACE_TYPE]->SetValue(cd.faceType);
//	spins[SPIN_HAIR_COLOR]->SetValue(cd.hairColor);
//	spins[SPIN_HAIR_STYLE]->SetValue(cd.hairStyle);
//	spins[SPIN_FACIAL_HAIR]->SetValue(cd.facialHair);
//	spins[SPIN_FACIAL_COLOR]->SetValue(cd.facialColor);

	tabardSpins[SPIN_TABARD_ICON]->SetValue(td.Icon);
	tabardSpins[SPIN_TABARD_ICONCOLOR]->SetValue(td.IconColor);
	tabardSpins[SPIN_TABARD_BORDER]->SetValue(td.Border);
	tabardSpins[SPIN_TABARD_BORDERCOLOR]->SetValue(td.BorderColor);
	tabardSpins[SPIN_TABARD_BACKGROUND]->SetValue(td.Background);

	tabardSpins[SPIN_TABARD_ICON]->SetRange(0, td.maxIcon);
	tabardSpins[SPIN_TABARD_ICONCOLOR]->SetRange(0, td.maxIconColor);
	tabardSpins[SPIN_TABARD_BORDER]->SetRange(0, td.maxBorder);
	tabardSpins[SPIN_TABARD_BORDERCOLOR]->SetRange(0, td.maxBorderColor);
	tabardSpins[SPIN_TABARD_BACKGROUND]->SetRange(0, td.maxBackground);

//	for (int i=0; i<NUM_SPIN_BTNS; i++)
//		spins[i]->Refresh(false);
	for (int i=0; i<NUM_TABARD_BTNS; i++) 
		tabardSpins[i]->Refresh(false);
//	for (int i=0; i<NUM_SPIN_BTNS; i++)
//		spinLabels[i]->SetLabel(wxString::Format(_T("%i / %i"), spins[i]->GetValue(), spins[i]->GetMax()));

	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		if (labels[i]) {
			labels[i]->SetLabel(_("---- None ----"));
			labels[i]->SetForegroundColour(*wxBLACK);
		}
	}

	RefreshModel();
}

void CharControl::UpdateNPCModel(Attachment *a, unsigned int id)
{
	if (!a)
		return;

	charAtt = a;
	model = (Model*)a->model;

	charTex = 0;
	if (charTex==0) 
		glGenTextures(1, &charTex);

	// Open the first record, just so we can declare the var.
	NPCDB::Record npcrec = npcdb.getRecord(0);

	// Get the NPC Record to construct the NPC character model from.
	try {
		npcrec = npcdb.getByNPCID(id);
	} catch (...) {
		wxLogMessage(_T("Exception Error: %s : line #%i : %s\n\tUnable to load NPC #%i"), __FILE__, __LINE__, __FUNCTION__, id);
		return;
	}

	cd.reset();
	td.showCustom = false;

	// hide most geosets
	for (size_t i=0; i<model->geosets.size(); i++) {
		model->showGeosets[i] = (model->geosets[i].id==0);
	}

	size_t p1 = model->name.find_first_of('\\', 0);
	size_t p2 = model->name.find_first_of('\\', p1+1);
	size_t p3 = model->name.find_first_of('\\', p2+1);

	std::string raceName = model->name.substr(p1+1,p2-p1-1);
	std::string genderName = model->name.substr(p2+1,p3-p2-1);

	int race = 0, gender = 0;

	try {
		CharRacesDB::Record raceRec = racedb.getByName(wxString(raceName.c_str(), wxConvUTF8));
		race = raceRec.Get<unsigned int>(CharRacesDB::RaceID);

		gender = (genderName == "Female") ? 1 : 0;
	} catch (std::exception&) {
		// wtf
		race = 0;
		gender = 0;
	}

	cd.race = race;
	cd.gender = gender;

	// Enable the use of NPC skins if its a goblin.
	if (race == 9 && gameVersion < 40000)
		cd.useNPC=1;
	else
		cd.useNPC=0;

	// Model Characteristics
	try {
		cd.skinColor = npcrec.Get<unsigned int>(NPCDB::SkinColor);
		cd.faceType = npcrec.Get<unsigned int>(NPCDB::Face);
		cd.hairColor = npcrec.Get<unsigned int>(NPCDB::HairColor);
		cd.hairStyle = npcrec.Get<unsigned int>(NPCDB::HairStyle);
		cd.facialHair = npcrec.Get<unsigned int>(NPCDB::FacialHair);
	} catch (...) {
		wxLogMessage(_T("Exception Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
	}
	
	cd.maxFaceType = 0;
	cd.maxSkinColor = 0;
	cd.maxHairColor = 0;
	cd.maxHairStyle = 0;
	cd.maxFacialHair = 0;
//	spins[0]->SetRange(0, cd.maxSkinColor-1);
//	spins[1]->SetRange(0, cd.maxFaceType-1);
//	spins[2]->SetRange(0, cd.maxHairColor-1);
//	spins[3]->SetRange(0, cd.maxHairStyle-1);
//	spins[4]->SetRange(0, cd.maxFacialHair-1);
//	spins[5]->SetRange(0, cd.maxHairColor-1);

//	spins[SPIN_SKIN_COLOR]->SetValue(cd.skinColor);
//	spins[SPIN_FACE_TYPE]->SetValue(cd.faceType);
//	spins[SPIN_HAIR_COLOR]->SetValue(cd.hairColor);
//	spins[SPIN_HAIR_STYLE]->SetValue(cd.hairStyle);
//	spins[SPIN_FACIAL_HAIR]->SetValue(cd.facialHair);
//	spins[SPIN_FACIAL_COLOR]->SetValue(cd.facialColor);

	tabardSpins[SPIN_TABARD_ICON]->SetValue(td.Icon);
	tabardSpins[SPIN_TABARD_ICONCOLOR]->SetValue(td.IconColor);
	tabardSpins[SPIN_TABARD_BORDER]->SetValue(td.Border);
	tabardSpins[SPIN_TABARD_BORDERCOLOR]->SetValue(td.BorderColor);
	tabardSpins[SPIN_TABARD_BACKGROUND]->SetValue(td.Background);

	tabardSpins[SPIN_TABARD_ICON]->SetRange(0, td.maxIcon);
	tabardSpins[SPIN_TABARD_ICONCOLOR]->SetRange(0, td.maxIconColor);
	tabardSpins[SPIN_TABARD_BORDER]->SetRange(0, td.maxBorder);
	tabardSpins[SPIN_TABARD_BORDERCOLOR]->SetRange(0, td.maxBorderColor);
	tabardSpins[SPIN_TABARD_BACKGROUND]->SetRange(0, td.maxBackground);

//	for (int i=0; i<NUM_SPIN_BTNS; i++)
//		spins[i]->Refresh(false);
	for (int i=0; i<NUM_TABARD_BTNS; i++) 
		tabardSpins[i]->Refresh(false);

	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		if (labels[i]) 
			labels[i]->SetLabel(_("---- None ----"));
	}

	// Equip our npc
	try {
		cd.equipment[CS_HEAD] = npcrec.Get<unsigned int>(NPCDB::HelmID);
		cd.equipment[CS_SHOULDER] = npcrec.Get<unsigned int>(NPCDB::ShoulderID);
		cd.equipment[CS_SHIRT] = npcrec.Get<unsigned int>(NPCDB::ShirtID);
		cd.equipment[CS_CHEST] = npcrec.Get<unsigned int>(NPCDB::ChestID);
		cd.equipment[CS_BELT] = npcrec.Get<unsigned int>(NPCDB::BeltID);
		cd.equipment[CS_PANTS] = npcrec.Get<unsigned int>(NPCDB::PantsID);
		cd.equipment[CS_BOOTS] = npcrec.Get<unsigned int>(NPCDB::BootsID);
		cd.equipment[CS_BRACERS] = npcrec.Get<unsigned int>(NPCDB::BracersID);
		cd.equipment[CS_GLOVES] = npcrec.Get<unsigned int>(NPCDB::GlovesID);
		cd.equipment[CS_TABARD] = npcrec.Get<unsigned int>(NPCDB::TabardID);
		cd.equipment[CS_CAPE] = npcrec.Get<unsigned int>(NPCDB::CapeID);
		if (cd.equipment[CS_TABARD] != 0) 
			cd.geosets[12] = 2;
	} catch (...) {
		wxLogMessage(_T("Exception Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
	}

	RefreshNPCModel();
}

void CharControl::OnCheck(wxCommandEvent &event)
{
	if (event.GetId()==ID_SHOW_UNDERWEAR) 
		cd.showUnderwear = event.IsChecked();
	else if (event.GetId()==ID_SHOW_HAIR) 
		cd.showHair = event.IsChecked();
	else if (event.GetId()==ID_SHOW_FACIALHAIR) 
		cd.showFacialHair = event.IsChecked();
	else if (event.GetId()==ID_SHOW_EARS) 
		cd.showEars = event.IsChecked();
	else if (event.GetId()==ID_SHEATHE) 
		bSheathe = event.IsChecked();
	else if (event.GetId()==ID_SHOW_FEET) 
		cd.showFeet = event.IsChecked();
#ifndef	WotLK
	else if (event.GetId()==ID_USE_NPCSKINS) {		
		// All this extra checking is to modify the the 'bounds' of the max skins on the spin button.
		size_t p1 = model->name.find_first_of('\\', 0);
		size_t p2 = model->name.find_first_of('\\', p1+1);
		size_t p3 = model->name.find_first_of('\\', p2+1);

		std::string raceName = model->name.substr(p1+1,p2-p1-1);
		std::string genderName = model->name.substr(p2+1,p3-p2-1);

		unsigned int race, gender;

		try {
			CharRacesDB::Record raceRec = racedb.getByName(wxString(raceName.c_str(), wxConvUTF8));
			race = raceRec.Get<unsigned int>(CharRacesDB::RaceID);
			gender = (genderName == "female" || genderName == "Female" || genderName == "FEMALE") ? 1 : 0;
		} catch (std::exception&) {
			// wtf
			race = 0;
			gender = 0;
		}

		// If the race is a goblin, then ignore this
		if (race == 9 && gameVersion < 40000) {
			g_modelViewer->optMenu->Check(ID_USE_NPCSKINS, true);
			return;
		}

		//  set our flag
		cd.useNPC = event.IsChecked();

		cd.maxSkinColor = chardb.getColorsFor(race, gender, CharSectionsDB::SkinType, 0, cd.useNPC);
		if (cd.maxSkinColor==0 && cd.useNPC==1) {
			wxMessageBox(_T("The selected character does not have any NPC skins!\nSwitching back to normal character skins."));
			cd.useNPC = 0;
			cd.maxSkinColor = chardb.getColorsFor(race, gender, CharSectionsDB::SkinType, 0, cd.useNPC);
		} else {
			cd.skinColor = 0;
			spins[0]->SetValue(0);
			spins[0]->SetRange(0, cd.maxSkinColor-1);
		}
	}
#endif

	//  Update controls associated
	RefreshEquipment();
	g_modelViewer->UpdateControls();	
	// ----
}

bool slotHasModel(int i)
{
	return (i==CS_HEAD || i==CS_SHOULDER || i==CS_HAND_LEFT || i==CS_HAND_RIGHT || i==CS_QUIVER);
}

void CharControl::RefreshEquipment()
{
	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		if (slotHasModel(i)) 
			RefreshItem(i);

		if (g_canvas->model->modelType != MT_NPC) {
			if (labels[i]) {
				labels[i]->SetLabel(CSConv(items.getByID(cd.equipment[i]).name));
				labels[i]->SetForegroundColour(ItemQualityColour(items.getByID(cd.equipment[i]).quality));
			}
		}
	}
}

void CharControl::OnButton(wxCommandEvent &event)
{
	if (event.GetId()==ID_CLEAR_EQUIPMENT) {
		for (int i=0; i<NUM_CHAR_SLOTS; i++) 
			cd.equipment[i] = 0;
		RefreshEquipment();

	} else if (event.GetId()==ID_LOAD_SET) {
		selectSet();

	} else if (event.GetId()==ID_LOAD_START) {
		selectStart();

	} else if (event.GetId()==ID_LOAD_NPC_START) {
		// TODO: a "Load NPC Outfit..." option to the character menu. It's now possible to equip a character with the exact NPC's gear (without weapons).		
//		selectNPC(UPDATE_NPC_START);
	} else if (event.GetId()==ID_MOUNT) {
		selectMount();

	} else {
		for (int i=0; i<NUM_CHAR_SLOTS; i++) {
			if (buttons[i] && (wxButton*)event.GetEventObject()==buttons[i]) {
				selectItem(UPDATE_ITEM, i, cd.equipment[i], buttons[i]->GetLabel().GetData());
				break;
			}
		}
	}

	if (g_canvas->model->modelType == MT_NPC)
		RefreshNPCModel();
	else
		RefreshModel();
}


const char* regionPaths[NUM_REGIONS] =
{
	"",
	"Item\\TextureComponents\\ArmUpperTexture\\",
	"Item\\TextureComponents\\ArmLowerTexture\\",
	"Item\\TextureComponents\\HandTexture\\",
	"",
	"",
	"Item\\TextureComponents\\TorsoUpperTexture\\",
	"Item\\TextureComponents\\TorsoLowerTexture\\",
	"Item\\TextureComponents\\LegUpperTexture\\",
	"Item\\TextureComponents\\LegLowerTexture\\",
	"Item\\TextureComponents\\FootTexture\\"
};

void CharControl::RefreshModel()
{
	hairTex = 0;
	furTex = 0;
	gobTex = 0;
	capeTex = 0;

	// Reset geosets
	for (int i=0; i<cd.NUM_GEOSETS; i++) 
		cd.geosets[i] = 1;
	cd.geosets[1] = cd.geosets[2] = cd.geosets[3] = 0;

	// show ears, if toggled
	if (cd.showEars) 
		cd.geosets[7] = 2;

	CharTexture tex;

	CharSectionsDB::Record rec = chardb.getRecord(0);

	// base character layer/texture
	try {
		rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::SkinType, 0, cd.skinColor, cd.useNPC);
		tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex1),wxConvUTF8), CR_BASE, 0);
		
		// Tauren fur
		wxString furTexName ( rec.getString(CharSectionsDB::Tex2),wxConvUTF8);
		if (!furTexName.IsEmpty())
			furTex = texturemanager.add(std::string(furTexName.mb_str()));

	} catch (std::exception&) {
		wxLogMessage(_T("Assertion base character Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
	}
#if 1 // for worgen female
		if (gameVersion >= 40000 && cd.race == 22 && cd.gender == 1) { // female worgen
			wxString fn;
			fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleSkin%02d_%02d.blp"), 0, cd.skinColor);
			if (FS().getSize(fn.mb_str()) > 0)
				tex.addLayer(fn, CR_BASE, 0);
			fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleSkin%02d_%02d_Extra.blp"), 0, cd.skinColor);
			if (FS().getSize(fn.mb_str()) > 0) {
				furTex = texturemanager.add(std::string(fn.mb_str()));
				model->textures[4] = furTex;
			}
		}
#endif // for worgen female

	// HACK: for goblin males, explicitly load a hair texture
	if (cd.race == 9 && cd.gender == 0 && gobTex == 0 && gameVersion < 40000) {
        gobTex = texturemanager.add("Creature\\Goblin\\Goblin.blp");		
	}

	// Hair related boolean flags
	bool bald = false;
	bool showHair = cd.showHair;
	bool showFacialHair = cd.showFacialHair;

	if (cd.race != 9 || gameVersion >= 40000) { // Goblin chars base texture already contains all this stuff.

		// Display underwear on the model?
		if (cd.showUnderwear) {
			try {
				rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::UnderwearType, 0, cd.skinColor, cd.useNPC);
				tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex1),wxConvUTF8), CR_PELVIS_UPPER, 1); // pants
				tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex2),wxConvUTF8), CR_TORSO_UPPER, 1); // top
			} catch (std::exception&) {
				wxLogMessage(_T("DBC underwear Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
			}
#if 1 // for worgen female
				if (gameVersion >= 40000 && cd.race == 22 && cd.gender == 1) { // female worgen
					wxString fn;
					fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleNakedPelvisSkin%02d_%02d.blp"), 0, cd.skinColor);
					if (FS().getSize(fn.mb_str()) > 0)
						tex.addLayer(fn, CR_PELVIS_UPPER, 1); // pants
					fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleNakedTorsoSkin%02d_%02d.blp"), 0, cd.skinColor);
					if (FS().getSize(fn.mb_str()) > 0)
						tex.addLayer(fn, CR_TORSO_UPPER, 1); // top
				}
#endif // for worgen female
		}

		// face
		try {
			rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::FaceType, cd.faceType, cd.skinColor, cd.useNPC);
			tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex1),wxConvUTF8), CR_FACE_LOWER, 1);
			tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex2),wxConvUTF8), CR_FACE_UPPER, 1);
		} catch (std::exception&) {
			wxLogMessage(_T("DBC face Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
#if 1 // for worgen female
			if (gameVersion >= 40000 && cd.race == 22 && cd.gender == 1) { // female worgen
				wxString fn;
				fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleFaceUpper%02d_%02d.blp"), cd.faceType, cd.skinColor);
				if (FS().getSize(fn.mb_str()) > 0)
					tex.addLayer(fn, CR_FACE_UPPER, 1);
				fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleFaceLower%02d_%02d.blp"), cd.faceType, cd.skinColor);
				if (FS().getSize(fn.mb_str()) > 0)
					tex.addLayer(fn, CR_FACE_LOWER, 1);
			}
#endif // for worgen female

		// facial feature geosets
		try {
			CharFacialHairDB::Record frec = facialhairdb.getByParams(cd.race, cd.gender, cd.facialHair);
			if (gameVersion == 40000) {
				cd.geosets[1] = frec.Get<unsigned int>(CharFacialHairDB::Geoset100V400);
				cd.geosets[2] = frec.Get<unsigned int>(CharFacialHairDB::Geoset200V400);
				cd.geosets[3] = frec.Get<unsigned int>(CharFacialHairDB::Geoset300V400);
			} else {
				cd.geosets[1] = frec.Get<unsigned int>(CharFacialHairDB::Geoset100);
				cd.geosets[2] = frec.Get<unsigned int>(CharFacialHairDB::Geoset200);
				cd.geosets[3] = frec.Get<unsigned int>(CharFacialHairDB::Geoset300);
			}
		} catch (std::exception&) {
			wxLogMessage(_T("DBC facial feature geosets Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}

		// facial feature
		try {
			rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::FacialHairType, cd.facialHair, cd.hairColor, 0);
			tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex1),wxConvUTF8), CR_FACE_LOWER, 2);
			tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex2),wxConvUTF8), CR_FACE_UPPER, 2);
		} catch (std::exception&) {
			wxLogMessage(_T("DBC facial feature Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
	
	}

	// select hairstyle geoset(s)
	for (CharHairGeosetsDB::Iterator it = hairdb.begin(); it != hairdb.end(); ++it) {
		if (it->Get<unsigned int>(CharHairGeosetsDB::Race)==cd.race && it->Get<unsigned int>(CharHairGeosetsDB::Gender)==cd.gender) {
			unsigned int id = it->Get<unsigned int>(CharHairGeosetsDB::Geoset);
			unsigned int section = it->Get<unsigned int>(CharHairGeosetsDB::Section);

			if (id!=0) {
				for (size_t j=0; j<model->geosets.size(); j++) {
					if (model->geosets[j].id == id) 
						model->showGeosets[j] = (cd.hairStyle == section) && showHair;
				}

			} else if (cd.hairStyle==section) {
				bald = true;
			}
		}
	}
#if 1 // for worgen female
	if (gameVersion >= 40000 && cd.race == 22 && cd.gender == 1) { // female worgen 
		for(int i=1; i<=21; i++) {
			int section = i - 1;
			for (size_t j=0; j<model->geosets.size(); j++) {
				if (model->geosets[j].id == i)
					model->showGeosets[j] = (cd.hairStyle==section) && showHair;
			}
		}
	}
#endif // for worgen female

	// hair
	try {
		rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::HairType, cd.hairStyle, cd.hairColor, 0);
		wxString hairTexfn ( rec.getString(CharSectionsDB::Tex1),wxConvUTF8);
		if (!hairTexfn.IsEmpty()) 
			hairTex = texturemanager.add(std::string(hairTexfn.mb_str()));
		else {
			// oops, looks like we're missing a hair texture. Let's try with hair style #0.
			// (only a problem for orcs with no hair but some beard
			try {
				rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::HairType, 0, cd.hairColor, 0);
				hairTexfn = wxString(rec.getString(CharSectionsDB::Tex1), wxConvUTF8);
				if (!hairTexfn.IsEmpty()) 
					hairTex = texturemanager.add(std::string(hairTexfn.mb_str()));
				else 
					hairTex = 0;
			} catch (std::exception&) {
				// oh well, give up.
				hairTex = 0; // or chartex?
			}
		}
		if (!bald) {
			tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex2), wxConvUTF8), CR_FACE_LOWER, 3);
			tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex3), wxConvUTF8), CR_FACE_UPPER, 3);
		}

	} catch (std::exception&) {
		wxLogMessage(_T("DBC hair Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		hairTex = 0;
	}
#if 1 // for worgen female
		if (gameVersion >= 40000 && cd.race == 22 && cd.gender == 1) { // female worgen
			wxString fn;
			fn.Printf(_T("Character\\Worgen\\Hair00_%02d.blp"), cd.hairColor);
			if (FS().getSize(fn.mb_str()) > 0) {
				hairTex = texturemanager.add(std::string(fn.mb_str()));
				model->textures[2] = hairTex;
			}
		}
#endif // for worgen female

	// If they have no hair, toggle the 'bald' flag.
	if (!showHair)
		bald = true;
	
	// Hide facial hair if it isn't toggled and they don't have tusks, horns, etc.
	if (!showFacialHair) {		
		try {
			CharRacesDB::Record race = getByID(racedb,cd.race);
			wxString tmp;
			if (gameVersion == 40000)
				tmp = wxString(race.getString(CharRacesDB::GeoType1V400), wxConvUTF8);
			else
				tmp = wxString(race.getString(CharRacesDB::GeoType1), wxConvUTF8);
			if (tmp.Lower() == _T("normal")) {
				cd.geosets[1] = 1;
				cd.geosets[2] = 1;
				cd.geosets[3] = 1;
			}
		} catch (std::exception&) {
			wxLogMessage(_T("Assertion FacialHair Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
      }

	// check if we have a robe on
	bool hadRobe = false;
	if (cd.equipment[CS_CHEST] != 0) {
		try {
			const ItemRecord &item = items.getByID(cd.equipment[CS_CHEST]);
			if (item.type==IT_ROBE || item.type==IT_CHEST) {
				ItemDisplayDB::Record r = get_by_ID(itemdisplaydb,item.model);
				if (r.robe_geoset_flags()==1)
					hadRobe = true;
			}
		} catch (std::exception&) {
			wxLogMessage(_T("Assertion robe Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
	}

	// check if we have a kilt on, just like our robes
	if (cd.equipment[CS_PANTS] != 0) {
		try {
			const ItemRecord &item = items.getByID(cd.equipment[CS_PANTS]);
			if (item.type==IT_PANTS) {
				ItemDisplayDB::Record r = get_by_ID(itemdisplaydb,item.model);
				if (r.robe_geoset_flags()==1)
					hadRobe = true;
			}
		} catch (std::exception&) {
			wxLogMessage(_T("Assertion kilt Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
	}

	// Default
	slotOrderWithRobe[7] = CS_CHEST;
	slotOrderWithRobe[8] = CS_GLOVES;

	// check the order of robe/gloves
	if (cd.equipment[CS_CHEST] && cd.equipment[CS_GLOVES]) {
		try {
			//const ItemRecord &item = items.getByID(cd.equipment[CS_CHEST]);
			//if (item.type==IT_ROBE) {
			//	ItemDisplayDB::Record r = itemdisplaydb.getByID(item.model);
				//if (r.Get<unsigned int>(ItemDisplayDB::GeosetA)>0) {
					const ItemRecord &item2 = items.getByID(cd.equipment[CS_GLOVES]);
					ItemDisplayDB::Record r2 = get_by_ID(itemdisplaydb,item2.model);
					if (r2.glove_geoset_flags()==0) {
						slotOrderWithRobe[7] = CS_GLOVES;
						slotOrderWithRobe[8] = CS_CHEST;
					}
				//}
			//}
		} catch (std::exception&) {
			wxLogMessage(_T("Assertion robe/gloves Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
	}
	
	// dressup
	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		int sn = hadRobe ? slotOrderWithRobe[i] : slotOrder[i];
		if (cd.equipment[sn] != 0) 
			AddEquipment(sn, cd.equipment[sn], 10+i, tex);
	}

	// reset geosets
	for (size_t j=0; j<model->geosets.size(); j++) {
		int id = model->geosets[j].id;

		// hide top-of-head if we have hair.
		if (id == 1)
			model->showGeosets[j] = bald;

		for (int i=1; i<19; i++) {
			int a = i*100, b = (i+1) * 100;
			if (id>a && id<b) 
				model->showGeosets[j] = (id == (a + cd.geosets[i]));
		}
	}

	// finalize character texture
	tex.compose(charTex);
	
	// set replacable textures
	model->replaceTextures[TEXTURE_BODY] = charTex;
	model->replaceTextures[TEXTURE_CAPE] = capeTex;
	model->replaceTextures[TEXTURE_HAIR] = hairTex;
	model->replaceTextures[TEXTURE_FUR] = furTex;
	model->replaceTextures[TEXTURE_GAMEOBJECT1] = gobTex;

	/*
	for (int i=0; i<40; i++) {
		model->atts[i].dr = (model->atts[i].id==cd.hairStyle);
	}
	*/

	// Alfred 2009.07.18 show max value
//	for (int i=0; i<NUM_SPIN_BTNS; i++)
//		spinLabels[i]->SetLabel(wxString::Format(_T("%i / %i"), spins[i]->GetValue(), spins[i]->GetMax()));

//	spins[SPIN_SKIN_COLOR]->SetValue(cd.skinColor);
//	spins[SPIN_FACE_TYPE]->SetValue(cd.faceType);
//	spins[SPIN_HAIR_COLOR]->SetValue(cd.hairColor);
//	spins[SPIN_HAIR_STYLE]->SetValue(cd.hairStyle);
//	spins[SPIN_FACIAL_HAIR]->SetValue(cd.facialHair);
//	spins[SPIN_FACIAL_COLOR]->SetValue(cd.facialColor);

	// eyeglow for Scourge
	if (cd.race == 5 && bKnightEyeGlow) {
		if (cd.gender == 0 && model->passes.size() > 52 && model->showGeosets[51] == false && model->showGeosets[52] == false)
			model->showGeosets[51] = true;
		else if (cd.gender == 1 && model->passes.size() > 47 && model->showGeosets[45] == false && model->showGeosets[47] == false)
			model->showGeosets[45] = true;
	}

	new_cc->setVisible(true);
	new_ec->setVisible(true);
}

void CharControl::RefreshNPCModel()
{
	hairTex = 0;
	furTex = 0;
	gobTex = 0;
	capeTex = 0;

	// Reset geosets
	for (int i=0; i<cd.NUM_GEOSETS; i++) 
		cd.geosets[i] = 1;
	cd.geosets[1] = cd.geosets[2] = cd.geosets[3] = 0;

	// show ears, if toggled
	if (cd.showEars) 
		cd.geosets[7] = 2;

	CharTexture tex;

	// Open first record to declare var
	CharSectionsDB::Record rec = chardb.getRecord(0);
	// It is VITAL that this record can be retrieved to display the NPC
	try {
		rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::SkinType, 0, cd.skinColor, cd.useNPC);
	} catch (...) {
		wxLogMessage(_T("DBC Error: %s : line #%i : %s\n\tAttempting to use character base colour."), __FILE__, __LINE__, __FUNCTION__);
		try {
			rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::SkinType, 0, 0, cd.useNPC);
		} catch (...) {
			wxLogMessage(_T("Exception Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
			return;
		}
	}

	// base layer texture
	try {
		if (!customSkin.IsEmpty()) {
			tex.addLayer(customSkin, CR_BASE, 0);
		} else { 
			tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex1), wxConvUTF8), CR_BASE, 0);

			if (cd.showUnderwear) {
				rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::UnderwearType, 0, cd.skinColor, cd.useNPC);
				tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex1), wxConvUTF8), CR_PELVIS_UPPER, 1); // panties
				tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex1), wxConvUTF8), CR_PELVIS_UPPER, 1); // panties
			}

			// face
			rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::FaceType, cd.faceType, cd.skinColor, cd.useNPC);
			tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex1), wxConvUTF8), CR_FACE_LOWER, 1);
			tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex2), wxConvUTF8), CR_FACE_UPPER, 1);

			// facial hair
			rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::FacialHairType, cd.facialHair, cd.hairColor, 0);
			tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex1), wxConvUTF8), CR_FACE_LOWER, 2);
			tex.addLayer(wxString(rec.getString(CharSectionsDB::Tex2), wxConvUTF8), CR_FACE_UPPER, 2);
		} 

		// Tauren fur
		wxString furTexName = wxString(rec.getString(CharSectionsDB::Tex2), wxConvUTF8);
		if (!furTexName.IsEmpty())
			furTex = texturemanager.add(std::string(furTexName.mb_str()));

	} catch (...) {
		wxLogMessage(_T("Exception base layer Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
	}

	// hair
	try {
		CharSectionsDB::Record rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::HairType, cd.hairStyle, cd.hairColor, cd.useNPC);
		wxString hairTexfn = wxString(rec.getString(CharSectionsDB::Tex1), wxConvUTF8);
		if (!hairTexfn.IsEmpty()) {
			hairTex = texturemanager.add(std::string(hairTexfn.mb_str()));
		} else {
			rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::HairType, 1, cd.hairColor, cd.useNPC);
			hairTexfn = wxString(rec.getString(CharSectionsDB::Tex1), wxConvUTF8);
			if (!hairTexfn.IsEmpty()) 
				hairTex = texturemanager.add(std::string(hairTexfn.mb_str()));
			else 
				hairTex = 0;
		}

		//tex.addLayer(rec.getString(CharSectionsDB::Tex2), CR_FACE_LOWER, 3);
		//tex.addLayer(rec.getString(CharSectionsDB::Tex3), CR_FACE_UPPER, 3);

	} catch (std::exception&) {
		wxLogMessage(_T("Assertion hair Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		hairTex = 0;
	}
	
	bool bald = false;
	bool showHair = cd.showHair;
	bool showFacialHair = cd.showFacialHair;

	// facial hair geosets
	try {
		CharFacialHairDB::Record frec = facialhairdb.getByParams(cd.race, cd.gender, cd.facialHair);
		if (gameVersion == 40000) {
			cd.geosets[1] = frec.Get<unsigned int>(CharFacialHairDB::Geoset100);
			cd.geosets[2] = frec.Get<unsigned int>(CharFacialHairDB::Geoset200);
			cd.geosets[3] = frec.Get<unsigned int>(CharFacialHairDB::Geoset300);
		} else {
			cd.geosets[1] = frec.Get<unsigned int>(CharFacialHairDB::Geoset100V400);
			cd.geosets[2] = frec.Get<unsigned int>(CharFacialHairDB::Geoset200V400);
			cd.geosets[3] = frec.Get<unsigned int>(CharFacialHairDB::Geoset300V400);
		}

		// Hide facial fair if it isn't toggled and they don't have tusks, horns, etc.
		if (showFacialHair == false) {		
			CharRacesDB::Record race = getByID(racedb,cd.race);
			wxString tmp;
			if (gameVersion == 40000)
				tmp = wxString(race.getString(CharRacesDB::GeoType1V400), wxConvUTF8);
			else
				tmp = wxString(race.getString(CharRacesDB::GeoType1), wxConvUTF8);
			if (tmp.Lower() == _T("normal")) {
				cd.geosets[1] = 1;
				cd.geosets[2] = 1;
				cd.geosets[3] = 1;
			}
		}
	} catch (std::exception&) {
		wxLogMessage(_T("Assertion facial hair Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
	}

	// select hairstyle geoset(s)
	for (CharHairGeosetsDB::Iterator it = hairdb.begin(); it != hairdb.end(); ++it) {
		if (it->Get<unsigned int>(CharHairGeosetsDB::Race)==cd.race && it->Get<unsigned int>(CharHairGeosetsDB::Gender)==cd.gender) {
			unsigned int id = it->Get<unsigned int>(CharHairGeosetsDB::Geoset);
			unsigned int section = it->Get<unsigned int>(CharHairGeosetsDB::Section);
			if (id!=0) {
				for (size_t j=0; j<model->geosets.size(); j++) {
					if (model->geosets[j].id == id)
						model->showGeosets[j] = (cd.hairStyle==section) && showHair;
				}
			} else if (cd.hairStyle==section) 
				bald = true;
		}
	}

	// If they have no hair, toggle the 'bald' flag.
	if (!showHair)
		bald = true;
	
	// check if we have a robe on
	bool hadRobe = false;
	if (cd.equipment[CS_CHEST] != 0) {
		try {
			const ItemRecord &item = items.getByID(cd.equipment[CS_CHEST]);
			if (item.type==IT_ROBE || item.type==IT_CHEST) {
				ItemDisplayDB::Record r = get_by_ID(itemdisplaydb,item.model);
				if (r.robe_geoset_flags()==1)
					hadRobe = true;
			}
		} catch (...) {
			wxLogMessage(_T("Exception robe Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
	}
	

	// check if we have a kilt on, just like our robes
	if (cd.equipment[CS_PANTS] != 0) {
		try {
			const ItemRecord &item = items.getByID(cd.equipment[CS_PANTS]);
			int type = item.type;
			if (type==IT_PANTS) {
				ItemDisplayDB::Record r = get_by_ID(itemdisplaydb,item.model);
				if (r.robe_geoset_flags()==1)
					hadRobe = true;
			}
		} catch (...) {
			wxLogMessage(_T("Exception Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
	}

	// Default
	slotOrderWithRobe[7] = CS_CHEST;
	slotOrderWithRobe[8] = CS_GLOVES;

	// check the order of robe/gloves
	if (cd.equipment[CS_CHEST] && cd.equipment[CS_GLOVES]) {
		try {
			//const ItemRecord &item = items.getByID(cd.equipment[CS_CHEST]);
			//if (item.type==IT_ROBE) {
			//	ItemDisplayDB::Record r = itemdisplaydb.getByID(item.model);
				//if (r.Get<unsigned int>(ItemDisplayDB::GeosetA)>0) {
					ItemDisplayDB::Record r = get_by_ID(itemdisplaydb,cd.equipment[CS_GLOVES]);
					if (r.glove_geoset_flags()==0) {
						slotOrderWithRobe[7] = CS_GLOVES;
						slotOrderWithRobe[8] = CS_CHEST;
					}
				//}
			//}
		} catch (...) {
			wxLogMessage(_T("Exception Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
	}

	// dressup
	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		int sn = hadRobe ? slotOrderWithRobe[i] : slotOrder[i];
		if (cd.equipment[sn] != 0) 
			AddEquipment(sn, cd.equipment[sn], 10+i, tex, false);
	}
	

	// reset geosets
	for (size_t j=0; j<model->geosets.size(); j++) {
		int id = model->geosets[j].id;
		
		// hide top-of-head if we have hair.
		if (id == 1) 
			model->showGeosets[j] = bald;

		for (int i=1; i<19; i++) {
			int a = i*100, b = (i+1) * 100;
			if (id>a && id<b) 
				model->showGeosets[j] = (id == (a + cd.geosets[i]));
		}
	}

	// finalize texture
	tex.compose(charTex);
	
	// set replacable textures
	model->replaceTextures[TEXTURE_BODY] = charTex;
	model->replaceTextures[TEXTURE_CAPE] = capeTex;
	model->replaceTextures[TEXTURE_HAIR] = hairTex;
	model->replaceTextures[TEXTURE_FUR] = furTex;
	model->replaceTextures[TEXTURE_GAMEOBJECT1] = gobTex;
}

void CharControl::AddEquipment(int slot, int itemnum, int layer, CharTexture &tex, bool lookup)
{
	if (slot==CS_PANTS && cd.geosets[13]==2) 
		return; // if we are wearing a robe, no pants for us! ^_^

	try {
		const ItemRecord &item = items.getByID(itemnum);
		int type = item.type;
		int itemID = 0;

		if (lookup)
			itemID = item.model;
		else
			itemID = itemnum;

		ItemDisplayDB::Record r = get_by_ID(itemdisplaydb,itemID);
		
		// Just a rough check to make sure textures are only being added to where they're suppose to.
		if (slot == CS_CHEST || slot == CS_SHIRT) {
			cd.geosets[8] = 1 + r.glove_geoset_flags();

			tex.addLayer(makeItemTexture(CR_ARM_UPPER, wxString(r.upper_arm_texture(), wxConvUTF8)), CR_ARM_UPPER, layer);
			tex.addLayer(makeItemTexture(CR_ARM_LOWER, wxString(r.lower_arm_texture(), wxConvUTF8)), CR_ARM_LOWER, layer);

			tex.addLayer(makeItemTexture(CR_TORSO_UPPER, wxString(r.upper_torso_texture(), wxConvUTF8)), CR_TORSO_UPPER, layer);
			tex.addLayer(makeItemTexture(CR_TORSO_LOWER, wxString(r.lower_torso_texture(), wxConvUTF8)), CR_TORSO_LOWER, layer);

			if (type == IT_ROBE || r.robe_geoset_flags()==1) {
				tex.addLayer(makeItemTexture(CR_LEG_UPPER, wxString(r.upper_leg_texture(), wxConvUTF8)), CR_LEG_UPPER, layer);
				tex.addLayer(makeItemTexture(CR_LEG_LOWER, wxString(r.lower_leg_texture(), wxConvUTF8)), CR_LEG_LOWER, layer);
			}
		} /*else if (slot == CS_SHOULDER)
			tex.addLayer(makeItemTexture(CR_ARM_UPPER, r.getString(ItemDisplayDB::TexArmUpper)), CR_ARM_UPPER, layer);
		*/else if (slot == CS_BELT) {
			// Alfred 2009.08.15 add torso_lower for Titan-Forged Waistguard of Triumph
			tex.addLayer(makeItemTexture(CR_TORSO_LOWER, wxString(r.lower_torso_texture(), wxConvUTF8)), CR_TORSO_LOWER, layer);
			tex.addLayer(makeItemTexture(CR_LEG_UPPER, wxString(r.upper_leg_texture(), wxConvUTF8)), CR_LEG_UPPER, layer);
		} else if (slot == CS_BRACERS)
			tex.addLayer(makeItemTexture(CR_ARM_LOWER, wxString(r.lower_arm_texture(), wxConvUTF8)), CR_ARM_LOWER, layer);
		else if (slot == CS_PANTS) {
			cd.geosets[9] = 1 + r.bracer_geoset_flags();

			tex.addLayer(makeItemTexture(CR_LEG_UPPER, wxString(r.upper_leg_texture(), wxConvUTF8)), CR_LEG_UPPER, layer);
			tex.addLayer(makeItemTexture(CR_LEG_LOWER, wxString(r.lower_leg_texture(), wxConvUTF8)), CR_LEG_LOWER, layer);
		} else if (slot == CS_GLOVES) {
			cd.geosets[4] = 1 + r.glove_geoset_flags();

			tex.addLayer(makeItemTexture(CR_HAND, wxString(r.hands_texture(), wxConvUTF8)), CR_HAND, layer);
			tex.addLayer(makeItemTexture(CR_ARM_LOWER, wxString(r.lower_arm_texture(), wxConvUTF8)), CR_ARM_LOWER, layer);
		} else if (slot == CS_BOOTS) { // && cd.showFeet==false) {
			cd.geosets[5] = 1 + r.glove_geoset_flags();

			tex.addLayer(makeItemTexture(CR_LEG_LOWER, wxString(r.lower_leg_texture(), wxConvUTF8)), CR_LEG_LOWER, layer);
			if (!cd.showFeet)
				tex.addLayer(makeItemTexture(CR_FOOT, wxString(r.foot_texture(), wxConvUTF8)), CR_FOOT, layer);
		} else if (slot==CS_TABARD && td.showCustom) { // Display our customised tabard
			cd.geosets[12] = 2;
			tex.addLayer(wxString(td.GetBackgroundTex(CR_TORSO_UPPER).c_str(), wxConvUTF8), CR_TORSO_UPPER, layer);
			tex.addLayer(wxString(td.GetBackgroundTex(CR_TORSO_LOWER).c_str(), wxConvUTF8), CR_TORSO_LOWER, layer);
			tex.addLayer(wxString(td.GetIconTex(CR_TORSO_UPPER).c_str(), wxConvUTF8), CR_TORSO_UPPER, layer);
			tex.addLayer(wxString(td.GetIconTex(CR_TORSO_LOWER).c_str(), wxConvUTF8), CR_TORSO_LOWER, layer);
			tex.addLayer(wxString(td.GetBorderTex(CR_TORSO_UPPER).c_str(), wxConvUTF8), CR_TORSO_UPPER, layer);
			tex.addLayer(wxString(td.GetBorderTex(CR_TORSO_LOWER).c_str(), wxConvUTF8), CR_TORSO_LOWER, layer);

		} else if (slot==CS_TABARD) { // if its just a normal tabard then do the usual
			cd.geosets[12] = 2;
			tex.addLayer(makeItemTexture(CR_TORSO_UPPER, wxString(r.upper_torso_texture(), wxConvUTF8)), CR_TORSO_UPPER, layer);
			tex.addLayer(makeItemTexture(CR_TORSO_LOWER, wxString(r.lower_torso_texture(), wxConvUTF8)), CR_TORSO_LOWER, layer);
		
		} else if (slot==CS_CAPE) { // capes
			cd.geosets[15] = 1 + r.glove_geoset_flags();

			// load the cape texture
			wxString tex ( r.skin(0), wxConvUTF8);
			if (!tex.IsEmpty()) 
				capeTex = texturemanager.add(std::string(AnimControl::makeSkinTexture("Item\\ObjectComponents\\Cape\\",tex.mb_str()).mb_str()));
		}

		// robe
		if (cd.geosets[13]==1) 
			cd.geosets[13] = 1 + r.robe_geoset_flags();
		if (cd.geosets[13]==2) {
			cd.geosets[5] = 0;		// hide the boots
			//cd.geosets[9] = 0;		// hide the pants
			cd.geosets[12] = 0;		// also hide the tabard.
		}

		// gloves - this is so gloves have preference over shirt sleeves.
		if (cd.geosets[4] > 1) 
			cd.geosets[8] = 0;

	} catch (std::exception&) {}
}

void CharControl::RefreshItem(int slot)
{	
	if (!charAtt)
		return;

	if (slot==CS_HEAD && bHideHelmet == true)
		return;

	// delete all attachments in that slot
	charAtt->delSlot(slot);

	int itemnum = cd.equipment[slot];
	if (itemnum!=0) {
		// load new model(s)
		int id1=-1, id2=-1;
		wxString path;
		//float sc = 1.0f;

		if (slot==CS_HEAD) {
			id1 = PS_HELMET;
			path = _T("Item\\ObjectComponents\\Head\\");
		} else if (slot==CS_SHOULDER) {
			id1 = PS_LEFT_SHOULDER;
			id2 = PS_RIGHT_SHOULDER;
			path = _T("Item\\ObjectComponents\\Shoulder\\");
		} else if (slot == CS_HAND_LEFT) {
			id1 = PS_LEFT_PALM;
			model->charModelDetails.closeLHand = true;
		} else if (slot == CS_HAND_RIGHT) {
			id1 = PS_RIGHT_PALM;
			model->charModelDetails.closeRHand = true;
		} else if (slot == CS_QUIVER) {
			id1 = PS_RIGHT_BACK_SHEATH;
			path = _T("Item\\ObjectComponents\\Quiver\\");
		} else 
			return;

		if (slot==CS_HAND_LEFT || slot==CS_HAND_RIGHT) {
			if (items.getByID(itemnum).type == IT_SHIELD) {
				path = _T("Item\\ObjectComponents\\Shield\\");
				id1 = PS_LEFT_WRIST;
			} else {
				path = _T("Item\\ObjectComponents\\Weapon\\");
			}

			// If we're sheathing our weapons, relocate the items to
			// their correct positions
			if (bSheathe && items.getByID(itemnum).sheath>0) {
				id1 = items.getByID(itemnum).sheath;
				if (id1==PS_LEFT_HIP_SHEATH && slot==CS_HAND_LEFT)
					id1 = PS_RIGHT_HIP_SHEATH;

				// One-handed Sword 2:7 && items.getByID(itemnum).subclass==7
				if (id1==PS_RIGHT_BACK_SHEATH && slot==CS_HAND_LEFT)
					id1 = PS_LEFT_BACK_SHEATH;

				if (slot==CS_HAND_LEFT)
					model->charModelDetails.closeLHand = false;
				if (slot==CS_HAND_RIGHT)
					model->charModelDetails.closeRHand = false;

				/* in itemcache.wdb & item.dbc
				0	 None	 Used on Armor, non-weapon items.
				1	 Angled Back	 Used on two-handed swords/axes, and some one-handers such as Thunderfury.
				2	 Upper Back	 Used on staffs and polearms, positioned higher and straighter.
				3	 Side	 Used on one-handed maces,swords,axes,daggers.
				4	 Back	 Used for shields
				5	 ?	
				6	 	
				7	 Invisible	 Used for fist weapons and offhands.
				*/
				/*
				26 = upper right back, two-handed sword(2:8) sheath1
				27 = upper left back, 
				28 = center back, shield(4:6), sheath4
				30 = upside down, upper left back -- staff(2:10) sheath2, spears
				32 = left hip, mace(2:4) sheath3, sword(2:7) sheath3
				33 = right hip
				*/
			}
		}

		try {

			// This corrects the problem with trying to automatically load equipment on NPC's
			int ItemID = 0;
			if (g_canvas->model->modelType == MT_NPC)
				ItemID = itemnum;
			else {
				const ItemRecord &item = items.getByID(itemnum);
				ItemID = item.model;
			}
			
			ItemDisplayDB::Record r = get_by_ID(itemdisplaydb,ItemID);

			GLuint tex;
			std::string mp;
			bool succ = false;
			Attachment *att = NULL;
			Model *m = NULL;

			if (id1>=0) {
				mp = (std::string(path.mb_str()) + r.model(0));

				if (slot==CS_HEAD) {
					// sigh, head items have more crap to sort out
					mp = mp.substr(0, mp.length()-4); // delete .mdx
					mp.append("_");
					try {
						CharRacesDB::Record race = getByID(racedb,cd.race);
						mp.append(race.getString(CharRacesDB::ShortName));
						mp.append(cd.gender?"F":"M");
						mp.append(".m2");
					} catch (std::exception&) {
						mp = "";
					}
				}

				if (mp.length()) {
					att = charAtt->addChild(mp.c_str(), id1, slot);
					if (att) {
						m = static_cast<Model*>(att->model);
						if (m->ok) {
							mp = (std::string(path.mb_str()) + r.skin(0));
							mp.append(".blp");
							tex = texturemanager.add(mp);
							m->replaceTextures[TEXTURE_CAPE] = tex;
							
							succ = true;
						}
					}
				}
			}
			if (id2>=0) {
				mp = (std::string(path.mb_str()) + r.model(1));
				if (mp.length()) {
					att = charAtt->addChild(mp.c_str(), id2, slot);
					if (att) {
						m = static_cast<Model*>(att->model);
						if (m->ok) {
							mp = (std::string(path.mb_str()) + r.skin(1));
							mp.append(".blp");
							tex = texturemanager.add(mp);
							m->replaceTextures[TEXTURE_CAPE] = tex;

							succ = true;
						}
					}
				}
			}

			if (succ) {
				// Manual position correction of items equipped on the back, staves, 2h weapons, quivers, etc.
				//if (id1 > 25 && id1 < 32)
				//	att->pos = Vec3D(0.0f, 0.0f, 0.06f);

				// okay, see if we have any glowy effects
				int visualid = r.visuals();
				
				if (visualid == 0) {
					if ((g_modelViewer->enchants->RHandEnchant > -1) && (slot == CS_HAND_RIGHT)) {
						visualid = g_modelViewer->enchants->RHandEnchant;
					} else if ((g_modelViewer->enchants->LHandEnchant > -1) && (slot == CS_HAND_LEFT)) {
						visualid = g_modelViewer->enchants->LHandEnchant;
					}
				}

				if (m == NULL)
					m = static_cast<Model*>(att->model);

				if (visualid > 0) {
					try {
						ItemVisualDB::Record vis = getByID(visualdb,visualid);
						for (int i=0; i<5; i++) {
							// try all five visual slots
							int effectid = vis.Get<int>(ItemVisualDB::Effect1 + i);
							if (effectid==0 || m->attLookup[i]<0) continue;

							try {
								ItemVisualEffectDB::Record eff = getByID(effectdb,effectid);
								wxString filename ( eff.getString(ItemVisualEffectDB::Model),wxConvUTF8);

								att->addChild(filename.mb_str(), i, -1);

							} catch (std::exception&) {}
						}
					} catch (std::exception&) {}
				}

			} else {
				cd.equipment[slot] = 0; // no such model? :(
			}

		} catch (std::exception&) {}
	}
}

void CharControl::RefreshCreatureItem(int slot)
{
	// delete all attachments in that slot
	g_canvas->root->delSlot(slot);

	int itemnum = cd.equipment[slot];
	if (itemnum!=0) {
		// load new model(s)
		int id1=-1;
		wxString path;
		//float sc = 1.0f;

		if (slot == CS_HAND_LEFT) 
			id1 = PS_LEFT_PALM;
		else if (slot == CS_HAND_RIGHT) 
			id1 = PS_RIGHT_PALM;
		else 
			return;

		if (slot==CS_HAND_LEFT || slot==CS_HAND_RIGHT) {
			if (items.getByID(itemnum).type == IT_SHIELD) {
				path = _T("Item\\ObjectComponents\\Shield\\");
				id1 = PS_LEFT_WRIST;
			} else {
				path = _T("Item\\ObjectComponents\\Weapon\\");
			}
		}

		try {
			const ItemRecord &item = items.getByID(itemnum);
			ItemDisplayDB::Record r = get_by_ID(itemdisplaydb,item.model);

			GLuint tex;
			std::string mp;
			bool succ = false;
			Attachment *att = NULL;
			Model *m = NULL;

			if (id1>=0) {
				mp = (std::string(path.mb_str()) + r.model(0));

				if (mp.length()) {
					att = g_canvas->root->addChild(mp.c_str(), id1, slot);
					if (att) {
						m = static_cast<Model*>(att->model);
						if (m->ok) {
							mp = (std::string(path.mb_str()) + r.skin(0));
							mp.append(".blp");
							tex = texturemanager.add(mp);
							m->replaceTextures[TEXTURE_CAPE] = tex;
							succ = true;
						}
					}
				}
			}

			if (succ) {
				// okay, see if we have any glowy effects
				int visualid = r.visuals();
				
				if (visualid == 0) {
					if ((g_modelViewer->enchants->RHandEnchant > -1) && (slot == CS_HAND_RIGHT)) {
						visualid = g_modelViewer->enchants->RHandEnchant;
					} else if ((g_modelViewer->enchants->LHandEnchant > -1) && (slot == CS_HAND_LEFT)) {
						visualid = g_modelViewer->enchants->LHandEnchant;
					}
				}

				if (m == NULL)
					m = static_cast<Model*>(att->model);

				if (visualid > 0) {
					try {
						ItemVisualDB::Record vis = getByID(visualdb,visualid);
						for (int i=0; i<5; i++) {
							// try all five visual slots
							int effectid = vis.Get<int>(ItemVisualDB::Effect1 + i);
							if (effectid==0 || m->attLookup[i]<0) continue;

							try {
								ItemVisualEffectDB::Record eff = getByID(effectdb,effectid);
								wxString filename ( eff.getString(ItemVisualEffectDB::Model),wxConvUTF8);

								att->addChild(filename.mb_str(), i, -1);

							} catch (std::exception&) {}
						}
					} catch (std::exception&) {}
				}

			} else {
				cd.equipment[slot] = 0; // no such model? :(
			}

		} catch (std::exception&) {}
	}
}

wxString CharControl::makeItemTexture(int region, const wxString name)
{
//	wxString name(param, wxConvUTF8);

	// just return an empty filename
	if (name.Length() < 3) 
		return wxEmptyString;

	wxChar leggings = name[name.Length() - 2];
	
	// try prefered version first
	wxString fn(regionPaths[region], wxConvUTF8);
	fn += name;
	fn += _T("_");

	if (leggings == 'l' || leggings == 'L')
		fn += _T("U");
	else
		fn += cd.gender ? _T("F") : _T("M");
		
	fn += _T(".blp");
	if (FS().getSize(fn.fn_str()) > 0)  //MPQFile::exists(fn.c_str()) &&
		return fn;

	if (fn.Length() < 5)
		return wxEmptyString;

	// if that failed try alternate texture version
	if (leggings == 'l' || leggings == 'L')
		fn[fn.Length()-5] = cd.gender ? 'F' : 'M';
	else
		fn[fn.Length()-5] = 'U';

	if (FS().getSize(fn.fn_str()) > 0) //MPQFile::exists(fn.c_str()) &&
		return fn;

	fn = wxString(regionPaths[region], wxConvUTF8);
	fn += name;	
	fn += _T(".blp");

	// return the default name, nothing else could be found.
	return fn;
}

// 2007.07.03 Alfred, enlarge buf size and make it static to prevent stack overflow
//static unsigned char destbuf[REGION_PX*REGION_PX*4], tempbuf[REGION_PX*REGION_PX*4];
void CharTexture::compose(TextureID texID)
{
	// if we only have one texture then don't bother with compositing
	if (components.size() == 1)
	{
		Texture temp(std::string(components[0].name.mb_str()));
		texturemanager.LoadBLP(texID, &temp);
		return;
	}

	std::sort(components.begin(), components.end());


	std::vector<unsigned char> destbuf(REGION_PX * REGION_PX * 4);
	std::fill(destbuf.begin(), destbuf.end(), 0);

	std::vector<unsigned char> tempbuf;

	for (std::vector<CharTextureComponent>::iterator it = components.begin(); it
		!= components.end(); ++it)
	{
		CharTextureComponent &comp = *it;
		const CharRegionCoords &coords = regions[comp.region];
		TextureID temptex = texturemanager.add(std::string(comp.name.mb_str()));
		Texture &tex = *((Texture*) texturemanager.items[temptex]);

		// Alfred 2009.07.03, tex width or height can't be zero
		if (tex.w == 0 || tex.h == 0)
		{
			texturemanager.del(temptex);
			continue;
		}

		tempbuf.resize(tex.w * tex.h * 4);

		tex.getPixels(&tempbuf[0]);

		// blit the texture region over the original
		for (int y = 0, dy = coords.ypos; y < coords.ysize; y++, dy++)
		{
			for (int x = 0, dx = coords.xpos; x < coords.xsize; x++, dx++)
			{
				int sx = (x * tex.w) / coords.xsize;
				int sy = (y * tex.h) / coords.ysize;

				unsigned char *src = &tempbuf[0] + sy * tex.w * 4 + sx * 4;
				unsigned char *dest = &destbuf[0] + dy * REGION_PX * 4 + dx * 4;

				// this is slow and ugly but I don't care
				float r = src[3] / 255.0f;
				float ir = 1.0f - r;
				// zomg RGBA?
				dest[0] = (unsigned char) (dest[0] * ir + src[0] * r);
				dest[1] = (unsigned char) (dest[1] * ir + src[1] * r);
				dest[2] = (unsigned char) (dest[2] * ir + src[2] * r);
				dest[3] = 255;
			}
		}

		texturemanager.del(temptex);
	}

	// good, upload this to video
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, REGION_PX, REGION_PX, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, &destbuf[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void CharDetails::reset()
{
	skinColor = 0;
	faceType = 0;
	hairColor = 0;
	hairStyle = 0;
	facialHair = 0;

	showUnderwear = true;
	showHair = true;
	showFacialHair = true;
	showEars = true;
	showFeet = false;

	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		equipment[i] = 0;
	}
}


bool correctType(int type, int slot)
{
	if (type == IT_ALL) 
		return true;

	switch (slot) {
	case CS_HEAD:		return (type == IT_HEAD);
//	case CS_NECK:		return (type == IT_NECK);
	case CS_SHOULDER:	return (type == IT_SHOULDER);
	case CS_SHIRT:		return (type == IT_SHIRT);
	case CS_CHEST:		return (type == IT_CHEST || type == IT_ROBE);
	case CS_BELT:		return (type == IT_BELT);
	case CS_PANTS:		return (type == IT_PANTS);
	case CS_BOOTS:		return (type == IT_BOOTS);
	case CS_BRACERS:	return (type == IT_BRACERS);
	case CS_GLOVES:		return (type == IT_GLOVES);

	// Slight correction.  Type 21 = Lefthand weapon, Type 22 = Righthand weapon
	//case CS_HAND_RIGHT:	return (type == IT_1HANDED || type == IT_GUN || type == IT_THROWN || type == IT_2HANDED || type == IT_CLAW || type == IT_DAGGER);
	//case CS_HAND_LEFT:	return (type == IT_1HANDED || type == IT_BOW || type == IT_SHIELD || type == IT_2HANDED || type == IT_CLAW || type == IT_DAGGER || type == IT_OFFHAND);
	case CS_HAND_RIGHT:	return (type == IT_LEFTHANDED || type == IT_GUN || type == IT_THROWN || type == IT_2HANDED || type == IT_DAGGER);
	case CS_HAND_LEFT:	return (type == IT_RIGHTHANDED || type == IT_BOW || type == IT_SHIELD || type == IT_2HANDED || type == IT_DAGGER || type == IT_OFFHAND);
	case CS_CAPE:		return (type == IT_CAPE);
	case CS_TABARD:		return (type == IT_TABARD);
	case CS_QUIVER:		return (type == IT_QUIVER);
	}
	return false;
}


void CharControl::ClearItemDialog()
{
	if (itemDialog) {
		itemDialog->Show(FALSE);
		itemDialog->Destroy();
		wxDELETE(itemDialog);
	}
}

void CharControl::selectItem(int type, int slot, int current, const wxChar *caption)
{
	ClearItemDialog();

	numbers.clear();
	choices.Clear();

	std::vector<int> quality;

	// collect all items for this slot, making note of the occurring subclasses
	std::set<std::pair<int,int> > subclassesFound;
	
	int sel=0, ord=0;
	for (std::vector<ItemRecord>::iterator it = items.items.begin(); it != items.items.end(); ++it) {
		if (type == UPDATE_SINGLE_ITEM) {
			if (/*it->type == IT_HEAD ||*/ it->type == IT_SHOULDER || it->type == IT_SHIELD || 
				it->type == IT_BOW || it->type == IT_2HANDED || it->type == IT_LEFTHANDED || 
				it->type == IT_RIGHTHANDED || it->type == IT_OFFHAND || it->type == IT_GUN) {
				choices.Add(CSConv(it->name));
				numbers.push_back(it->id);
				quality.push_back(it->quality);

				subclassesFound.insert(std::pair<int,int>(it->itemclass,it->subclass));
			}
		}
		else if (correctType(it->type, slot)) {
			choices.Add(CSConv(it->name));
			numbers.push_back(it->id);
			quality.push_back(it->quality);

			if (it->id == current) 
				sel = ord;

			ord++;

			if (it->itemclass > 0) 
				subclassesFound.insert(std::pair<int,int>(it->itemclass,it->subclass));
		}
	}

	// make category list
	cats.clear();
	catnames.clear();

	std::map<std::pair<int,int>, int> subclasslookup;
	for (ItemSubClassDB::Iterator it=subclassdb.begin(); it != subclassdb.end(); ++it) {
		int cl = it->Get<int>(ItemSubClassDB::ClassID);
		int scl = it->Get<int>(ItemSubClassDB::SubClassID);
		// only add the subclass if it was found in the itemlist
		if (cl>0 && subclassesFound.find(std::pair<int,int>(cl,scl)) != subclassesFound.end()) {
			
			// Used to go through the 'string fields' looking for the one with data.
			wxString str;
			if (gameVersion == 40000)
				str = CSConv(wxString(it->getString(ItemSubClassDB::NameV400 + langOffset),wxConvUTF8));
			else
				str = CSConv(wxString(it->getString(ItemSubClassDB::Name + langOffset),wxConvUTF8));

			int hands;
			if (gameVersion == 40000)
				hands = it->Get<int>(ItemSubClassDB::HandsV400);
			else
				hands = it->Get<int>(ItemSubClassDB::Hands);
			if (hands > 0) {
				str.append(wxString::Format(_T(" (%d-handed)"), hands));
			}
			catnames.Add(str);
			subclasslookup[std::pair<int,int>(cl,scl)] = (int)catnames.size()-1;
		}
	}

	if (subclassesFound.size() > 1) {
		// build category list
		for (size_t i=0; i<numbers.size(); i++) {
			ItemRecord r = items.getByID(numbers[i]);
			cats.push_back(subclasslookup[std::pair<int,int>(r.itemclass, r.subclass)]);
		}

		itemDialog = new CategoryChoiceDialog(this, type, g_modelViewer, _("Choose an item"), caption, choices, cats, catnames, &quality);
	} else {
		itemDialog = new FilteredChoiceDialog(this, type, g_modelViewer, _("Choose an item"), caption, choices, &quality);
	}

	itemDialog->SetSelection(sel);

	wxSize s = itemDialog->GetSize();
	const int w = 250;
	if (s.GetWidth() > w) {
		itemDialog->SetSizeHints(w,-1,-1,-1,-1,-1);
		itemDialog->SetSize(w, -1);
	}

	itemDialog->Move(itemDialog->GetParent()->GetPosition() + wxPoint(4,64));
	itemDialog->Show();
	choosingSlot = slot;
}

/*
struct NumStringPair {
	int id;
	string name;

	const bool operator< (const NumStringPair &p) const {
		return name < p.name;
	}
};
*/

void CharControl::selectSet()
{
	ClearItemDialog();

	std::vector<NumStringPair> items;

	// Adds "none" to select
	NumStringPair n; 
	n.id = -1; 
	n.name = wxString(_("---- None ----")).mb_str();
	items.push_back(n);

	for (ItemSetDB::Iterator it = setsdb.begin(); it != setsdb.end(); ++it) {
		int id = it->Get<unsigned int>(ItemSetDB::SetID);
		if (setsdb.available(id)) {
			NumStringPair p;
			p.id = id;
			p.name = it->getString(ItemSetDB::Name + langOffset);
			items.push_back(p);
		}
	}

	std::sort(items.begin(), items.end());
	numbers.clear();
	choices.Clear();
	for (std::vector<NumStringPair>::iterator it = items.begin(); it != items.end(); ++it) {
		//choices.Add(wxString(it->name.c_str(), *wxConvCurrent));
		choices.Add(wxString(it->name.c_str(), *wxConvCurrent));
		numbers.push_back(it->id);
	}

	itemDialog = new FilteredChoiceDialog(this, UPDATE_SET, g_modelViewer, _("Choose an item set"), _("Item sets"), choices, NULL);
	itemDialog->Move(itemDialog->GetParent()->GetPosition() + wxPoint(4,64));
	itemDialog->Show();
}

void CharControl::selectStart()
{
	ClearItemDialog();

	numbers.clear();
	choices.Clear();

	for (dbc::CharStartOutfit::Iterator it = startdb.begin(); it != startdb.end(); ++it) {
		if ((it->race() == cd.race) && (it->gender() == cd.gender)) {
			try {
				CharClassesDB::Record r = getByID(classdb,it->cclass());
				if (gameVersion == 40000)
					choices.Add(CSConv(wxString(r.getString(CharClassesDB::NameV400 + langOffset),wxConvUTF8)));
				else
					choices.Add(CSConv(wxString(r.getString(CharClassesDB::Name + langOffset),wxConvUTF8)));
				numbers.push_back(it->id());
			} catch (std::exception&) {}
		}
	}

	itemDialog = new ChoiceDialog(this, UPDATE_START, g_modelViewer, _T("Choose a class"), _T("Classes"), choices);
	itemDialog->Move(itemDialog->GetParent()->GetPosition() + wxPoint(4,64));
	itemDialog->Show();
}

bool filterCreatures(std::string const& fn)
{
	wxString tmp(fn.c_str(), wxConvUTF8);
	tmp.MakeLower();
	return (tmp.StartsWith(_T("crea")) && tmp.EndsWith(_T("m2")));
}

std::vector<wxString> creaturemodels;
std::vector<bool> ridablelist;

// TODO: Add an equivilant working version of this function for Linux / Mac OS X
void CharControl::selectMount()
{
	ClearItemDialog();

	numbers.clear();
	choices.Clear();
	cats.clear();
	catnames.Clear();
	catnames.Add(_("Known ridable models"));
	catnames.Add(_("Other models"));

	static bool filelistInitialized = false;

	if (!filelistInitialized) {
		std::set<FileTreeItem> filelist;

		wxArrayString knownRidable;

		FS().getFileLists(filelist, filterCreatures);

		wxTextFile file;
		file.Open(_T("ridable.csv"));
		if (file.IsOpened()) {
			wxString tmp;
			for ( tmp = file.GetFirstLine(); !file.Eof(); tmp = file.GetNextLine() ) {
				tmp.MakeLower();
				if (knownRidable.Index(tmp, false)==wxNOT_FOUND)
					knownRidable.Add(tmp);
			}
		} else {
			wxLogMessage(_T("Can't Initiate ridable.csv ..."));
		}

		for (std::set<FileTreeItem>::iterator it = filelist.begin(); it != filelist.end(); ++it) {
			wxString str((*it).file_name.c_str(), wxConvUTF8);
			str.MakeLower();
			creaturemodels.push_back(str);
			ridablelist.push_back(knownRidable.Index(str, false)!=wxNOT_FOUND);
		}
		filelistInitialized = true;
	}

	choices.Add(_("---- None ----"));
	cats.push_back(0);
	
	for (size_t i=0; i<creaturemodels.size(); i++) {
		choices.Add(wxString(creaturemodels[i].c_str() + 9, *wxConvCurrent));
		cats.push_back(ridablelist[i]?0:1);
	}

	CategoryChoiceDialog *itemDialog = new CategoryChoiceDialog(this, UPDATE_MOUNT, g_modelViewer, _("Choose a mount"), _("Creatures"), choices, cats, catnames, 0);
	itemDialog->Move(itemDialog->GetParent()->GetPosition() + wxPoint(4,64));
	itemDialog->Check(1, false);
	itemDialog->DoFilter();
	itemDialog->Show();

	const int w = 250;
	itemDialog->SetSizeHints(w,-1,-1,-1,-1,-1);
	itemDialog->SetSize(w, -1); 
	this->itemDialog = itemDialog;
}

void CharControl::OnUpdateItem(int type, int id)
{
	switch (type) {
	case UPDATE_ITEM:
		if (choosingSlot == CS_HAND_LEFT)
			model->charModelDetails.closeLHand = false;
		else if (choosingSlot == CS_HAND_RIGHT)
			model->charModelDetails.closeRHand = false;

		cd.equipment[choosingSlot] = numbers[id];
		if (slotHasModel(choosingSlot))
			RefreshItem(choosingSlot);
		labels[choosingSlot]->SetLabel(CSConv(items.getByID(cd.equipment[choosingSlot]).name));
		labels[choosingSlot]->SetForegroundColour(ItemQualityColour(items.getByID(cd.equipment[choosingSlot]).quality));

		// Check if its a 'guild tabard'
		if (choosingSlot == CS_TABARD) 
			td.showCustom = (labels[choosingSlot]->GetLabel() == _T("Guild Tabard"));

		break;

	case UPDATE_SET:
		id = numbers[id];

		if (id) {
			for (int i=0; i<NUM_CHAR_SLOTS; i++) {
				//if (i!=CS_HAND_LEFT && i!=CS_HAND_RIGHT) 
				cd.equipment[i] = 0;
			}
			cd.loadSet(setsdb, items, id);
			RefreshEquipment();
			RefreshModel();
		}
		break;

	case UPDATE_START:
		id = numbers[id];

		if (id) {
			for (int i=0; i<NUM_CHAR_SLOTS; i++) cd.equipment[i] = 0;
			cd.loadStart(startdb, items, id);
			RefreshEquipment();
		}
		break;

	case UPDATE_MOUNT:
		if (model == 0)
			return;

		//canvas->timer.Stop();
		if (g_canvas->root->model) {
			delete g_canvas->root->model;
			g_canvas->root->model = 0;
			g_canvas->model = 0;
		}

		if (id == 0) {
			// clearing the mount
			model->charModelDetails.isMounted = false;
			g_canvas->model = model;
			if (charAtt) {
				charAtt->scale = g_canvas->root->scale;
				charAtt->id = 0;
			}
			g_animControl->UpdateModel(model);
		} else {
			//wxString fn(creaturemodels[id-1].c_str());
			Model *m = new Model(std::string(creaturemodels[id-1].mb_str()), false);
			m->isMount = true;

			// TODO: check if model is ridable
			g_canvas->root->model = m;
			g_canvas->model = m;
			g_animControl->UpdateModel(m);
			
			// find the "mount" animation
			/*
			for (size_t i=0; i<model->header.nAnimations; i++) {
				if (model->anims[i].animID == ANIMATION_MOUNT) {
					model->animManager->Stop();
					model->currentAnim = (int)i;
					model->animManager->Set(0,(short)i,0);
					break;
				}
			}
			*/
			// Alfred 2009.7.23 use animLookups to speednup
			if (model->header.nAnimationLookup >= ANIMATION_MOUNT && model->animLookups[ANIMATION_MOUNT] >= 0) {
					model->animManager->Stop();
					model->currentAnim = model->animLookups[ANIMATION_MOUNT];
					model->animManager->Set(0,(short)model->currentAnim,0);
			}
			
			g_canvas->curAtt = g_canvas->root;
			model->charModelDetails.isMounted = true;

			if (charAtt) {
				charAtt->parent = g_canvas->root;
				//charAtt->id = 42;

				// Need to set this - but from what
				// Model data doesn't contain sizes for different race/gender
				// Character data doesn't contain sizes for different mounts
				// possibly some formula that from both models that needs to be calculated.
				// For "Taxi" mounts scale should be 1.0f I think, for now I'll ignore them
				// I really have no idea!  
				if(creaturemodels[id-1].Mid(9,9).IsSameAs(_T("Kodobeast"), false))
					charAtt->scale = 2.25f;
				else
					charAtt->scale = 1.0f;
				
				//Model *mChar = static_cast<Model*>(charAtt->model);
				//charAtt->scale = m->rad / mChar->rad;

				// Human Male = 2.0346599
				// NE Male = 2.5721216
				// NE Female = 2.2764397

				// RidingFrostSaber = 2.4360743
				// 1.00000

				//canvas->root->scale = 0.5f;
				//Attachment *att = charAtt->addChild("World\\ArtTest\\Boxtest\\xyz.m2", 24, -1);
				//m-> = att->scale;
				//delete att;
			}
		}
		//canvas->timer.Start();
		break;

	case UPDATE_CREATURE_ITEM:
		cd.equipment[choosingSlot] = numbers[id];
		//RefreshCreatureItem(choosingSlot);
		RefreshItem(choosingSlot);
		return;

	case UPDATE_NPC:
		break;

	case UPDATE_SINGLE_ITEM:
		id = numbers[id];
		g_modelViewer->LoadItem(items.getByID(id).model);
		break;

	case UPDATE_NPC_START:
		break;
	}

	//  Update controls associated
	g_modelViewer->UpdateControls();
}

void CharControl::OnTabardSpin(wxSpinEvent &event)
{
	if (!g_canvas || !g_canvas->model || g_canvas->model->modelType == MT_NPC){
		wxLogMessage(_T("Tabard Error: Model Not Present, or can't use a tabard."));
		return;
	}

	switch (event.GetId()) {
	case ID_TABARD_ICON:
		wxLogMessage(_T("Tabard Notice: Icon Change."));
		td.Icon = event.GetPosition();
		break;
	case ID_TABARD_ICONCOLOR:
		wxLogMessage(_T("Tabard Notice: Icon Color Change."));
		td.IconColor = event.GetPosition();
		break;
	case ID_TABARD_BORDER:
		wxLogMessage(_T("Tabard Notice: Border Change."));
        td.Border = event.GetPosition();
		if (td.Border > 5)
		{
			td.BorderColor = 0;
			tabardSpins[SPIN_TABARD_BORDERCOLOR]->SetValue(0);
			tabardSpins[SPIN_TABARD_BORDERCOLOR]->SetRange(0, 3);
		}
		else
			tabardSpins[SPIN_TABARD_BORDERCOLOR]->SetRange(0, 16);
		break;
	case ID_TABARD_BORDERCOLOR:
		wxLogMessage(_T("Tabard Notice: Border Color Change."));
		td.BorderColor = event.GetPosition();
		break;
	case ID_TABARD_BACKGROUND:
		wxLogMessage(_T("Tabard Notice: Background Color Change."));
		td.Background = event.GetPosition();
		break;
	}

	RefreshModel();
}

std::string TabardDetails::GetBackgroundTex(int slot)
{
	std::ostringstream tmpStream;

	std::string tmpU = "textures\\GuildEmblems\\Background_";
	if (Background < 10)
		tmpU += "0";
	tmpStream << Background;
	tmpU += tmpStream.str();
	tmpU += "_TU_U.blp";
	
	std::string tmpL = tmpU;
	tmpL[37] = 'L';

	if (slot == CR_TORSO_UPPER)
		return tmpU;
	else
		return tmpL;
}

std::string TabardDetails::GetBorderTex(int slot)
{
	std::ostringstream tmpStream;

	std::string tmpU = "textures\\GuildEmblems\\Border_";

	if (Border < 10)
		tmpU += "0";
	tmpStream << Border << "_";
	tmpU += tmpStream.str();
	tmpStream.flush();
	tmpStream.str("");

	if (BorderColor < 10)
		tmpU += "0";
	tmpStream << BorderColor;
	tmpU += tmpStream.str();

	tmpU += "_TU_U.blp";
	
	std::string tmpL = tmpU;
	tmpL[36] = 'L';

	if (slot == CR_TORSO_UPPER)
		return tmpU;
	else
		return tmpL;
}

std::string TabardDetails::GetIconTex(int slot)
{
	std::ostringstream tmpStream;

	std::string tmpU = "textures\\GuildEmblems\\Emblem_";

	if(Icon < 10)
		tmpU += "0";
	tmpStream << Icon << "_";
	tmpU += tmpStream.str();
	tmpStream.flush();
	tmpStream.str("");

	if(IconColor < 10)
		tmpU += "0";
	tmpStream << IconColor;
	tmpU += tmpStream.str();

	tmpU += "_TU_U.blp";
	
	std::string tmpL = tmpU;
	tmpL[tmpU.length() - 7] = 'L';

	if(slot == CR_TORSO_UPPER)
		return tmpU;
	else
		return tmpL;
}


void CharDetails::loadSet(ItemSetDB &sets, ItemDatabase &items, int setid)
{
	ItemSetDB::Record rec = getByID(sets,setid);

	for (size_t i = 0; i < ItemSetDB::NumItems; i++)
	{
		int id;
		if (gameVersion == 40000)
			id = rec.Get<int>(ItemSetDB::ItemIDBaseV400 + i);
		else
			id = rec.Get<int>(ItemSetDB::ItemIDBase + i);

		equip(items, id);
	}
}

void CharDetails::loadStart(dbc::CharStartOutfit& start, ItemDatabase &items,
	int setid)
{
	dbc::CharStartOutfit::Record rec = get_by_ID(start, setid);

	for (size_t i = 0; i < 24; i++)
	{
		int id = rec.items(i);

		if (id == 0)
			continue;

		equip(items, id);
	}
}

void CharDetails::equip(ItemDatabase& items, int id)
{
	const ItemRecord &r = items.getByID(id);

	if (r.type > 0)
	{
		// find a slot for it
		for (int s = 0; s < NUM_CHAR_SLOTS; s++)
		{
			if (correctType(r.type, s))
			{
				equipment[s] = id;
				break;
			}
		}
	}
}
