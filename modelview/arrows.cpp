
#include "enums.h"
#include "arrows.h"

IMPLEMENT_CLASS(ArrowControl, wxWindow)

BEGIN_EVENT_TABLE(ArrowControl, wxWindow)
	EVT_CLOSE(ArrowControl::OnClose)

	EVT_BUTTON(ID_ARROW_ATTACH, ArrowControl::OnButton)
	EVT_BUTTON(ID_ARROW_CLEAR, ArrowControl::OnButton)

	EVT_COMBOBOX(ID_ARROW_MODEL, ArrowControl::OnCombo)
	EVT_COMBOBOX(ID_ARROW_JOINT, ArrowControl::OnCombo)

	EVT_SLIDER(ID_ARROW_SCALE, ArrowControl::OnSlider)
	EVT_SLIDER(ID_ARROW_ROTATION, ArrowControl::OnSlider)
	EVT_SLIDER(ID_ARROW_POSITION, ArrowControl::OnSlider)
END_EVENT_TABLE()


ArrowControl::ArrowControl(wxWindow* parent, wxWindowID id, const wxPoint& pos, Attachment *att)
{
	atts.clear();
	curAtt = NULL;
	charAtt = att;

	if(Create(parent, id, pos, wxSize(140, 300), 0, _T("ArrowControlFrame")) == false) {
		wxMessageBox(_T("Failed to create a window for our ArrowControl!"), _T("ERROR"));
		wxLogMessage(_T("ERROR - Failed to create a window for our ArrowControl!"));
		return;
	}
	
	const wxString loc[] = {_T("Left wrist"), _T("Right palm"), _T("Left palm"),
		_T("Right elbow"), _T("Left elbow"), _T("Right shoulder"), _T("Left shoulder"),
		_T("Right knee"), _T("Left knee"), _T("Unkown"), _T("Unknown"), _T("Head"),  _T("Back"), 
		_T("Unknown"), _T("Unknown"), _T("Bust"), _T("Bust"), _T("Face"), _T("Above char"), 
		_T("Ground"), _T("Top of Head"), _T("Left palm"), _T("Right palm"), _T("Unknown"),
		_T("Unknown"), _T("Unknown"), _T("R-Back"), _T("L-Back"), _T("M-Back"), _T("Belly"),  
		_T("L-Back"), _T("R-Back"), _T("L-Hip"), _T("R-Hip"), _T("Bust"), _T("Right palm")};

	// Item\\Objectcomponents\\ammo\\ 

	joint = new wxComboBox(this, ID_ARROW_JOINT, wxEmptyString, wxPoint(5,5), wxSize(130,20), 36, loc, wxCB_READONLY);
	const wxString models[] = {_T("arrowacidflight_01.m2"), _T("arrowfireflight_01.m2"), _T("arrowflight_01.m2"), _T("arrowiceflight_01.m2"), _T("arrowmagicflight_01.m2")};
	model = new wxComboBox(this, ID_ARROW_MODEL,wxEmptyString, wxPoint(5,30), wxSize(130,20), 5, models, wxCB_READONLY);
	//tex = new wxComboBox(this, ID_ARROW_TEXTURE,wxEmptyString, wxPoint(55,5), wxSize(100,20), 0, NULL, wxCB_READONLY);
	
	attach = new wxButton(this, ID_ARROW_ATTACH, _("Attach"), wxPoint(10,55), wxSize(45,20));
	clear = new wxButton(this, ID_ARROW_CLEAR, _("Clear All"), wxPoint(80,55), wxSize(45,20));

	rot = new wxSlider(this, ID_ARROW_ROTATION, 18,0,36, wxPoint(5, 85), wxSize(130,38), wxSL_HORIZONTAL|wxSL_LABELS );
	scale = new wxSlider(this, ID_ARROW_SCALE, 5,0,20, wxPoint(5, 125), wxSize(130,38), wxSL_HORIZONTAL|wxSL_LABELS );
	position = new wxSlider(this, ID_ARROW_POSITION, 0,-50,50, wxPoint(5, 165), wxSize(130,38), wxSL_HORIZONTAL|wxSL_LABELS );
}

ArrowControl::~ArrowControl()
{
	for (size_t i=0; i<atts.size(); i++) {
		curAtt = atts[i];
		wxDELETE(atts[i]->model);
		wxDELETE(curAtt);
	}
	atts.clear();
}

void ArrowControl::OnButton(wxCommandEvent &event)
{
	int id = event.GetId();

	if(id == ID_ARROW_ATTACH) {
		std::string mp = "Item\\ObjectComponents\\Ammo\\Arrow_A_01Brown.blp";
		
		curAtt = charAtt->addChild((_T("Item\\Objectcomponents\\ammo\\") + model->GetStringSelection()).mb_str(), joint->GetSelection(), -1);
		atts.push_back(curAtt);
		
		GLuint tex = texturemanager.add(mp);
		Model *m = static_cast<Model*>(curAtt->model);
		m->replaceTextures[TEXTURE_CAPE] = tex;

		curAtt->scale = 0.5f;
		curAtt->rot = 180.0f;

		rot->SetValue(18);
		scale->SetValue(5);
		position->SetValue(0);

	} else if (id == ID_ARROW_CLEAR) {
		curAtt = NULL;
		for (size_t i=0; i<atts.size(); i++) {
			wxDELETE(atts[i]->model);
		}
		atts.clear();
	}

}

void ArrowControl::OnCombo(wxCommandEvent &event)
{


}

void ArrowControl::OnSlider(wxCommandEvent &event)
{
	int id = event.GetId();

	if (id == ID_ARROW_ROTATION) {
		curAtt->rot = event.GetInt() * 10.0f;
	} else if ( id == ID_ARROW_SCALE) {
		curAtt->scale = event.GetInt() / 10.0f;
	} else if ( id == ID_ARROW_POSITION) {
		curAtt->pos.x = event.GetInt() / 10.0f;
	}
}

void ArrowControl::OnClose(wxCloseEvent &event)
{
	Show(false);
}

