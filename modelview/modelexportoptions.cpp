// Copied from Settings.cpp

#include "modelexportoptions.h"
#include "enums.h"

//#include "globalvars.h"


// All IDs & Vars should follow the naming structure similar to "ExportOptions_(3D Format)_(Option name)"

IMPLEMENT_CLASS(ModelExportOptions_Control, wxWindow)
IMPLEMENT_CLASS(ModelExportOptions_General, wxWindow)
IMPLEMENT_CLASS(ModelExportOptions_Lightwave, wxWindow)
IMPLEMENT_CLASS(ModelExportOptions_X3D, wxWindow)

BEGIN_EVENT_TABLE(ModelExportOptions_General, wxWindow)
	EVT_CHECKBOX(ID_EXPORTOPTIONS_PRESERVE_DIR, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_EXPORTOPTIONS_USE_WMV_POSROT, ModelExportOptions_General::OnCheck)
	/*
	EVT_BUTTON(ID_SETTINGS_UP, ModelExportOptions_General::OnButton)
	EVT_BUTTON(ID_SETTINGS_DOWN, ModelExportOptions_General::OnButton)
	EVT_BUTTON(ID_SETTINGS_ADD, ModelExportOptions_General::OnButton)
	EVT_BUTTON(ID_SETTINGS_REMOVE, ModelExportOptions_General::OnButton)
	EVT_BUTTON(ID_SETTINGS_CLEAR, ModelExportOptions_General::OnButton)
	EVT_CHECKBOX(ID_SETTINGS_RANDOMSKIN, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_SETTINGS_HIDEHELMET, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_SETTINGS_SHOWPARTICLE, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_SETTINGS_ZEROPARTICLE, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_SETTINGS_LOCALFILES, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_SETTINGS_KNIGHTEYEGLOW, ModelExportOptions_General::OnCheck)
	*/
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(ModelExportOptions_Lightwave, wxWindow)
	EVT_CHECKBOX(ID_EXPORTOPTIONS_LW_PRESERVE_DIR, ModelExportOptions_Lightwave::OnCheck)
	EVT_CHECKBOX(ID_EXPORTOPTIONS_LW_EXPORTLIGHTS, ModelExportOptions_Lightwave::OnCheck)
	EVT_CHECKBOX(ID_EXPORTOPTIONS_LW_EXPORTDOODADS, ModelExportOptions_Lightwave::OnCheck)
	EVT_COMBOBOX(ID_EXPORTOPTIONS_LW_DOODADSAS,ModelExportOptions_Lightwave::OnComboBox)
	
	// EVT_BUTTON(ID_SETTINGS_APPLY, ModelExportOptions_Lightwave::OnButton)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(ModelExportOptions_Control, wxWindow)
	
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(ModelExportOptions_X3D, wxWindow)
    EVT_CHECKBOX(ID_EXPORTOPTIONS_X3D_EXPORT_ANIMATION, ModelExportOptions_X3D::OnCheck)
    EVT_CHECKBOX(ID_EXPORTOPTIONS_X3D_CENTER_MODEL, ModelExportOptions_X3D::OnCheck)
END_EVENT_TABLE()


// --== Shared Options & Setup ==--

ModelExportOptions_General::ModelExportOptions_General(wxWindow* parent, wxWindowID id)
{
	if (Create(parent, id, wxPoint(0,0), wxSize(400,400), 0, _T("ModelExportOptions_General")) == false) {
		wxLogMessage(_T("GUI Error: ModelExportOptions_General"));
		return;
	}

	chkbox[MEO_CHECK_PRESERVE_DIR] = new wxCheckBox(this, ID_EXPORTOPTIONS_PRESERVE_DIR, _T("Preserve Directory Structure"), wxPoint(5,5), wxDefaultSize, 0);
	chkbox[MEO_CHECK_USE_WMV_POSROT] = new wxCheckBox(this, ID_EXPORTOPTIONS_USE_WMV_POSROT, _T("Use Position and Rotation from WMV"), wxPoint(5,22), wxDefaultSize, 0);
	
}


