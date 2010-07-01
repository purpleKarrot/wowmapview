
#include <GL/glew.h>
#include "qtcanvas.hpp"
#include "modelcanvas.h"
#include "video.h"
#include "animcontrol.h"
#include "shaders.h"

#include "globalvars.h"

const float defaultMatrix[] = {1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000};

//float animSpeed = 1.0f;
const float piover180 = 0.0174532925f;
const float rad2deg = 57.295779513f;

// 100 fps?
const int TIME_STEP = 10; // 10 millisecs between each frame


unsigned long lastTime = 0;
unsigned long pauseTime = 0;

IMPLEMENT_CLASS(ModelCanvas, wxWindow)
BEGIN_EVENT_TABLE(ModelCanvas, wxWindow)
    EVT_TIMER(ID_TIMER, ModelCanvas::OnTimer)
END_EVENT_TABLE()


#include <sys/time.h>

int timeGetTime()
{
	static struct timeval t;
	gettimeofday(&t, NULL);

	static int start = 0;
	if (start == 0)
		start = t.tv_sec;

	return int((t.tv_sec - start) * 1000 + t.tv_usec / 1000);
}

#ifndef _WINDOWS
static int attrib[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };
#endif

ModelCanvas::ModelCanvas(wxWindow *parent, VideoCaps *caps)
#ifndef _WINDOWS
: wxGLCanvas(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxCLIP_CHILDREN|wxFULL_REPAINT_ON_RESIZE, _T("ModelCanvas"), attrib, wxNullPalette)
#endif
{
	wxLogMessage(_T("Creating OpenGL Canvas..."));

    init = false;
	initShaders = false;

	// Init time related stuff
	srand(timeGetTime());
	time = 0;
	lastTime = timeGetTime();

	// Set all our pointers to null
	model =	0;			// Main model.
	wmo = 0;			// world map object model
	adt = 0;			// ADT
	animControl = 0;
	curAtt = 0;			// Current Attachment
	root = 0;
	useCamera = false;
	
	//wxNO_BORDER|wxCLIP_CHILDREN|wxFULL_REPAINT_ON_RESIZE
#ifdef _WINDOWS
	if(!Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxCLIP_CHILDREN|wxFULL_REPAINT_ON_RESIZE, _T("ModelCanvas"))) {
		wxLogMessage(_T("Critcal Error: Unable to create a window to handle our OpenGL rendering.\n\tWon't be able to continue."));
		parent->Close();
		return;
	} else 
#endif
	{
		SetBackgroundStyle(wxBG_STYLE_CUSTOM);
		Show(true);

		// Initiate the timer that handles our animation and setting the canvas to redraw
		timer.SetOwner(this, ID_TIMER);
		timer.Start(TIME_STEP);

		wxDisplay *disp = new wxDisplay(0);
		int bpp = disp->GetCurrentMode().bpp;

		// Initiate our default OpenGL settings
		wxLogMessage(_T("Initiating OpenGL..."));
#ifdef _WINDOWS
		video.SetHandle((HWND)this->GetHandle(), bpp);
#endif
	}
	
	root = new Attachment(NULL, NULL, -1, -1);

	new_ = new qtcanvas(this);
}

ModelCanvas::~ModelCanvas()
{
	// Clear remaining textures.
	texturemanager.clear();

	// Uninitialise shaders
	UninitShaders();

	// Clear model attachments
	clearAttachments();

	wxDELETE(root);
}

void ModelCanvas::InitView()
{
	// set GL viewport
	int w=0, h=0;
	GetClientSize(&w, &h);

	video.xRes = w;
	video.yRes = h;
}

void ModelCanvas::InitShaders()
{
	fxBlur = false;
	fxGlow = false;
	fxFog = false;

	initShaders = true;
}

void ModelCanvas::UninitShaders()
{
	if (!initShaders)
		return;
}

Attachment* ModelCanvas::LoadModel(const char *fn)
{
	clearAttachments();
	root->model = NULL;

	wxDELETE(wmo);

	model = new Model(fn, true);
	if (!model->ok) {
		wxDELETE(model);
		model = NULL;
		return NULL;
	}
	root->model = model;

	curAtt = root;

	return root;
}

Attachment* ModelCanvas::LoadCharModel(const char *fn)
{
	clearAttachments();
	root->model = NULL;

	wxDELETE(wmo);

	// Create new one
	model = new Model(fn, true);
	if (!model->ok) {
		wxDELETE(model);
		model = NULL;
		return NULL;
	}

	Attachment *att = root->addChild(model, 0, -1);
	curAtt = att;
	return att;
}

