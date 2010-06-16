#include "animcontrol.h"
#include "util.h"
#include "globalvars.h"
#include "wx/wx.h"

IMPLEMENT_CLASS(AnimControl, wxWindow)

BEGIN_EVENT_TABLE(AnimControl, wxWindow)
	EVT_COMBOBOX(ID_ANIM, AnimControl::OnAnim)
	EVT_COMBOBOX(ID_ANIM_SECONDARY, AnimControl::OnAnim)
	EVT_COMBOBOX(ID_ANIM_MOUTH, AnimControl::OnAnim)

	EVT_COMBOBOX(ID_LOOPS, AnimControl::OnLoop)
	EVT_COMBOBOX(ID_SKIN, AnimControl::OnSkin)
	EVT_COMBOBOX(ID_ITEMSET, AnimControl::OnItemSet)

	EVT_CHECKBOX(ID_OLDSTYLE, AnimControl::OnCheck)
	EVT_CHECKBOX(ID_ANIM_LOCK, AnimControl::OnCheck)

	EVT_BUTTON(ID_PLAY, AnimControl::OnButton)
	EVT_BUTTON(ID_PAUSE, AnimControl::OnButton)
	EVT_BUTTON(ID_STOP, AnimControl::OnButton)
	EVT_BUTTON(ID_ADDANIM, AnimControl::OnButton)
	EVT_BUTTON(ID_CLEARANIM, AnimControl::OnButton)
	EVT_BUTTON(ID_PREVANIM, AnimControl::OnButton)
	EVT_BUTTON(ID_NEXTANIM, AnimControl::OnButton)

	EVT_SLIDER(ID_SPEED, AnimControl::OnSliderUpdate)
	EVT_SLIDER(ID_SPEED_MOUTH, AnimControl::OnSliderUpdate)
	EVT_SLIDER(ID_FRAME, AnimControl::OnSliderUpdate)
END_EVENT_TABLE()

