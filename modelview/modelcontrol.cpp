
#include "modelcontrol.h"
#include "mpq.h"
#include "CxImage/ximage.h"

IMPLEMENT_CLASS(ModelControl, wxWindow)

BEGIN_EVENT_TABLE(ModelControl, wxWindow)
	EVT_CHECKLISTBOX(ID_MODEL_GEOSETS, ModelControl::OnList)

	EVT_COMBOBOX(ID_MODEL_NAME, ModelControl::OnCombo)
	EVT_COMBOBOX(ID_MODEL_LOD, ModelControl::OnCombo)

	EVT_CHECKBOX(ID_MODEL_BONES, ModelControl::OnCheck)
	EVT_CHECKBOX(ID_MODEL_BOUNDS, ModelControl::OnCheck)
	EVT_CHECKBOX(ID_MODEL_RENDER, ModelControl::OnCheck)
	EVT_CHECKBOX(ID_MODEL_WIREFRAME, ModelControl::OnCheck)
	EVT_CHECKBOX(ID_MODEL_PARTICLES, ModelControl::OnCheck)
	EVT_CHECKBOX(ID_MODEL_TEXTURE, ModelControl::OnCheck)

	EVT_COMMAND_SCROLL(ID_MODEL_ALPHA, ModelControl::OnSlider)
	EVT_COMMAND_SCROLL(ID_MODEL_SCALE, ModelControl::OnSlider)
END_EVENT_TABLE()


// ModelName
// LevelOfDetail
// Opacity
// Bones
// Bounding Box
// Render
// Geosets
// Future Additions:
//		- Pos
//		- Rotation
//		- Scale
//		- Attach model

ModelControl::ModelControl(wxWindow* parent, wxWindowID id)
{
	model = NULL;
	att = NULL;

	wxLogMessage(_T("Creating Model Control..."));

	if (Create(parent, id, wxDefaultPosition, wxSize(160,460), 0, _T("ModelControlFrame")) == false) {
		wxLogMessage(_T("GUI Error: Failed to create a window for our ModelControl!"));
		return;
	}

	try {
		modelname = new wxComboBox(this, ID_MODEL_NAME, wxEmptyString, wxPoint(5,5), wxSize(150,16), 0, NULL, wxCB_READONLY);
		
		lblLod = new wxStaticText(this, wxID_ANY, _("View"), wxPoint(5,25), wxDefaultSize);
		lod = new wxComboBox(this, ID_MODEL_LOD, wxEmptyString, wxPoint(5,40), wxSize(120,16), 0, NULL, wxCB_READONLY, wxDefaultValidator, _T("LOD")); //|wxCB_SORT); //wxPoint(66,10)
		lod->Enable(false);

		lblAlpha = new wxStaticText(this, wxID_ANY, _("Alpha"), wxPoint(5,65), wxDefaultSize);
		alpha = new wxSlider(this, ID_MODEL_ALPHA, 100, 0, 100, wxPoint(45, 65), wxSize(110, 30), wxSL_HORIZONTAL);
		
		lblScale = new wxStaticText(this, wxID_ANY, _("Scale"), wxPoint(5,90), wxDefaultSize);
		scale = new wxSlider(this, ID_MODEL_SCALE, 100, 10, 300, wxPoint(45, 90), wxSize(110, 30), wxSL_HORIZONTAL);

		bones = new wxCheckBox(this, ID_MODEL_BONES, _("Bones"), wxPoint(5, 115), wxDefaultSize);
		box = new wxCheckBox(this, ID_MODEL_BOUNDS, _("Bounds"), wxPoint(5, 135), wxDefaultSize);
		render = new wxCheckBox(this, ID_MODEL_RENDER, _("Render"), wxPoint(5, 155), wxDefaultSize);
		wireframe = new wxCheckBox(this, ID_MODEL_WIREFRAME, _("Wireframe"), wxPoint(5, 175), wxDefaultSize);
		texture = new wxCheckBox(this, ID_MODEL_TEXTURE, _("Texture"), wxPoint(5, 195), wxDefaultSize);
		particles = new wxCheckBox(this, ID_MODEL_PARTICLES, _("Particles"), wxPoint(5, 215), wxDefaultSize);

		lblGeosets = new wxStaticText(this, wxID_ANY, _("Show Geosets"), wxPoint(5,235), wxDefaultSize);
		geosets = new wxCheckListBox(this, ID_MODEL_GEOSETS, wxPoint(5, 250), wxSize(150,120), 0, NULL, 0, wxDefaultValidator, _T("GeosetsList"));
		
		wxStaticText *lblXYZ = new wxStaticText(this, wxID_ANY, _T("X\nY\nZ"), wxPoint(2,380), wxSize(20,60));
		txtX = new wxTextCtrl(this, ID_MODEL_X, _T("0.0"), wxPoint(25,380), wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator);
		txtY = new wxTextCtrl(this, ID_MODEL_Y, _T("0.0"), wxPoint(25,400), wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator);
		txtZ = new wxTextCtrl(this, ID_MODEL_Z, _T("0.0"), wxPoint(25,420), wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator);

	} catch(...) {};
}

