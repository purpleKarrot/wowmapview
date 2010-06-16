#ifndef MODELCONTROL_H
#define MODELCONTROL_H

#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "model.h"
#include "modelcanvas.h"
#include "animcontrol.h"

#include "enums.h"

// ModelName
// LevelOfDetail
// Opacity
// Bones
// Bounding Box
// Render
// Wireframe
// Geosets
// Future Additions:
//		- Pos
//		- Rotation
//		- Scale
//		- Attach model

class ModelControl: public wxWindow
{
	DECLARE_CLASS(ModelControl)
	DECLARE_EVENT_TABLE()
	
	wxComboBox *lod, *modelname;
	wxSlider *alpha, *scale;
	wxCheckBox *bones, *box, *render, *wireframe, *texture, *particles;
	wxCheckListBox *geosets;
	wxStaticText *lblGeosets, *lblLod, *lblScale, *lblAlpha;
	wxTextCtrl *txtX, *txtY, *txtZ;

	// List of models in the scene.
	//std::vector<Model*> models;
	std::vector<Attachment*> attachments;

	bool init;
	
public:
	Model *model;	// Currently 'active' model.
	Attachment *att; // Currently 'active' attachment.
	AnimControl *animControl;

	ModelControl(wxWindow* parent, wxWindowID id);
	~ModelControl();

	void UpdateModel(Attachment *a);
	void Update();
	void RefreshModel(Attachment *root);
	void OnCheck(wxCommandEvent &event);
	void OnCombo(wxCommandEvent &event);
	void OnList(wxCommandEvent &event);
	void OnSlider(wxScrollEvent &event);

};

class ScrWindow : public wxFrame
{
	wxScrolledWindow *sw;
	wxStaticBitmap *sb;
public:
	ScrWindow(const wxString& title);
	~ScrWindow();
};

class ModelOpened: public wxWindow
{
	DECLARE_CLASS(ModelOpened)
	DECLARE_EVENT_TABLE()

	wxComboBox *openedList;
	wxButton *btnExport, *btnExportAll, *btnView, *btnExportAllPNG, *btnExportAllTGA;
	wxArrayString opened_files;
	wxCheckBox *chkPathPreserved;
	bool bPathPreserved;

public:

	ModelOpened(wxWindow* parent, wxWindowID id);
	~ModelOpened();	

	void OnButton(wxCommandEvent &event);
	void OnCombo(wxCommandEvent &event);
	void OnCheck(wxCommandEvent &event);
	void Add(wxString str);
	void Clear();
	void Export(wxString val);
	void ExportPNG(wxString val, wxString suffix);
};

#endif

