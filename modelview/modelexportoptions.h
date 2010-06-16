// Copied from Settings
#ifndef MODELEXPORTOPTIONS_H
#define MODELEXPORTOPTIONS_H

// WX Headers
#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/notebook.h"

// Custom headers
#include "util.h"
#include "enums.h"

class ModelExportOptions_General: public wxWindow
{
	DECLARE_CLASS(ModelExportOptions_General)
    DECLARE_EVENT_TABLE()

	wxCheckBox *chkbox[NUM_MEO1_CHECK];
	//wxListBox *mpqList;
	//wxTextCtrl *txtPath;

public:

	ModelExportOptions_General(wxWindow* parent, wxWindowID id);
	~ModelExportOptions_General(){};

	void Update();

	void OnButton(wxCommandEvent &event);
	void OnCheck(wxCommandEvent &event);
};

class ModelExportOptions_Lightwave: public wxWindow
{
	DECLARE_CLASS(ModelExportOptions_Lightwave)
    DECLARE_EVENT_TABLE()

	wxCheckBox *chkbox[NUM_MEO2_CHECK];
	wxComboBox *ddextype;
	//wxTextCtrl *txtFov;

public:

	ModelExportOptions_Lightwave(wxWindow* parent, wxWindowID id);
	~ModelExportOptions_Lightwave() {};

	void Update();

	void OnButton(wxCommandEvent &event);
	void OnCheck(wxCommandEvent &event);
	void OnComboBox(wxCommandEvent &event);
};

class ModelExportOptions_X3D: public wxWindow
{
    DECLARE_CLASS(ModelExportOptions_X3D)
    DECLARE_EVENT_TABLE()

    wxCheckBox *chkbox[NUM_MEO3_CHECK];

public:

    ModelExportOptions_X3D(wxWindow* parent, wxWindowID id);
    ~ModelExportOptions_X3D() {};

    void Update();

    void OnCheck(wxCommandEvent &event);
};

class ModelExportOptions_Control: public wxWindow
{
	DECLARE_CLASS(ModelExportOptions_Control)
    DECLARE_EVENT_TABLE()

	wxNotebook *notebook;
	ModelExportOptions_General *page1;
	ModelExportOptions_Lightwave *page2;
    ModelExportOptions_X3D *page3;
public:

	ModelExportOptions_Control(wxWindow* parent, wxWindowID id);
	~ModelExportOptions_Control();
	
	void Open();
	void Close();
};

#endif