ModelControl::~ModelControl()
{
	modelname->Destroy();
	lod->Destroy();
	alpha->Destroy();
	scale->Destroy();
	bones->Destroy();
	box->Destroy();
	render->Destroy();
	wireframe->Destroy();
	texture->Destroy();
	particles->Destroy();
	geosets->Destroy();
	txtX->Destroy();
	txtY->Destroy();
	txtZ->Destroy();
}

// Iterates through all the models counting and creating a list
void ModelControl::RefreshModel(Attachment *root)
{
	try {
		attachments.clear();

		Model *m = static_cast<Model*>(root->model);
		if (m) {
			wxASSERT(m);
			attachments.push_back(root);
			if (!init)
				UpdateModel(root);
		}
		
		for (std::vector<Attachment *>::iterator it=root->children.begin(); it!=root->children.end(); ++it) {
			//m = NULL;
			m = static_cast<Model*>((*it)->model);
			if (m) {
				attachments.push_back((*it));
				if (!init)
					UpdateModel((*it));
			}

			for (std::vector<Attachment *>::iterator it2=(*it)->children.begin(); it2!=(*it)->children.end(); ++it2) {
				m = static_cast<Model*>((*it2)->model);
				if (m) {
					//models.push_back(m);
					attachments.push_back((*it2));
					if (!init)
						UpdateModel((*it2));
				}

				for (std::vector<Attachment *>::iterator it3=(*it2)->children.begin(); it3!=(*it2)->children.end(); ++it3) {
					m = static_cast<Model*>((*it3)->model);
					if (m) {
						//models.push_back(m);
						attachments.push_back((*it3));
						if (!init)
							UpdateModel((*it3));
					}
				}
			}
		}
		
		// update combo box with the list of models?
		wxString tmp;
		modelname->Clear();
		for (std::vector<Attachment*>::iterator it=attachments.begin(); it!=attachments.end(); ++it) {
			m = static_cast<Model*>((*it)->model);
			if (m) {
				tmp = wxString(m->name.c_str(), wxConvUTF8);
				modelname->Append(tmp.AfterLast('\\'));
			}
		}

		if (modelname->GetCount() > 0)
			modelname->SetSelection(0);

	} catch( ... ) {
		wxLogMessage(_T("Error: Problem occured in ModelControl::RefreshModel(Attachment *)"));
	}

}

void ModelControl::UpdateModel(Attachment *a)
{
	if (!a)
		return;

	init = false;

	Model *m = NULL;
	if (a->model)
		m = static_cast<Model*>(a->model);

	if (m) {
		init = true;
		model = m;
		att = a;

		modelname->SetLabel(wxString(m->name.c_str(), wxConvUTF8));

		Update();
	}
}

void ModelControl::Update()
{
	if (!model)
		return;

	/*
	// Loop through all the views.
	lod->Clear();
	if (model->header.nViews == 1) {
		lod->Append(_T("1 (Only View)"));
	} else if (model->header.nViews == 2) {
		lod->Append(_T("1 (Worst)"));
		lod->Append(_T("2 (Best)"));
	} else {
		lod->Append(_T("1 (Worst)"));
		for (unsigned int i=1; i<(model->header.nViews-1); i++) {
			lod->Append(wxString::Format("%i", i+1));
		}
		lod->Append(wxString::Format("%i (Best)", model->header.nViews));
	}
	lod->SetSelection(lod->GetCount() - 1);
	*/

	// Loop through all the geosets.
	wxArrayString geosetItems;
	//geosets->Clear();
	wxString meshes[19] = {_T("Hairstyles"), _T("Facial1"), _T("Facial2"), _T("Facial3"), _T("Braces"),
		_T("Boots"), wxEmptyString, _T("Ears"), _T("Wristbands"),  _T("Kneepads"),
		 _T("Pants"), _T("Pants"), _T("Tarbard"), _T("Trousers"), wxEmptyString,
		  _T("Cape"), wxEmptyString, _T("Eyeglows"), _T("Belt") };
	for (unsigned int i=0; i<model->geosets.size(); i++) {
		int mesh = model->geosets[i].id / 100;
		if (mesh < 19 && meshes[mesh] != wxEmptyString)
			geosetItems.Add(wxString::Format(_T("%i [%s]"), i, meshes[mesh].c_str()), 1);
		else
			geosetItems.Add(wxString::Format(_T("%i [%i]"), i, mesh), 1);
	}
	//geosets->InsertItems(items, 0);
	geosets->Set(geosetItems, 0);

	for (unsigned int i=0; i<model->geosets.size(); i++) {
		geosets->Check(i, model->showGeosets[i]);
	}

	bones->SetValue(model->showBones);
	box->SetValue(model->showBounds);
	render->SetValue(model->showModel);
	wireframe->SetValue(model->showWireframe);
	particles->SetValue(model->showParticles);
	texture->SetValue(model->showTexture);

	alpha->SetValue(int(model->alpha * 100));
	scale->SetValue(100);

	txtX->SetValue(wxString::Format(_T("%f"), model->pos.x));
	txtY->SetValue(wxString::Format(_T("%f"), model->pos.y));
	txtZ->SetValue(wxString::Format(_T("%f"), model->pos.z));
}