void ModelExportOptions_General::OnButton(wxCommandEvent &event)
{
	int id = event.GetId();
}

void ModelExportOptions_General::OnCheck(wxCommandEvent &event)
{
	int id = event.GetId();

	if (id==ID_EXPORTOPTIONS_PRESERVE_DIR){
		modelExport_PreserveDir = event.IsChecked();
	}else if (id==ID_EXPORTOPTIONS_USE_WMV_POSROT){
		modelExport_UseWMVPosRot = event.IsChecked();
	}
}

void ModelExportOptions_General::Update()
{
	chkbox[MEO_CHECK_PRESERVE_DIR]->SetValue(modelExport_PreserveDir);
	chkbox[MEO_CHECK_USE_WMV_POSROT]->SetValue(modelExport_UseWMVPosRot);
}

ModelExportOptions_Control::ModelExportOptions_Control(wxWindow* parent, wxWindowID id)
{
	wxLogMessage(_T("Creating Model Export Options Control..."));
	
	if (Create(parent, id, wxDefaultPosition, wxSize(405,440), wxDEFAULT_FRAME_STYLE, _T("ModelExportOptions_ControlFrame")) == false) {
		wxLogMessage(_T("GUI Error: Failed to create the window for our ModelExportOptions_Control!"));
		return;
	}

	notebook = new wxNotebook(this, ID_EXPORTOPTIONS_TABS, wxPoint(0,0), wxSize(400,420), wxNB_TOP|wxNB_FIXEDWIDTH|wxNB_NOPAGETHEME);
	
	page1 = new ModelExportOptions_General(notebook, ID_EXPORTOPTIONS_PAGE_GENERAL);
	page2 = new ModelExportOptions_Lightwave(notebook, ID_EXPORTOPTIONS_PAGE_LIGHTWAVE);
    page3 = new ModelExportOptions_X3D(notebook, ID_EXPORTOPTIONS_PAGE_X3D);

	notebook->AddPage(page1, _T("General"), false, -1);
	notebook->AddPage(page2, _T("Lightwave"), false);
    notebook->AddPage(page3, _T("X3D and XHTML"), false);
}

ModelExportOptions_Control::~ModelExportOptions_Control()
{
	page1->Destroy();
	page2->Destroy();
    page3->Destroy();
	notebook->Destroy();
}

void ModelExportOptions_Control::Open()
{
	Show(true);

	page1->Update();
	page2->Update();
    page3->Update();
}

void ModelExportOptions_Control::Close()
{
	
}



// --== Individual Options ==--


// -= Lightwave Options =-

ModelExportOptions_Lightwave::ModelExportOptions_Lightwave(wxWindow* parent, wxWindowID id)
{
	if (Create(parent, id, wxPoint(0,0), wxSize(400,400), 0, _T("ModelExportOptions_Lightwave")) == false) {
		wxLogMessage(_T("GUI Error: ModelExportOptions_Lightwave"));
		return;
	}
	wxFlexGridSizer *top = new wxFlexGridSizer(1);

	chkbox[MEO_CHECK_PRESERVE_LWDIR] = new wxCheckBox(this, ID_EXPORTOPTIONS_LW_PRESERVE_DIR, _("Build Content Directories"), wxPoint(5,5), wxDefaultSize, 0);

	chkbox[MEO_CHECK_LW_EXPORTLIGHTS] = new wxCheckBox(this, ID_EXPORTOPTIONS_LW_EXPORTLIGHTS, _("Export Lights"), wxPoint(5,35), wxDefaultSize, 0);
	chkbox[MEO_CHECK_LW_EXPORTDOODADS] = new wxCheckBox(this, ID_EXPORTOPTIONS_LW_EXPORTDOODADS, _("Export Doodads"), wxPoint(160,35), wxDefaultSize, 0);
	top->Add(ddextype = new wxComboBox(this, ID_EXPORTOPTIONS_LW_DOODADSAS, _T("Doodads As"), wxPoint(160,50), wxSize(180, 25), 0, 0, wxCB_READONLY), 1, wxEXPAND, 10);
}

