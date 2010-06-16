
#include "effects.h"
#include "itemselection.h"
#include "enums.h"
#include "database.h"

wxArrayString spelleffects;
SpellEffectsDB spelleffectsdb;

void GetSpellEffects(){
	for (SpellEffectsDB::Iterator it=spelleffectsdb.begin(); it!=spelleffectsdb.end(); ++it) {
		wxString temp(it->getString(SpellEffectsDB::EffectName));
		if (temp.StartsWith(_T("zzOLD")))
			spelleffects.Insert(temp, 0);
	}

	spelleffects.Sort();
}

// 10 for rhand, 11 for lhand)
void SelectCreatureItem(int slot, int current, CharControl *cc, wxWindow *parent)
{
	cc->ClearItemDialog();
	cc->numbers.clear();
	cc->choices.Clear();

	// collect all items for this slot, making note of the occurring subclasses
	set<pair<int,int> > subclassesFound;

	int sel=0, ord=0;
	for (std::vector<ItemRecord>::iterator it = items.items.begin();  it != items.items.end();  ++it) {
		if (correctType(it->type, slot)) {
			cc->choices.Add(wxString(it->name, *wxConvCurrent));
			cc->numbers.push_back(it->id);
			if (it->id == current)
				sel = ord;

			ord++;

			if (it->itemclass > 0) 
				subclassesFound.insert(pair<int,int>(it->itemclass, it->subclass));
		}
	}

	// make category list
	cc->cats.clear();
	cc->catnames.clear();

	map<pair<int,int>, int> subclasslookup;
	for (ItemSubClassDB::Iterator it=subclassdb.begin(); it != subclassdb.end(); ++it) {
		int cl = it->getInt(ItemSubClassDB::ClassID);
		int scl = it->getInt(ItemSubClassDB::SubClassID);

		// only add the subclass if it was found in the itemlist
		if (cl>0 && subclassesFound.find(pair<int,int>(cl, scl)) != subclassesFound.end()) {
			wxString str;
			if (gameVersion == 40000)
				str = CSConv(it->getString(ItemSubClassDB::NameV400 + langOffset));
			else
				str = CSConv(it->getString(ItemSubClassDB::Name + langOffset));

			int hands;
			if (gameVersion == 40000)
				hands = it->getInt(ItemSubClassDB::HandsV400);
			else
				hands = it->getInt(ItemSubClassDB::Hands);
			if (hands > 0) {
				str << wxT(" (") << hands << wxT("-handed)");

				//char buf[16];
				//sprintf(buf, " (%d-handed)", hands);
				//str.append(buf);
			}
			cc->catnames.Add(str.c_str());
			subclasslookup[pair<int,int>(cl,scl)] = (int)cc->catnames.size()-1;
		}
	}

	if (subclassesFound.size() > 1) {
		// build category list
		for (size_t i=0; i<cc->numbers.size(); i++) {
			ItemRecord r = items.getById(cc->numbers[i]);
			cc->cats.push_back(subclasslookup[pair<int,int>(r.itemclass, r.subclass)]);
		}

		cc->itemDialog = new CategoryChoiceDialog(cc, UPDATE_CREATURE_ITEM, parent, _("Choose an item"), _("Select a Weapon"), cc->choices, cc->cats, cc->catnames, 0);
	} else {
		cc->itemDialog = new FilteredChoiceDialog(cc, UPDATE_CREATURE_ITEM, parent, _("Choose an item"), _("Select a Weapon"), cc->choices, 0);
	}

	cc->itemDialog->SetSelection(sel);

	wxSize s = cc->itemDialog->GetSize();
	const int w = 250;
	if (s.GetWidth() > w) {
		cc->itemDialog->SetSizeHints(w,-1,-1,-1,-1,-1);
		cc->itemDialog->SetSize(w, -1);
	}

	cc->itemDialog->Move(parent->GetPosition() + wxPoint(4,64));
	cc->itemDialog->Show();
	cc->choosingSlot = slot;
}

// Enchants Dialog

BEGIN_EVENT_TABLE(EnchantsDialog, wxDialog)
	EVT_BUTTON(ID_ENCHANTSOK, EnchantsDialog::OnClick)
	EVT_BUTTON(ID_ENCHANTSCANCEL, EnchantsDialog::OnClick)
END_EVENT_TABLE()

EnchantsDialog::EnchantsDialog(wxWindow *parent, CharControl *cc)
{
	charControl = cc;
	Initiated = false;
	EnchantsInitiated = false;

	LHandEnchant = -1;
	RHandEnchant = -1;

	slot = NULL;
	effectsListbox = NULL;

	Create(parent, -1, _T("Weapon Enchants"), wxDefaultPosition, wxSize(200,350), wxDEFAULT_DIALOG_STYLE);
	Show(false);
}

