#ifndef MODELBANKCONTROL_H
#define MODELBANKCONTROL_H

#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/arrstr.h>

#include "vec3d.h"
#include "enums.h"
#include <vector>

struct ModelBank
{
	// Standard
	wxString name;
	wxString fileName;
	ModelType modelType;

	// Non-Char info
	wxArrayString textures;

	Vec3D pos;
	Vec3D rot;

	// Char and NPC character info
	unsigned int skinColor;
	unsigned int faceType;
	unsigned int hairColor;
	unsigned int hairStyle;
	unsigned int facialHair;
	unsigned int facialColor;
	unsigned int race, gender;
	unsigned int useNPC;
	bool showUnderwear, showEars, showHair, showFacialHair, showFeet;
	int equipment[NUM_CHAR_SLOTS];
	// -----------

	
};

class ModelBankControl: public wxWindow
{
	DECLARE_CLASS(ModelBankControl)
	DECLARE_EVENT_TABLE()
	
	wxButton *btnAdd, *btnRemove, *btnDisplay;
	wxListBox *lstBank;
	wxStaticText *lblName;
	wxTextCtrl *txtName;

	std::vector<ModelBank> bankList;

public:
	ModelBankControl(wxWindow* parent, wxWindowID id);
	~ModelBankControl();

	// Gui events
	void OnButton(wxCommandEvent &event);
	//void OnSelect(wxCommandEvent &event);

	// functions / routines
	void LoadModel();
	void AddModel();
	void RemoveModel();
	void UpdateList();

	// file routines
	void SaveList();
	void LoadList();
};


#endif
// --
