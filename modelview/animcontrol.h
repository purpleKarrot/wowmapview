#ifndef ANIMCONTROL_H
#define ANIMCONTROL_H

#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//#include "model.h"
//#include "wmo.h"
#include "modelcanvas.h"
#include "database.h"

extern float animSpeed;

// AnimationData.dbc
#define	ANIM_STAND	0

struct TextureGroup {
	static const int num = 3;
	int base, count;
	std::string tex[num];
	TextureGroup()
	{
		for (int i=0; i<num; i++) {
			tex[i] = "";
		}
	}

	// default copy constr
	TextureGroup(const TextureGroup &grp)
	{
		for (int i=0; i<num; i++) {
			tex[i] = grp.tex[i];
		}
		base = grp.base;
		count = grp.count;
	}
	const bool operator<(const TextureGroup &grp) const
	{
		for (int i=0; i<num; i++) {
			if (tex[i]<grp.tex[i]) return true;
			if (tex[i]>grp.tex[i]) return false;
		}
		return false;
	}
};

typedef std::set<TextureGroup> TextureSet;

class AnimControl: public wxWindow
{
	DECLARE_CLASS(AnimControl)
    DECLARE_EVENT_TABLE()

	wxComboBox *animCList, *animCList2, *animCList3, *wmoList, *loopList;
	wxStaticText *wmoLabel,*speedLabel, *speedMouthLabel, *frameLabel;
	wxSlider *speedSlider, *speedMouthSlider, *frameSlider;
	wxButton *btnAdd;
	wxCheckBox *lockAnims;

	wxButton *btnPlay, *btnPause, *btnStop, *btnClear, *btnPrev, *btnNext;
	wxCheckBox *oldStyle;

	bool UpdateCreatureModel(Model *m);
	bool UpdateItemModel(Model *m);
	bool FillSkinSelector(TextureSet &skins);

public:
	AnimControl(wxWindow* parent, wxWindowID id);
	~AnimControl();

	wxComboBox *skinList;

	void UpdateModel(Model *m);
	void UpdateWMO(WMO *w, int group);

	void OnButton(wxCommandEvent &event);
	void OnCheck(wxCommandEvent &event);
	void OnAnim(wxCommandEvent &event);
	void OnSkin(wxCommandEvent &event);
	void OnItemSet(wxCommandEvent &event);
	void OnSliderUpdate(wxCommandEvent &event);
	void OnLoop(wxCommandEvent &event); 

	int AddSkin(TextureGroup grp);
	void SetSkin(int num);
	void SetAnimSpeed(float speed);
	void SetAnimFrame(int frame);

	static wxString makeSkinTexture(const char *texfn, const char *skin);

	bool randomSkins;
	bool defaultDoodads;

	std::string oldname;

	int selectedAnim;
	int selectedAnim2;
	int selectedAnim3;
	bool bOldStyle;
	bool bLockAnims;
};





#endif