void ModelCanvas::LoadADT(wxString fn)
{
	OldinitShaders();

	root->model = NULL;
	wxDELETE (adt);

	if (!adt) {
		adt = new MapTile(fn);
		if (adt->ok) {
			Vec3D vc = adt->topnode.vmax;
			if (vc.y < 0) vc.y = 0;
			adt->viewpos.y = vc.y + 50.0f;
			adt->viewpos.x = adt->xbase;
			adt->viewpos.z = adt->zbase;
			root->model = adt;
		} else
			wxDELETE(adt);
	}
}

void ModelCanvas::LoadWMO(wxString fn)
{
	if (!wmo) {
		wmo = new WMO(std::string(fn.mb_str()));
		root->model = wmo;
	}
}

void ModelCanvas::clearAttachments()
{
	if (root)
		root->delChildren();
}

void ModelCanvas::InitGL()
{
	video.InitGL();

	GLenum err = 0;

	init = true;

	// load up our shaders
	InitShaders();

	// init the default view
	InitView();
}

void ModelCanvas::RenderObjects()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	root->draw(this);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);

	root->drawParticles();

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void ModelCanvas::RenderWMO()
{
	if (!init)
		InitGL();

	InitView();

	// From what I can tell, WoW OpenGL only uses 4 g_modelViewer->lightControl->lights
	for (int i=0; i<4; i++) {
		GLuint light = GL_LIGHT0 + i;
		glLightf(light, GL_CONSTANT_ATTENUATION, 0.0f);
		glLightf(light, GL_LINEAR_ATTENUATION, 0.7f);
		glLightf(light, GL_QUADRATIC_ATTENUATION, 0.03f);
		glDisable(light);
	}

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Vec4D(0.35f, 0.35f, 0.35f, 1.0f));
	glColor3f(1.0f, 1.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	root->draw(this);
	//root->drawParticles(true);
}

void ModelCanvas::RenderADT()
{
	if (!init)
		InitGL();

	InitView();

	// From what I can tell, WoW OpenGL only uses 4 g_modelViewer->lightControl->lights
	for (int i=0; i<4; i++) {
		GLuint light = GL_LIGHT0 + i;
		glLightf(light, GL_CONSTANT_ATTENUATION, 0.0f);
		glLightf(light, GL_LINEAR_ATTENUATION, 0.7f);
		glLightf(light, GL_QUADRATIC_ATTENUATION, 0.03f);
		glDisable(light);
	}

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Vec4D(0.35f, 0.35f, 0.35f, 1.0f));
	glColor3f(1.0f, 1.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	root->draw(this);
	//root->drawParticles(true);
}