void ModelControl::OnCheck(wxCommandEvent &event)
{
	if (!init || !model)
		return;

	int id = event.GetId();

	if (id == ID_MODEL_BONES) {
		model->showBones = event.IsChecked();
	} else if (id == ID_MODEL_BOUNDS) {
		model->showBounds = event.IsChecked();
	} else if (id == ID_MODEL_RENDER) {
		model->showModel = event.IsChecked();
	} else if (id == ID_MODEL_WIREFRAME) {
		model->showWireframe = event.IsChecked();
	} else if (id == ID_MODEL_PARTICLES) {
		model->showParticles = event.IsChecked();
	} else if (id == ID_MODEL_TEXTURE) {
		model->showTexture = event.IsChecked();
	}
}

void ModelControl::OnCombo(wxCommandEvent &event)
{
	if (!init)
		return;

	int id = event.GetId();
	
	if (id == ID_MODEL_LOD) {
		int value = event.GetInt();
		
		MPQFile f(model->name.c_str());
		if (f.isEof() || (f.getSize() < sizeof(ModelHeader))) {
			wxLogMessage(_T("ERROR - unable to open MPQFile: [%s]"), model->name.c_str());
			f.close();
			return;
		}

		model->showModel = false;
		model->setLOD(f, value);
		model->showModel = true;

		/*
		for (size_t i=0; i<model->geosets.size(); i++) {
			int id = model->geosets[i].id;
			model->showGeosets[i] = (id==0);
		}

		cc->RefreshModel();
		*/

		f.close();
	} else if (id == ID_MODEL_NAME) {
		// Error check
		/* Alfred 2009/07/16 fix crash, remember CurrentSelection before UpdateModel() */
		int CurrentSelection = modelname->GetCurrentSelection();
		if (CurrentSelection < (int)attachments.size()) {
			UpdateModel(attachments[CurrentSelection]);
			att = attachments[CurrentSelection];
			model = static_cast<Model*>(attachments[CurrentSelection]->model);
			
			animControl->UpdateModel(model);
			modelname->SetSelection(CurrentSelection);
		}
	}
}

void ModelControl::OnList(wxCommandEvent &event)
{
	if (!init || !model)
		return;

	for (unsigned int i=0; i<model->geosets.size(); i++) {
		model->showGeosets[i] = geosets->IsChecked(i);
	}
}

void ModelControl::OnSlider(wxScrollEvent &event)
{
	if (!init || !model)
		return;

	int id = event.GetId();
	if (id == ID_MODEL_ALPHA) {
		model->alpha = event.GetInt() / 100.0f;
	} else if (id == ID_MODEL_SCALE) {
		att->scale = event.GetInt() / 100.0f;
	}
}


/**************************************************************************
  * ScrWindow
  *************************************************************************/

ScrWindow::ScrWindow(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(512, 512))
{
	wxImage::AddHandler(new wxPNGHandler);
	sw = new wxScrolledWindow(this);
  
	wxBitmap bmp(title, wxBITMAP_TYPE_PNG);
	sb = new wxStaticBitmap(sw, -1, bmp);

	int width = bmp.GetWidth();
	int height = bmp.GetHeight();

	CreateStatusBar();
	wxString sbarText;
	sbarText.Printf(_T("%ix%i"), width, height);
	SetStatusText(sbarText);

	sw->SetScrollbars(10, 10, width/10, height/10);
//	sw->Scroll(50,10);

	Center();
}