void EnchantsDialog::OnClick(wxCommandEvent &event)
{
	if (event.GetId() == ID_ENCHANTSOK) {
		wxString sel(effectsListbox->GetStringSelection());

		if (sel.IsEmpty()) {
			Show(false);
			return;
		}

		if (sel==_T("NONE") || sel==_T("None")) {
			if (slot->GetSelection() == 0)
				RHandEnchant = -1;
			else
				LHandEnchant = -1;
			Show(false);
			return;
		}
		
		for (std::vector<EnchantsRec>::iterator it=enchants.begin();  it!=enchants.end();  ++it) {
			if (wxString(it->name.c_str(), wxConvUTF8) == sel) {
				int s = slot->GetSelection();
				s += 10;

				if (!charControl->model)
					return;

				if (!slotHasModel(s))
					return;
				
				if (slot->GetSelection() == 0)
					RHandEnchant = it->id;
				else
					LHandEnchant = it->id;

				// children:
				for (size_t i=0; i < charControl->charAtt->children.size(); i++) {
					if (charControl->charAtt->children[i]->slot == s) {
						Attachment *att = charControl->charAtt->children[i];
						if (att->children.size() > 0)
							att->delChildren();

						Model *m = static_cast<Model*>(att->model);
						if (!m)
							return;

						for (int k=0; k<5; k++) {
							if ((it->index[k] > 0) && (m->attLookup[k]>=0)) {
								ItemVisualEffectDB::Record rec = effectdb.getById(it->index[k]);
								att->addChild(rec.getString(ItemVisualEffectDB::Model).mb_str(), k, -1);
							}
						}
						break;
					}
				}

				Show(false);
				return;
			}
		}
		
	} else if (event.GetId() == ID_ENCHANTSCANCEL)
		this->Show(false);
}

void EnchantsDialog::InitObjects()
{
	wxString slots[2] = {_T("Right Hand"), _T("Left Hand")};

	slot = new wxRadioBox(this, -1, _T("Apply effects to:"), wxPoint(10,10), wxSize(180, 80), 2, slots, 4, wxRA_SPECIFY_ROWS, wxDefaultValidator, _T("radioBox"));

	text1 = new wxStaticText(this, -1, _T("Enchantments:"), wxPoint(10, 110), wxDefaultSize);
	effectsListbox = new wxListBox(this, -1, wxPoint(10,130), wxSize(180,160), choices, wxLB_SINGLE);
	
	wxButton *btnOK = new wxButton(this, ID_ENCHANTSOK, _("OK"), wxPoint(90,295), wxSize(50,22));
	wxButton *btnCancel = new wxButton(this, ID_ENCHANTSCANCEL, _("Cancel"), wxPoint(140,295), wxSize(50,22));

	Initiated = true;
}

void EnchantsDialog::InitEnchants()
{
	EnchantsRec temp;
/*
	ifstream fin("enchants.csv");
	char line[512];
	while (fin.getline(line,512)) {
		temp.id = 0;
		temp.index[0] = 0;
		temp.index[1] = 0;
		temp.index[2] = 0;
		temp.index[3] = 0;
		temp.index[4] = 0;
		temp.name = "nil";

		sscanf(line, "%u,%u,%u,%u,%u,%u", &temp.id, &temp.index[0], &temp.index[1], &temp.index[2], &temp.index[3], &temp.index[4]);
		for (size_t i=strlen(line)-2; i>1; i--) {
			if (line[i]==',') {
				temp.name = (line + i + 1);
				break;
			}
		}
		enchants.push_back(temp);
	}
	fin.close();
*/
	//std::sort(enchants.begin(), enchants.end());

	// Alfred 2009.07.17 rewrite, use system database
	temp.id = 0;
	temp.index[0] = 0;
	temp.index[1] = 0;
	temp.index[2] = 0;
	temp.index[3] = 0;
	temp.index[4] = 0;
	temp.name = "None";
	enchants.push_back(temp);

	for (SpellItemEnchantmentDB::Iterator it=spellitemenchantmentdb.begin();  it!=spellitemenchantmentdb.end(); ++it) {
		int visualid;
		if (gameVersion == 40000)
			visualid = it->getInt(SpellItemEnchantmentDB::VisualIDV400);
		else
			visualid = it->getInt(SpellItemEnchantmentDB::VisualID);
		if (visualid < 1)
			continue;
		for (ItemVisualsDB::Iterator it2=itemvisualsdb.begin();  it2!=itemvisualsdb.end(); ++it2) {
			if (it2->getInt(ItemVisualsDB::VisualID) == visualid) {
				temp.id = visualid;
				for(size_t i=0; i<5; i++)
					temp.index[i] = it2->getInt(ItemVisualsDB::VisualID+1+i);
				temp.name = CSConv(it->getString(SpellItemEnchantmentDB::Name + langOffset)).mb_str();
				enchants.push_back(temp);
				break;
			}
		}
	}

	choices.Clear();
	for (std::vector<EnchantsRec>::iterator it=enchants.begin();  it!=enchants.end();  ++it)
		choices.Add(wxString(it->name.c_str(), *wxConvCurrent));

	EnchantsInitiated = true;
}

void EnchantsDialog::Display()
{
	if (!EnchantsInitiated)
		InitEnchants();

	if (!Initiated)
		InitObjects();

	if (Initiated) {
		Center();
		Show(true);
	}
}