void Attachment::draw(ModelCanvas *c)
{
	if (!c)
		return;

	glPushMatrix();
	
	if (model) {
		//model->reset();
		setup();

		Model *m = static_cast<Model*>(model);
		
		if (!m)
			return;

		if (c->model) {
			// no need to scale if its already 100%
			if (scale != 1.0f)
				glScalef(scale, scale, scale);

			if (pos != Vec3D(0.0f, 0.0f, 0.0f))
				glTranslatef(pos.x, pos.y, pos.z);

			if (rot != Vec3D(0.0f,0.0f,0.0f)) {
				glRotatef(rot.x, 1.0f, 0.0f, 0.0f);
				glRotatef(rot.y, 0.0f, 1.0f, 0.0f);
				glRotatef(rot.z, 0.0f, 0.0f, 1.0f);
			}

			
			if (m->showModel && (m->alpha!=1.0f)) {
				glDisable(GL_COLOR_MATERIAL);

				float a[] = {1.0f, 1.0f, 1.0f, m->alpha};
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, a);

				glEnable(GL_BLEND);
				//glDisable(GL_DEPTH_TEST);
				//glDepthMask(GL_FALSE);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}

			if (!m->showTexture)
				glDisable(GL_TEXTURE_2D);
			else
				glEnable(GL_TEXTURE_2D);
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// We call this no matter what so that the model will still 'animate'.
		// and we do the 'showmodel' check inside the function
		model->draw();

		if (c->model) {	
			if (m->showModel && (m->alpha!=1.0f)) {
				float a[] = {1.0f, 1.0f, 1.0f, 1.0f};
				glMaterialfv(GL_FRONT, GL_DIFFUSE, a);

				glDisable(GL_BLEND);
				//glEnable(GL_DEPTH_TEST);
				//glDepthMask(GL_TRUE);
				glEnable(GL_COLOR_MATERIAL);
			}

				glDisable(GL_LIGHTING);
				glDisable(GL_TEXTURE_2D);

				if (m->showBounds) 
					m->drawBoundingVolume();
					
				if (m->showBones) 
					m->drawBones();
					
				glEnable(GL_LIGHTING);
		}
	}

	// children:
	for (size_t i=0; i<children.size(); i++)
		children[i]->draw(c);
	
	glPopMatrix();
}

void Attachment::drawParticles(bool force)
{
	glPushMatrix();

	if (model) {
		Model *m = static_cast<Model*>(model);
		if (!m)
			return;
		
		model->reset();
		setupParticle();

		// no need to scale if its already 100%
		if (scale != 1.0f)
			glScalef(scale, scale, scale);

		if (rot != Vec3D(0.0f, 0.0f, 0.0f))
			glRotatef(rot.y, 0.0f, 1.0f, 0.0f);

		//glRotatef(45.0f, 1,0,0);

		if (pos != Vec3D(0.0f, 0.0f, 0.0f))
			glTranslatef(pos.x, pos.y, pos.z);

		if (force)
			m->drawParticles();
		else if (m->hasParticles && m->showParticles) 
			m->drawParticles();
				
	}
	
	// children:
	for (size_t i=0; i<children.size(); i++)
		children[i]->drawParticles();

	glPopMatrix();
}



void Attachment::tick(float dt)
{
	if (model)
		model->update(dt);
	for (size_t i=0; i<children.size(); i++)
		children[i]->tick(dt);
}


void ModelCanvas::OnTimer(wxTimerEvent& event)
{
	int ddt = (timeGetTime() - lastTime);
	lastTime = timeGetTime();

	globalTime += ddt;

	if (model) {
		if (model->animManager && !wmo) {
			if (model->animManager->IsPaused())
				ddt = 0;
			
			if (!model->animManager->IsParticlePaused())
				ddt = model->animManager->GetTimeDiff();
		}
		
		root->tick(ddt);
	}

	new_->updateGL();
}

void ModelCanvas::ResetViewWMO(int id)
{
	if (!wmo || id>=wmo->nGroups) 
		return;

	Vec3D mid;

	if (id==-1) {
		mid = (wmo->v1+wmo->v2)*0.3f;
	} else {
		// zoom/center on current WMO group
		WMOGroup &g = wmo->groups[id];
		//model->pos.z = (g.v2-g.v1).length();
		mid = (g.v1+g.v2)*0.5f;
	}
}

void Attachment::setup()
{
	if (parent==0) 
		return;
	if (parent->model) 
		parent->model->setupAtt(id);
}

void Attachment::setupParticle()
{
	if (parent==0) 
		return;
	if (parent->model) 
		parent->model->setupAtt2(id);
}

Attachment* Attachment::addChild(const char *modelfn, int id, int slot, float scale, float rot, Vec3D pos)
{
	if (!modelfn || !strlen(modelfn) || id<0) 
		return 0;

	Model *m = new Model(modelfn, true);

	if (m && m->ok) {
		return addChild(m, id, slot, scale, rot, pos);
	} else {
		wxDELETE(m);
		return 0;
	}
}

Attachment* Attachment::addChild(Displayable *disp, int id, int slot, float scale, float rot, Vec3D pos)
{
	Attachment *att = new Attachment(this, disp, id, slot, scale, rot, pos);
	children.push_back(att);
	return att;
}

void Attachment::delChildren()
{
	for (size_t i=0; i<children.size(); i++) {
		children[i]->delChildren();

		wxDELETE(children[i]->model);
		wxDELETE(children[i]);
	}

	children.clear();
}

void Attachment::delSlot(int slot)
{
	for (size_t i=0; i<children.size(); ) {
		if (children[i]->slot == slot) {
			wxDELETE(children[i]);
			children.erase(children.begin() + i);
		} else i++;
	}
}

Model* Attachment::getModelFromSlot(int slot)
{
	for (size_t i=0; i<children.size(); ) {
		if (children[i]->slot == slot) {
			return (static_cast<Model*>(children[i]->model));
		} else i++;
	}

	return NULL;
}