void ModelExportOptions_Lightwave::Update()
{
	chkbox[MEO_CHECK_PRESERVE_LWDIR]->SetValue(modelExport_LW_PreserveDir);
	chkbox[MEO_CHECK_LW_EXPORTLIGHTS]->SetValue(modelExport_LW_ExportLights);
	chkbox[MEO_CHECK_LW_EXPORTDOODADS]->SetValue(modelExport_LW_ExportDoodads);

	ddextype->Clear();

	ddextype->Append(wxString(wxT("as Nulls")));
	// Uncomment as we're able to do it!
	ddextype->Append(wxString(wxT("as Objects")));
	//ddextype->Append(wxString("as a Single Object"));
	//ddextype->Append(wxString("as a Single Object, Per Group"));
	ddextype->SetSelection(modelExport_LW_DoodadsAs);

	ddextype->Enable(modelExport_LW_ExportDoodads);

}

void ModelExportOptions_Lightwave::OnButton(wxCommandEvent &event)
{
	int id = event.GetId();
	
}

void ModelExportOptions_Lightwave::OnCheck(wxCommandEvent &event)
{
	int id = event.GetId();

	if (id==ID_EXPORTOPTIONS_LW_PRESERVE_DIR){
		modelExport_LW_PreserveDir = event.IsChecked();
	}else if (id==ID_EXPORTOPTIONS_LW_EXPORTLIGHTS){
		modelExport_LW_ExportLights = event.IsChecked();
	}else if (id==ID_EXPORTOPTIONS_LW_EXPORTDOODADS){
		modelExport_LW_ExportDoodads = event.IsChecked();
		ddextype->Enable(event.IsChecked());
	}
}

void ModelExportOptions_Lightwave::OnComboBox(wxCommandEvent &event)
{
	int id = event.GetId();

	if (id==ID_EXPORTOPTIONS_LW_DOODADSAS){
		modelExport_LW_DoodadsAs = ddextype->GetCurrentSelection();
	}
}



// -= X3D Options =-

ModelExportOptions_X3D::ModelExportOptions_X3D(wxWindow* parent, wxWindowID id)
{
    if (Create(parent, id, wxPoint(0,0), wxSize(400,400), 0, _T("ModelExportOptions_X3D")) == false) {
        wxLogMessage(_T("GUI Error: ModelExportOptions_X3D"));
        return;
    }
    wxFlexGridSizer *top = new wxFlexGridSizer(1);

    chkbox[MEO_CHECK_EXPORT_ANIMATION] = new wxCheckBox(this, ID_EXPORTOPTIONS_X3D_EXPORT_ANIMATION, _("Export keyframe animation"), wxPoint(5,5), wxDefaultSize, 0);
    chkbox[MEO_CHECK_CENTER_MODEL] = new wxCheckBox(this, ID_EXPORTOPTIONS_X3D_CENTER_MODEL, _("Add Transform to center model"), wxPoint(160,5), wxDefaultSize, 0);
    
    // disabled for now
    chkbox[MEO_CHECK_EXPORT_ANIMATION]->Enable(false);
}

void ModelExportOptions_X3D::Update()
{
    chkbox[MEO_CHECK_EXPORT_ANIMATION]->SetValue(modelExport_X3D_ExportAnimation);
    chkbox[MEO_CHECK_CENTER_MODEL]->SetValue(modelExport_X3D_CenterModel);
}

void ModelExportOptions_X3D::OnCheck(wxCommandEvent &event)
{
    int id = event.GetId();

    if (id==ID_EXPORTOPTIONS_X3D_EXPORT_ANIMATION){
        modelExport_X3D_ExportAnimation = event.IsChecked();
    }else if (id==ID_EXPORTOPTIONS_X3D_CENTER_MODEL){
        modelExport_X3D_CenterModel = event.IsChecked();
    }
}
