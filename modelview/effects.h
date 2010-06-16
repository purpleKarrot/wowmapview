#ifndef EFFECTS_H
#define EFFECTS_H

#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "database.h"
#include "charcontrol.h"
#include "modelcanvas.h"

extern wxArrayString spelleffects;
extern SpellEffectsDB spelleffectsdb;

void GetSpellEffects();
void SelectCreatureItem(int slot, int current, CharControl *cc, wxWindow *parent);

struct NumStringPair {
	int id;
	std::string name;

	const bool operator< (const NumStringPair &p) const {
		return name < p.name;
	}
};

struct EnchantsRec {
	unsigned int id;
	unsigned int index[5];
	std::string name;

	const bool operator< (const EnchantsRec &p) const {
		return name < p.name;
	}
};

/*
struct SpellRec {
	unsigned int index;
	unsigned int id;
	std::string name;

};
*/

class EnchantsDialog : public wxDialog {
    DECLARE_EVENT_TABLE()

	void InitObjects();
	void InitEnchants();

	CharControl *charControl;

	wxRadioBox *slot;
	wxListBox *effectsListbox;
	wxStaticText *text1;

	bool EnchantsInitiated;
	bool Initiated;

	wxArrayString choices;
	std::vector<EnchantsRec> enchants;
	
public:
    EnchantsDialog(wxWindow *parent, CharControl *cc);
	~EnchantsDialog() 
	{ 
		enchants.clear();
	}
	
	void OnClick(wxCommandEvent &event);
	void Display();

	int RHandEnchant;
	int LHandEnchant;
};


#endif