AnimControl::AnimControl(wxWindow* parent, wxWindowID id)
{
	wxLogMessage(_T("Creating Anim Control..."));

	if(Create(parent, id, wxDefaultPosition, wxSize(700,90), 0, _T("AnimControlFrame")) == false) {
		wxMessageBox(_T("Failed to create a window for our AnimControl!"), _T("Error"));
		wxLogMessage(_T("GUI Error: Failed to create a window for our AnimControl!"));
		return;
	}

	const wxString strLoops[10] = {_T("0"), _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8"), _T("9")};
	
	animCList = new wxComboBox(this, ID_ANIM, _("Animation"), wxPoint(10,10), wxSize(150,16), 0, NULL, wxCB_READONLY|wxCB_SORT, wxDefaultValidator, _("Animation")); //|wxCB_SORT); //wxPoint(66,10)
	animCList2 = new wxComboBox(this, ID_ANIM_SECONDARY, _("Secondary"), wxPoint(170,10), wxSize(150,16), 0, NULL, wxCB_READONLY|wxCB_SORT, wxDefaultValidator, _("Secondary")); //|wxCB_SORT); //wxPoint(66,10)
	animCList2->Enable(false);
	animCList2->Show(false);

	// Our hidden head/mouth related controls
	animCList3 = new wxComboBox(this, ID_ANIM_MOUTH, _("Mouth"), wxPoint(10,100), wxSize(150,16), 0, NULL, wxCB_READONLY|wxCB_SORT, wxDefaultValidator, _("Secondary")); //|wxCB_SORT); //wxPoint(66,10)
	animCList3->Enable(false);
	animCList3->Show(false);

	//btnPauseMouth = new wxButton(this, ID_PAUSE_MOUTH, _("Pause"), wxPoint(160,100), wxSize(45,20));
	//btnPauseMouth->Show(false);

	speedMouthSlider = new wxSlider(this, ID_SPEED_MOUTH, 10, 0, 40, wxPoint(255,100), wxSize(100,38), wxSL_AUTOTICKS);
	speedMouthSlider->SetTickFreq(10, 1);
	speedMouthSlider->Show(false);

	speedMouthLabel = new wxStaticText(this, -1, _("Speed: 1.0x"), wxPoint(180,100), wxDefaultSize);
	speedMouthLabel->Show(false);
	// ---

	loopList = new wxComboBox(this, ID_LOOPS, wxT("0"), wxPoint(330, 10), wxSize(40,16), 10, strLoops, wxCB_READONLY, wxDefaultValidator, wxT("Loops")); //|wxCB_SORT); //wxPoint(66,10)
	btnAdd = new wxButton(this, ID_ADDANIM, _("Add"), wxPoint(380, 10), wxSize(45,20));

	skinList = new wxComboBox(this, ID_SKIN, _("Skin"), wxPoint(170,10), wxSize(144,16), 0, NULL, wxCB_READONLY);
	skinList->Show(FALSE);
	randomSkins = true;
	defaultDoodads = true;

	wmoList = new wxComboBox(this, ID_ITEMSET, _("Item set"), wxPoint(220,10), wxSize(128,16), 0, NULL, wxCB_READONLY);
	wmoList->Show(FALSE);
	wmoLabel = new wxStaticText(this, -1, wxEmptyString, wxPoint(10,15), wxSize(192,16));
	wmoLabel->Show(FALSE);

	speedSlider = new wxSlider(this, ID_SPEED, 10, 1, 40, wxPoint(490,56), wxSize(100,38), wxSL_AUTOTICKS);
	speedSlider->SetTickFreq(10, 1);
	speedLabel = new wxStaticText(this, -1, _("Speed: 1.0x"), wxPoint(490,40), wxDefaultSize);

	frameLabel = new wxStaticText(this, -1, _("Frame: 0"), wxPoint(330,40), wxDefaultSize);
	frameSlider = new wxSlider(this, ID_FRAME, 1, 1, 10, wxPoint(330,56), wxSize(160,38), wxSL_AUTOTICKS);
	frameSlider->SetTickFreq(2, 1);

	btnPlay = new wxButton(this, ID_PLAY, _("Play"), wxPoint(10,40), wxSize(45,20));
	btnPause = new wxButton(this, ID_PAUSE, _("Pause"), wxPoint(62,40), wxSize(45,20));
	btnStop = new wxButton(this, ID_STOP, _("Stop"), wxPoint(115,40), wxSize(45,20));
	
	btnClear = new wxButton(this, ID_CLEARANIM, _("Clear"), wxPoint(10,64), wxSize(45,20));
	btnPrev = new wxButton(this, ID_PREVANIM, _T("<<"), wxPoint(62,64), wxSize(45,20));
	btnNext = new wxButton(this, ID_NEXTANIM, _T(">>"), wxPoint(115,64), wxSize(45,20));
	
	lockAnims = new wxCheckBox(this, ID_ANIM_LOCK, _("Lock Animations"), wxPoint(170,40), wxDefaultSize, 0);
	lockAnims->SetValue(true);
	bLockAnims = true;
	oldStyle = new wxCheckBox(this, ID_OLDSTYLE, _("Auto Animate"), wxPoint(170,64), wxDefaultSize, 0);
	bOldStyle = true;
	oldStyle->SetValue(true);
}

AnimControl::~AnimControl()
{
	// Free the memory the was allocated (fixed: memory leak)
	for (unsigned int i=0; i<skinList->GetCount(); i++) {
		TextureGroup *grp = (TextureGroup *)skinList->GetClientData(i);
		wxDELETE(grp);
	}

	animCList->Clear();
	animCList2->Clear();
	animCList3->Clear();
	skinList->Clear();
	
	animCList->Destroy();
	animCList2->Destroy();
	animCList3->Destroy();
	skinList->Destroy();
}

void AnimControl::UpdateModel(Model *m)
{
	if (!m)
		return;
	
	// Clear skin/texture data from previous model - if there is any.
	if (g_selModel) {
		for (unsigned int i=0; i<skinList->GetCount(); i++) {
			TextureGroup *grp = (TextureGroup *)skinList->GetClientData(i);
			wxDELETE(grp);
		}
	}
	// --

	g_selModel = m;

	selectedAnim = 0;
	selectedAnim2 = -1;
	selectedAnim3 = -1;

	animCList->Clear();
	animCList2->Clear();
	animCList3->Clear();

	skinList->Clear();

	int useanim = -1;

	// Find any textures that exist for the model
	bool res = false;

	wxString fn(m->name.c_str(), wxConvUTF8);
	fn.MakeLower();
	if (fn.substr(0,4) != _T("char")) {

		if (fn.substr(0,8) == _T("creature"))
			res = UpdateCreatureModel(m);
		else if (fn.substr(0,4) == _T("item"))
			res = UpdateItemModel(m);
	}

	skinList->Show(res);

	// A small attempt at keeping the 'previous' animation that was selected when changing
	// the selected model via the model control.
/*
	// Alfred 2009.07.19 keep currentAnim may crash others if it doesn't have, we should save the animID, not currentAnim
	if (g_selModel->currentAnim > 0)
		useanim = g_selModel->currentAnim;
*/

	if (g_selModel->charModelDetails.isChar) { // only display the "secondary" animation list if its a character
		animCList2->Select(useanim);
		animCList2->Show(true);
		lockAnims->Show(true);
		loopList->Show(true);
		btnAdd->Show(true);
	} else {
		animCList2->Show(false);
		lockAnims->Show(false);
		loopList->Show(false);
		btnAdd->Show(false);
	}

	// Animation stuff
	if (g_selModel->animated && g_selModel->anims) {
		wxString strName;
		wxString strStand;
		int selectAnim = 0;
		for (unsigned int i=0; i<g_selModel->header.nAnimations; i++) {			
			try {
				AnimDB::Record rec = animdb.getByAnimID(g_selModel->anims[i].animID);
				strName = rec.getString(AnimDB::Name);
			} catch (AnimDB::NotFound) {
				strName = _T("???");
			}
			
			//strName = name;
			//if ((useanim==-1) && (strName=="Stand"))
			//	useanim = i;

			strName += wxString::Format(_T(" [%i]"), i);

			if (g_selModel->anims[i].animID == ANIM_STAND && useanim == -1) {
				strStand = strName;
				useanim = i;
			}

			animCList->Append(strName);
			if (g_selModel->charModelDetails.isChar) {
				animCList2->Append(strName);
				animCList3->Append(strName);
			}
		}

		if (useanim != -1) {
			for(unsigned int i=0; i<animCList->GetCount(); i++) {
				strName = animCList->GetString(i);
				if (strName == strStand) {
					selectAnim = i;
					break;
				}
			}
		}

		if (useanim==-1)
			useanim = 0;
			//return;

		g_selModel->currentAnim = useanim; // anim position in anims
		animCList->Select(selectAnim); // anim position in selection
		animCList->Show(true);

		frameSlider->SetRange(g_selModel->anims[useanim].timeStart, g_selModel->anims[useanim].timeEnd);
		frameSlider->SetTickFreq(g_selModel->anims[useanim].playSpeed, 1);
		
		g_selModel->animManager->Set(0, useanim, 0);
#if 0
		int NextAnimation = useanim;
		for(size_t i=1; i<4; i++) {
			NextAnimation = g_selModel->anims[NextAnimation].NextAnimation;
			if (NextAnimation >= 0)
				g_selModel->animManager->addAnim(NextAnimation, loopList->GetSelection());
			else
				break;
		}
#endif
		g_selModel->animManager->Play();
	}

	wmoList->Show(false);
	wmoLabel->Show(false);
}

void AnimControl::UpdateWMO(WMO *w, int group)
{
	if (!w || w->name.size()==0)
		return;

	bool newwmo = (oldname != w->name);
	oldname = w->name;

	//Model *m = static_cast<Model*>(canvas->root->children[0]);

	//if (!m || m->anims==NULL)
	//	return;

	//m->animManager->Reset();
	g_selWMO = w;


	frameSlider->SetRange(0, 10);
	frameSlider->SetTickFreq(2, 1);
	
	animCList->Show(false);
	skinList->Show(false);
	loopList->Show(false);
	btnAdd->Show(false);
	
	if (newwmo) {
		// build itemset list
		wmoList->Clear();
		wmoList->Append(_T("(No doodads)"));

		for (size_t i=0; i<g_selWMO->doodadsets.size(); i++) {
			wmoList->Append(wxString(g_selWMO->doodadsets[i].name, *wxConvCurrent));
		}

		int sel = defaultDoodads ? 1 : 0;
		g_selWMO->includeDefaultDoodads = defaultDoodads;
		wmoList->Select(sel);
		g_selWMO->showDoodadSet(sel-1);
	}
	wmoList->Show(TRUE);

	// get wmo name or current wmogroup name/descr
	if (group>=-1 && group<g_selWMO->nGroups) {
		std::string label;
		label = w->name.substr(w->name.find_last_of('\\')+1);
		if (group>=0) {
			label += " - " + g_selWMO->groups[group].name;
			if (g_selWMO->groups[group].desc.length()) {
				label += " - " + g_selWMO->groups[group].desc;
			}
		}
		wmoLabel->SetLabel(wxString(label.c_str(), *wxConvCurrent));
	} else {
		wmoLabel->SetLabel(_("This group has been removed from the WMO"));
	}
	wmoLabel->Show(TRUE);
}

wxString sFilterDir;
bool filterDir(std::string fn)
{
	wxString tmp(fn.c_str(), wxConvUTF8);
	tmp.MakeLower();
	return (tmp.StartsWith(sFilterDir) && tmp.EndsWith(_T("blp")));
}

bool AnimControl::UpdateCreatureModel(Model *m)
{
	wxString fn(m->name.c_str(), wxConvUTF8);

	// replace .M2 with .MDX
	fn = fn.BeforeLast(_T('.')).Append(_T(".mdx"));

	TextureSet skins;

	// see if this model has skins
	try {
		CreatureModelDB::Record rec = modeldb.getByFilename(fn);
		// for character models, don't use skins
		if (rec.getUInt(CreatureModelDB::Type) != 4) {
			//TextureSet skins;
			unsigned int modelid = rec.getUInt(CreatureModelDB::ModelID);

			for (CreatureSkinDB::Iterator it = skindb.begin();  it!=skindb.end();  ++it) {
				if (it->getUInt(CreatureSkinDB::ModelID) == modelid) {
					TextureGroup grp;
					for (int i=0; i<TextureGroup::num; i++) {
						wxString skin(it->getString(CreatureSkinDB::Skin + i));
						
						grp.tex[i] = skin.mb_str();
					}
					grp.base = 11;
					grp.count = 3;
					if (grp.tex[0].length() > 0) 
						skins.insert(grp);
				}
			}
			
			// Hard coded skin additions - missing from DBC ?
			if (fn == _T("Creature\\Furbolg\\furbolg.mdx")) {
				TextureGroup grp;
				grp.tex[0] = "FurbolgSkinPanda";
				grp.tex[1] = "FurbolgStuffWhite";
				grp.base = 11;
				grp.count = 2;
				skins.insert(grp);
			} else if (fn == _T("Creature\\Murloc\\babymurloc.mdx")) {
				TextureGroup grp;
				grp.tex[0] = "MurlocBabyBlue";
				grp.tex[1] = "TopHat";
				grp.tex[2] = "MurlocBabyReflect";
				grp.base = 11;
				grp.count = 3;
				skins.insert(grp);
				grp.tex[0] = "MurlocBabyGreen";
				skins.insert(grp);
				grp.tex[0] = "MurlocBabyOrange";
				skins.insert(grp);
				grp.tex[0] = "MurlocBabyPink";
				skins.insert(grp);
				grp.tex[0] = "MurlocBabyPurple";
				skins.insert(grp);
				grp.tex[0] = "MurlocBabyWhite";
				skins.insert(grp);
			} else if (fn == _T("Creature\\Felbeast\\felbeast.mdx")) {
				TextureGroup grp;
				grp.tex[0] = "FelBeastSkinGreenBlack";
				grp.base = 11;
				grp.count = 1;
				skins.insert(grp);
			} else if (fn == _T("Creature\\Gryphon\\gryphon.mdx")) {
				TextureGroup grp;
				grp.tex[0] = "Gryphon_Skin01Black";
				grp.tex[1] = "Gryphon_Skin02Black";
				grp.base = 11;
				grp.count = 2;
				skins.insert(grp);
				grp.tex[0] = "Gryphon_Skin01White";
				grp.tex[1] = "Gryphon_Skin02White";
				skins.insert(grp);
			} else if (fn == _T("Creature\\Lasher\\lasher.mdx")) {
				TextureGroup grp;
				grp.tex[0] = "LasherSkinBlue";
				grp.base = 11;
				grp.count = 1;
				skins.insert(grp);
				grp.tex[0] = "LasherSkinGreen";
				skins.insert(grp);
				grp.tex[0] = "LasherSkinPurple";
				skins.insert(grp);
			} else if (fn == _T("Creature\\Minespider\\minespider.mdx")) {
				TextureGroup grp;
				grp.tex[0] = "MineSpiderSkinViolet";
				grp.base = 11;
				grp.count = 1;
				skins.insert(grp);
			} else if (fn == _T("Creature\\Owl\\owl.mdx")) {
				TextureGroup grp;
				grp.tex[0] = "OwlSpirit";
				grp.base = 11;
				grp.count = 1;
				skins.insert(grp);
			} else if (fn == _T("Creature\\Parrot\\parrot.mdx")) {
				TextureGroup grp;
				grp.tex[0] = "ParrotSkinBrown";
				grp.base = 11;
				grp.count = 1;
				skins.insert(grp);
				grp.tex[0] = "ParrotSkinCrimson";
				skins.insert(grp);
				grp.tex[0] = "ParrotSkinlavender";
				skins.insert(grp);
			} else if (fn == _T("Creature\\Pterrordax\\pterrordax.mdx")) {
				TextureGroup grp;
				grp.tex[0] = "PterrordaxSkinBrown";
				grp.tex[1] = "PterrordaxWingSkinBrown";
				grp.base = 11;
				grp.count = 2;
				skins.insert(grp);
			} else if (fn == _T("Creature\\Silithidscarab\\silithidscarab.mdx")) {
				TextureGroup grp;
				grp.tex[0] = "SilithidScarabSkinGreen";
				grp.base = 11;
				grp.count = 1;
				skins.insert(grp);
			} else if (fn == _T("Creature\\Silithidflyer\\silithidflyer.mdx")) {
				TextureGroup grp;
				grp.tex[0] = "SilithidFlyerSkinBlack";
				grp.base = 11;
				grp.count = 1;
				skins.insert(grp);
				grp.tex[0] = "SilithidFlyerSkinOrange";
				skins.insert(grp);
			}
			
			// hard-coded fixes.
			if (skins.empty() == true) {
				if (fn == _T("Creature\\Dwarfmalewarriorlight\\dwarfmalewarriorlight.mdx")) {
					TextureGroup grp;
					grp.tex[0] = "DwarfMaleWarriorLightSkin";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
					grp.tex[0] = "DwarfMaleWarriorLightSkin2";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
				} else if (fn == _T("Creature\\Faeriedragon\\faeriedragon_ghost.mdx")) {
					TextureGroup grp;
					grp.tex[0] = "FaerieDragon_Ghost";
					grp.tex[1] = "faeriewing_Ghost";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
				} else if (fn == _T("Creature\\Felbat\\felbat.mdx")) {
					TextureGroup grp;
					grp.tex[0] = "BatSkin01";
					grp.tex[1] = "BatSkin02";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
					grp.tex[0] = "BatSkinBrown01";
					grp.tex[1] = "BatSkinBrown02";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
					grp.tex[0] = "BatSkinViolet01";
					grp.tex[1] = "BatSkinViolet02";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
					grp.tex[0] = "BatSkinWhite01";
					grp.tex[1] = "BatSkinWhite02";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
				} else if (fn == _T("Creature\\Highelf\\highelfmale_priest.mdx")) {
					TextureGroup grp;
					grp.tex[0] = "HighElfMalePriest";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
				} else if (fn == _T("Creature\\Orcmalewarriorheavy\\orcmalewarriorheavy.mdx")) {
					TextureGroup grp;
					grp.tex[1] = "OrcMaleSamuHairSkin";
					grp.tex[0] = "OrcNPCSkin";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
				} else if (fn == _T("Creature\\Seagiant\\seagiant.mdx")) {
					TextureGroup grp;
					grp.tex[0] = "SeaGiantSkin01";
					grp.tex[1] = "SeaGiantSkin02";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
				} else if (fn == _T("Creature\\Sealion\\sealion.mdx")) {
					TextureGroup grp;
					grp.tex[0] = "SeaLionSkin";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
				} else if (fn == _T("Creature\\Stormcrow\\stormcrow.mdx")) {
					TextureGroup grp;
					grp.tex[0] = "StormCrowSkin";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
				} else if(fn == _T("Creature\\Humanmalewizard\\humanmalewizard.mdx")) {
					TextureGroup grp;
					grp.tex[0] = "HumanMaleMerchant01Skin";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
					grp.tex[0] = "HumanMaleMerchant02Skin";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
				} 
			}
		}
	} catch (CreatureModelDB::NotFound) {
		// Try hardcoding some fixes for missing model info from the DBC
		if(fn == _T("Creature\\Dwarfmalewarriorlight\\dwarfmalewarriorlight_ghost.mdx")) {
			TextureGroup grp;
			grp.tex[0] = "DwarfMaleWarriorLightSkin_Ghost";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
		} else if(fn == _T("Creature\\Mounteddemonknight\\mounteddemonknight.mdx")) {
			TextureGroup grp;
			grp.tex[0] = "MountedDemonKnightBlack_01";
			grp.tex[1] = "MountedDemonKnightBlack_02";
			grp.tex[2] = "MountedDemonKnightBlack_03";
			grp.base = 11;
			grp.count = 3;
			skins.insert(grp);
		} else if(fn == _T("Creature\\Orcfemalewarriorlight\\orcfemale.mdx")) {
			TextureGroup grp;
			grp.tex[0] = "OrcFemaleSkin";
			//grp.tex[1] = "OrcFemaleSkin";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
		} else if(fn == _T("Creature\\Tigon\\tigon.mdx")) {
			TextureGroup grp;
			grp.tex[0] = "TIGONSKIN_ORANGE";
			grp.tex[1] = "TIGONEYEGLOW";
			grp.base = 11;
			grp.count = 2;
			skins.insert(grp);
			grp.tex[0] = "CatManSkin_Orange";
			grp.tex[1] = "TIGONEYEGLOW";
			grp.base = 11;
			grp.count = 2;
			skins.insert(grp);
			grp.tex[0] = "CatManSkin_Black";
			grp.tex[1] = "TIGONEYEGLOW";
			grp.base = 11;
			grp.count = 2;
			skins.insert(grp);
		} else if(fn == _T("Creature\\Humanmalepiratecaptain\\humanmalepiratecaptain_ghost.mdx")) {
			TextureGroup grp;
			grp.tex[0] = "PirateCaptain02_Ghost";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
		} else if(fn == _T("Creature\\Humanmalepiratecrewman\\humanmalepiratecrewman_ghost.mdx")) {
			TextureGroup grp;
			grp.tex[0] = "PirateCrewman01_Ghost";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "PirateCrewman02_Ghost";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "PirateCrewman03_Ghost";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			/* 
			// Cataclysm Beta have some skins not in CreatureModelData.dbc
		} else if (fn == _T("Creature\\Druidbearworgen\\druidbearworgen.mdx")) {
			TextureGroup grp;
			grp.tex[0] = "DruidBearWorgen";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "DruidBearWorgenBlack";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "DruidBearWorgenBrown";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "DruidBearWorgenTan";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "DruidBearWorgenWhite";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			return FillSkinSelector(skins);
		} else if (fn == _T("Creature\\Druidcatworgen\\druidcatworgen.mdx")) {
			TextureGroup grp;
			grp.tex[0] = "DruidCatSkinWorgen";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "DruidCatSkinWorgenBlack";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "DruidCatSkinWorgenBrown";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "DruidCatSkinWorgenWhite";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "DruidCatSkinWorgenYellow";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			return FillSkinSelector(skins);
		} else if (fn == _T("Creature\\Druidcattroll\\druidcattroll.mdx")) {
			TextureGroup grp;
			grp.tex[0] = "DruidCatTrollSkinBlack";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "DruidCatTrollSkinBlue";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "DruidCatTrollSkinGreen";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "DruidCatTrollSkinRed";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "DruidCatTrollSkinWhite";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			return FillSkinSelector(skins);
			*/
		}
	}

	// Search the same directory for BLPs
	std::set<FileTreeItem> filelist;
	sFilterDir = fn.BeforeLast(SLASH)+SLASH;
	sFilterDir.MakeLower();
	getFileLists(filelist, filterDir);
	if (filelist.begin() != filelist.end()) {
		TextureGroup grp;
		grp.base = 11;
		grp.count = 1;
		for (std::set<FileTreeItem>::iterator it = filelist.begin(); it != filelist.end(); ++it) {
			wxString str((*it).fn.c_str(), wxConvUTF8);
			grp.tex[0] = std::string((char *)str.BeforeLast(_T('.')).AfterLast(SLASH).c_str());
			skins.insert(grp);
		}
	}

	if (!skins.empty())
		return FillSkinSelector(skins);

	return false;
}

bool AnimControl::UpdateItemModel(Model *m)
{
	wxString fn(m->name.c_str(), wxConvUTF8);

	// change M2 to mdx
	fn = fn.BeforeLast(_T('.')).Append(_T(".mdx"));

	// Check to see if its a helmet model, if so cut off the race
	// and gender specific part of the filename off
	if (fn.Find(_T("\\head\\")) > wxNOT_FOUND) {
		fn = fn.BeforeLast('_');
		fn.Append(_T(".mdx"));
	}

	// just get the file name, exclude the path.
	fn = fn.AfterLast('\\');
	
	TextureSet skins;

	for (ItemDisplayDB::Iterator it=itemdisplaydb.begin(); it!=itemdisplaydb.end(); ++it) {
		if (fn.IsSameAs(it->getString(ItemDisplayDB::Model), false)) {
            TextureGroup grp;
			grp.base = 2;
			grp.count = 1;
			wxString skin = it->getString(ItemDisplayDB::Skin);
			grp.tex[0] = skin.mb_str();
			if (grp.tex[0].length() > 0) 
				skins.insert(grp);
		}
		
		//if (!strcmp(it->getString(ItemDisplayDB::Model2), fn.c_str())) {
		if (fn.IsSameAs(it->getString(ItemDisplayDB::Model2), false)) {
            TextureGroup grp;
			grp.base = 2;
			grp.count = 1;
			wxString skin = it->getString(ItemDisplayDB::Skin2);
			grp.tex[0] = skin.mb_str();
			if (grp.tex[0].length() > 0) 
				skins.insert(grp);
		}
	}

	// Search the same directory for BLPs
	std::set<FileTreeItem> filelist;
	sFilterDir = wxString(m->name.c_str(), wxConvUTF8).BeforeLast(_T('.'));
	sFilterDir.MakeLower();
	getFileLists(filelist, filterDir);
	if (filelist.begin() != filelist.end()) {
		TextureGroup grp;
		grp.base = 2;
		grp.count = 1;
		for (std::set<FileTreeItem>::iterator it = filelist.begin(); it != filelist.end(); ++it) {
			wxString str((*it).fn.c_str(), wxConvUTF8);
			grp.tex[0] = std::string((char *)str.BeforeLast(_T('.')).AfterLast(SLASH).c_str());
			skins.insert(grp);
		}
	}

	if (!skins.empty())
		return FillSkinSelector(skins);

	return false;
}


bool AnimControl::FillSkinSelector(TextureSet &skins)
{
	if (skins.size() > 0) {
		int num = 0;
		// fill our skin selector
		for (TextureSet::iterator it = skins.begin(); it != skins.end(); ++it) {
			wxString texname = wxString(it->tex[0].c_str(), *wxConvCurrent);
			skinList->Append(texname);
			const  char * sName = g_selModel->name.c_str();
			texname = wxString(sName, wxConvUTF8).BeforeLast(SLASH) << _T("\\") << texname << _T(".blp");
			std::string sTexname = std::string(wxString(texname.c_str()).mb_str());
			g_selModel->TextureList.push_back(sTexname);
			//g_selModel->TextureList.push_back( wxString(texname.c_str()).mb_str() );
			TextureGroup *grp = new TextureGroup(*it);
			skinList->SetClientData(num++, grp);
		}

		bool existingTexture = false;
		for (int i = 0; i<TEXTURE_MAX; i++) {
			if (g_selModel->replaceTextures[i] > 0) {
				existingTexture = true;
				break;
			}
		}

		if (!existingTexture) {
			int mySkin = randomSkins ? randint(0,(int)skins.size()-1) : 0;
			SetSkin(mySkin);
		}

		bool ret = (skins.size() > 1);
		skins.clear();
		return ret;
	} else 
		return false;
}


void AnimControl::OnButton(wxCommandEvent &event)
{
	if (!g_selModel)
		return;

	int id = event.GetId();

	if (id == ID_PLAY) {
		g_selModel->currentAnim = g_selModel->animManager->GetAnim();
		g_selModel->animManager->Play();
	} else if (id == ID_PAUSE) {
		g_selModel->animManager->Pause();
	} else if (id == ID_STOP) {
		g_selModel->animManager->Stop();
	} else if (id == ID_CLEARANIM) {
		g_selModel->animManager->Clear();
	} else if (id == ID_ADDANIM) {
		g_selModel->animManager->AddAnim(selectedAnim, loopList->GetSelection());
	} else if (id == ID_PREVANIM) {
		g_selModel->animManager->PrevFrame();
		SetAnimFrame(g_selModel->animManager->GetFrame());
	} else if (id == ID_NEXTANIM) {
		g_selModel->animManager->NextFrame();
		SetAnimFrame(g_selModel->animManager->GetFrame());
	}
}

void AnimControl::OnCheck(wxCommandEvent &event)
{
	if (event.GetId() == ID_OLDSTYLE)
		bOldStyle = event.IsChecked();
	else if (event.GetId() == ID_ANIM_LOCK) {
		bLockAnims = event.IsChecked();
		animCList2->Enable(bLockAnims==false);
		if (bLockAnims)
			g_selModel->animManager->ClearSecondary();

		if (wxGetKeyState(WXK_SHIFT) && event.IsChecked()==false) {
			animCList3->Enable(true);
			animCList3->Show(true);
			speedMouthSlider->Show(true);
			speedMouthLabel->Show(true);
			//btnPauseMouth->Show(true);
		} else {
			animCList3->Enable(false);
			animCList3->Show(false);
			speedMouthSlider->Show(false);
			speedMouthLabel->Show(false);
			//btnPauseMouth->Show(false);
		}
	}
}

void AnimControl::OnAnim(wxCommandEvent &event)
{
	if (event.GetId() == ID_ANIM) {
		if (g_selModel) {
			wxString val = animCList->GetValue();
			int first = val.Find('[')+1;
			int last = val.Find(']');
			selectedAnim = wxStringToInt(val.Mid(first, last-first));
			
			if (bLockAnims) {
				//selectedAnim2 = -1;
				animCList2->SetSelection(event.GetSelection());
			}

			if (bOldStyle == true) {
				g_selModel->currentAnim = selectedAnim;
				g_selModel->animManager->Stop();
				g_selModel->animManager->Set(0, selectedAnim, loopList->GetSelection());
#if 0
				int NextAnimation = selectedAnim;
				for(size_t i=1; i<4; i++) {
					NextAnimation = g_selModel->anims[NextAnimation].NextAnimation;
					if (NextAnimation >= 0)
						g_selModel->animManager->addAnim(NextAnimation, loopList->GetSelection());
					else
						break;
				}
#endif
				g_selModel->animManager->Play();
				
				frameSlider->SetRange(g_selModel->anims[selectedAnim].timeStart, g_selModel->anims[selectedAnim].timeEnd);
				frameSlider->SetTickFreq(g_selModel->anims[selectedAnim].playSpeed, 1);
			}
		}

		//canvas->resetTime();
	} else if (event.GetId() == ID_ANIM_SECONDARY) {
		wxString val = animCList2->GetValue();
		int first = val.Find('[')+1;
		int last = val.Find(']');
		selectedAnim2 = wxStringToInt(val.Mid(first, last-first));

		g_selModel->animManager->SetSecondary(selectedAnim2);
	} else if (event.GetId() == ID_ANIM_MOUTH) {
		wxString val = animCList3->GetValue();
		int first = val.Find('[')+1;
		int last = val.Find(']');
		selectedAnim3 = wxStringToInt(val.Mid(first, last-first));

		//canvas->g_selModel->animManager->SetSecondary(selectedAnim2);
		g_selModel->animManager->SetMouth(event.GetSelection());
	}
}

void AnimControl::OnSkin(wxCommandEvent &event)
{
	if (g_selModel) {
		int sel = event.GetSelection();
		SetSkin(sel);
	}
}

void AnimControl::OnItemSet(wxCommandEvent &event)
{
	if (g_selWMO) {
		int sel = event.GetSelection();
		// -1 for no doodads
		g_selWMO->showDoodadSet(sel - 1);
	}
}

void AnimControl::OnSliderUpdate(wxCommandEvent &event)
{
	if (event.GetId() == ID_SPEED) {
		SetAnimSpeed(speedSlider->GetValue() / 10.0f);

	} else if (event.GetId() == ID_SPEED_MOUTH) {
		if (!g_selModel || !g_selModel->animManager)
			return;
		
		float speed = speedMouthSlider->GetValue() / 10.0f;
		g_selModel->animManager->SetMouthSpeed(speed);
		speedMouthLabel->SetLabel(wxString::Format(_T("Speed: %.1fx"), speed));

	} else if (event.GetId() == ID_FRAME)
		SetAnimFrame(frameSlider->GetValue());

}

void AnimControl::OnLoop(wxCommandEvent &event)
{
	if (bOldStyle == true) {
		g_selModel->animManager->Stop();
		g_selModel->animManager->Set(0, selectedAnim, loopList->GetSelection());
#if 0
		int NextAnimation = selectedAnim;
		for(size_t i=1; i<4; i++) {
			NextAnimation = g_selModel->anims[NextAnimation].NextAnimation;
			if (NextAnimation >= 0)
				g_selModel->animManager->addAnim(NextAnimation, loopList->GetSelection());
			else
				break;
		}
#endif
		g_selModel->animManager->Play();
	} 
}

void AnimControl::SetSkin(int num)
{
	TextureGroup *grp = (TextureGroup*) skinList->GetClientData(num);
	for (int i=0; i<grp->count; i++) {
		if (g_selModel->useReplaceTextures[grp->base+i]) {
			texturemanager.del(g_selModel->replaceTextures[grp->base+i]);
			g_selModel->replaceTextures[grp->base+i] = texturemanager.add(std::string(makeSkinTexture(g_selModel->name.c_str(), grp->tex[i].c_str()).mb_str()));
		}
	}

	skinList->Select(num);
}

wxString AnimControl::makeSkinTexture(const char *texfn, const char *skin)
{
	wxString res(texfn, wxConvUTF8);
	size_t i = res.find_last_of('\\');
	res = res.substr(0,i+1);
	res.append(wxString(skin, wxConvUTF8));
	res.append(_T(".blp"));
	return res;
}

int AnimControl::AddSkin(TextureGroup grp)
{
	skinList->Append(wxString(_T("Custom"), *wxConvCurrent));
	int count = skinList->GetCount() - 1;
	TextureGroup *group = new TextureGroup(grp);
	skinList->SetClientData(count, group);
	SetSkin(count);
	return count;
}

void AnimControl::SetAnimSpeed(float speed)
{
	if (!g_selModel || !g_selModel->animManager)
		return;

	g_selModel->animManager->SetSpeed(speed);
	
	speedLabel->SetLabel(wxString::Format(_T("Speed: %.1fx"), speed));
}


void AnimControl::SetAnimFrame(int frame)
{
	if (!g_selModel || !g_selModel->animManager)
		return;

	g_selModel->animManager->SetFrame((unsigned int) frame);
	
	int frameNum = (frame - g_selModel->anims[g_selModel->currentAnim].timeStart);

	frameLabel->SetLabel(wxString::Format(_T("Frame: %i"), frameNum));
	frameSlider->SetValue(frame);
}