ScrWindow::~ScrWindow()
{
	sb->Destroy();
	sw->Destroy();
}

//manager->GetPane(this).Show(false);
/**************************************************************************
  * Model Opened
  *************************************************************************/
IMPLEMENT_CLASS(ModelOpened, wxWindow)

BEGIN_EVENT_TABLE(ModelOpened, wxWindow)
	EVT_COMBOBOX(ID_MODELOPENED_COMBOBOX, ModelOpened::OnCombo)

	EVT_BUTTON(ID_MODELOPENED_EXPORT, ModelOpened::OnButton)
	EVT_BUTTON(ID_MODELOPENED_EXPORTALL, ModelOpened::OnButton)
	EVT_BUTTON(ID_MODELOPENED_VIEW, ModelOpened::OnButton)
	EVT_BUTTON(ID_MODELOPENED_EXPORTALLPNG, ModelOpened::OnButton)
	EVT_BUTTON(ID_MODELOPENED_EXPORTALLTGA, ModelOpened::OnButton)
	EVT_CHECKBOX(ID_MODELOPENED_PATHPRESERVED, ModelOpened::OnCheck)
END_EVENT_TABLE()

ModelOpened::ModelOpened(wxWindow* parent, wxWindowID id)
{
	wxLogMessage(_T("Creating Model Opened..."));
	if (Create(parent, id, wxDefaultPosition, wxSize(700, 90), 0, _T("ModelOpenedControlFrame")) == false) {
		wxLogMessage(_T("GUI Error: Failed to create a window for our ModelOpenedControl."));
		return;
	}

	openedList = new wxComboBox(this, ID_MODELOPENED_COMBOBOX, _("Opened"), wxPoint(10,10), wxSize(500,16), 0, NULL, wxCB_READONLY, wxDefaultValidator, _("Opened")); //|wxCB_SORT); //wxPoint(66,10)
	btnExport = new wxButton(this, ID_MODELOPENED_EXPORT, _("Export"), wxPoint(10, 40), wxSize(46,20));
	btnExportAll = new wxButton(this, ID_MODELOPENED_EXPORTALL, _("Export All"), wxPoint(10+46+10, 40), wxSize(66,20));
	btnView = new wxButton(this, ID_MODELOPENED_VIEW, _("View In PNG"), wxPoint(10+46+10+66+10, 40), wxSize(86,20));
	btnView->Enable(false);
	btnExportAllPNG = new wxButton(this, ID_MODELOPENED_EXPORTALLPNG, _("Export All To PNG"), wxPoint(10+46+10+66+10+86+10, 40), wxSize(106,20));
	btnExportAllTGA = new wxButton(this, ID_MODELOPENED_EXPORTALLTGA, _("Export All To TGA"), wxPoint(10+46+10+66+10+86+10+106+10, 40), wxSize(106,20));
	chkPathPreserved = new wxCheckBox(this, ID_MODELOPENED_PATHPRESERVED, _("Path Preserved"), wxPoint(10+46+10+66+10+86+10+106+10+106+10, 40), wxDefaultSize, 0);
	chkPathPreserved->SetValue(false);
	bPathPreserved = false;
}

ModelOpened::~ModelOpened()
{
	openedList->Clear();
	openedList->Destroy();

	btnExport->Destroy();
	btnExportAll->Destroy();
	btnView->Destroy();
	btnExportAllPNG->Destroy();
	btnExportAllTGA->Destroy();
}

void ModelOpened::Export(wxString val)
{
	if (val == wxEmptyString)
		return;
	MPQFile f(val.mb_str());
	if (f.isEof()) {
		wxLogMessage(_T("Error: Could not extract %s\n"), val.c_str());
		f.close();
		return;
	}
	wxFileName fn(val);
	FILE *hFile = NULL;
	if (bPathPreserved) {
		wxFileName::Mkdir(wxGetCwd()+SLASH+wxT("Export")+SLASH+fn.GetPath(), 0755, wxPATH_MKDIR_FULL);
		hFile = fopen((wxGetCwd()+SLASH+wxT("Export")+SLASH+val).mb_str(), "wb");
	} else {
		hFile = fopen((wxGetCwd()+SLASH+wxT("Export")+SLASH+fn.GetFullName()).mb_str(), "wb");
	}
	if (hFile) {
		fwrite(f.getBuffer(), 1, f.getSize(), hFile);
		fclose(hFile);
	}
	f.close();
}

void ModelOpened::ExportPNG(wxString val, wxString suffix)
{
	if (val == wxEmptyString)
		return;
	wxFileName fn(val);
	if (fn.GetExt().Lower() != _T("blp"))
		return;
	TextureID temptex = texturemanager.add(std::string(val.mb_str()));
	Texture &tex = *((Texture*)texturemanager.items[temptex]);
	if (tex.w == 0 || tex.h == 0)
		return;

	wxString temp;

	unsigned char *tempbuf = (unsigned char*)malloc(tex.w*tex.h*4);
	tex.getPixels(tempbuf, GL_BGRA_EXT);

	CxImage *newImage = new CxImage(0);
	newImage->AlphaCreate();	// Create the alpha layer
	newImage->IncreaseBpp(32);	// set image to 32bit 
	newImage->CreateFromArray(tempbuf, tex.w, tex.h, 32, (tex.w*4), true);
	if (bPathPreserved) {
		wxFileName::Mkdir(wxGetCwd()+SLASH+wxT("Export")+SLASH+fn.GetPath(), 0755, wxPATH_MKDIR_FULL);
		temp = wxGetCwd()+SLASH+wxT("Export")+SLASH+fn.GetPath()+SLASH+fn.GetName()+wxT(".")+suffix;
	} else {
		temp = wxGetCwd()+SLASH+wxT("Export")+SLASH+fn.GetName()+wxT(".")+suffix;
	}
	//wxLogMessage(_T("Info: Exporting texture to %s..."), temp.c_str());
	if (suffix == _T("tga"))
		newImage->Save(temp.mb_str(), CXIMAGE_FORMAT_TGA);
	else
		newImage->Save(temp.mb_str(), CXIMAGE_FORMAT_PNG);
	free(tempbuf);
	newImage->Destroy();
	wxDELETE(newImage);
}


void ModelOpened::OnButton(wxCommandEvent &event)
{
	bool dialOK = true;
	int id = event.GetId();
	wxFileName::Mkdir(wxGetCwd()+SLASH+wxT("Export"), 0755, wxPATH_MKDIR_FULL);
	if (id == ID_MODELOPENED_EXPORT) {
		wxString val = openedList->GetValue();
		Export(val);
	} else if (id == ID_MODELOPENED_EXPORTALL) {
		for (unsigned int i = 0; i < opened_files.GetCount(); i++) {
			Export(opened_files[i]);
		}
	} else if (id == ID_MODELOPENED_VIEW) {
		wxString val = openedList->GetValue();
		ExportPNG(val, _T("png"));
		wxFileName fn(val);
		wxString temp;
		if (bPathPreserved)
			temp =  wxGetCwd()+SLASH+wxT("Export")+SLASH+fn.GetPath()+SLASH+fn.GetName()+wxT(".png");
		else
			temp =  wxGetCwd()+SLASH+wxT("Export")+SLASH+fn.GetName()+wxT(".png");
	    ScrWindow *sw = new ScrWindow(temp);
	    sw->Show(true);
		dialOK = false;
	} else if (id == ID_MODELOPENED_EXPORTALLPNG) {
		for (unsigned int i = 0; i < opened_files.GetCount(); i++) {
			ExportPNG(opened_files[i], _T("png"));
		}
	} else if (id == ID_MODELOPENED_EXPORTALLTGA) {
		for (unsigned int i = 0; i < opened_files.GetCount(); i++) {
			ExportPNG(opened_files[i], _T("tga"));
		}
	}

	if (dialOK) {
		wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("Export completed"), wxT("Info"), wxOK);
		dial->ShowModal();
	}
}
void ModelOpened::OnCombo(wxCommandEvent &event)
{
	int id = event.GetId();
	if (id == ID_MODELOPENED_COMBOBOX) {
		wxString val = openedList->GetValue();
		wxFileName fn(val);
		if (fn.GetExt().Lower() == _T("blp"))
			btnView->Enable(true);
		else
			btnView->Enable(false);
	}
}
void ModelOpened::OnCheck(wxCommandEvent &event)
{
	int id = event.GetId();
	if (id == ID_MODELOPENED_PATHPRESERVED) {
		bPathPreserved= event.IsChecked();
	}
}


void ModelOpened::Add(wxString str)
{
	MPQFile f(str.mb_str());
	if (f.isEof() == true)
		return;
	f.close();
	if (opened_files.Index(str, false) == wxNOT_FOUND) {
		opened_files.Add(str);
		openedList->Append(str);
	}
}

void ModelOpened::Clear()
{
	opened_files.Clear();
	openedList->Clear();
}


