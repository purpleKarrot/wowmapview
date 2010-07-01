
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
	EVT_SIZE(ModelCanvas::OnSize)
	EVT_PAINT(ModelCanvas::OnPaint)
	EVT_ERASE_BACKGROUND(ModelCanvas::OnEraseBackground)
    EVT_TIMER(ID_TIMER, ModelCanvas::OnTimer)
    EVT_MOUSE_EVENTS(ModelCanvas::OnMouse)
	EVT_KEY_DOWN(ModelCanvas::OnKey)
END_EVENT_TABLE()


#ifdef _WINDOWS // The following time related functions COULD be 64bit incompatible.
	// for timeGetTime:
	#pragma comment(lib,"Winmm.lib")

#else // for linux
	#include <sys/time.h>

	//typedef int DWORD;
	int timeGetTime()
	{
		static int start=0;
		static struct timeval t;
		gettimeofday(&t, NULL);
		if (start==0){
			start = t.tv_sec;
		}

		return (int)((t.tv_sec-start)*1000 + t.tv_usec/1000);
	}
#endif

#ifndef _WINDOWS
namespace {
	int attrib[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };
}
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
	skyModel = 0;		// SkyBox Model
	wmo = 0;			// world map object model
	adt = 0;			// ADT
	animControl = 0;
	curAtt = 0;			// Current Attachment
	root = 0;
	sky = 0;

	lightType = LIGHT_DYNAMIC;

	// Setup our default colour values.
	vecBGColor = Vec3D((float)(71.0/255),(float)(95.0/255),(float)(121.0/255)); 

	drawSky = false;
	bMouseLight = false;
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
	sky = new Attachment(NULL, NULL, -1, -1);

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
	wxDELETE(sky);
}

void ModelCanvas::OnEraseBackground(wxEraseEvent& event)
{
    event.Skip();
}

void ModelCanvas::OnSize(wxSizeEvent& event)
{
	event.Skip();

	if (init) 
		InitView();
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

	ResetView();

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
	
	ResetView();

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

	if (sky)
		sky->delChildren();
}

void ModelCanvas::OnMouse(wxMouseEvent& event)
{
	if (!model && !wmo && !adt)
		return;

	if (event.Button(wxMOUSE_BTN_ANY) == true)
		SetFocus();

	int px = event.GetX();
	int py = event.GetY();
	int pz = event.GetWheelRotation();

	// mul = multiplier in which to multiply everything to achieve a sense of control over the amount to move stuff by
	float mul = 1.0f;
	if (event.m_shiftDown)
		mul /= 10;
	if (event.m_controlDown)
		mul *= 10;
	if (event.m_altDown)
		mul *= 50;

	if (wmo) {

		if (event.ButtonDown()) {
			mx = px;
			my = py;

		} else if (event.Dragging()) {
			int dx = mx - px;
			int dy = my - py;
			mx = px;
			my = py;

			if (event.LeftIsDown() && event.RightIsDown()) {
				wmo->viewpos.y -= dy*mul;
			} else if (event.LeftIsDown()) {
				wmo->viewrot.x -= dx*mul/5;
				wmo->viewrot.y -= dy*mul/5;
			} else if (event.RightIsDown()) {
				wmo->viewrot.x -= dx*mul/5;
				float f = cos(wmo->viewrot.y * piover180);
				float sf = sin(wmo->viewrot.x * piover180);
				float cf = cos(wmo->viewrot.x * piover180);
				wmo->viewpos.x -= sf * mul * dy * f;
				wmo->viewpos.z += cf * mul * dy * f;
				wmo->viewpos.y += sin(wmo->viewrot.y * piover180) * mul * dy;
			} else if (event.MiddleIsDown()) {
				//?
			}

		} else if (event.GetEventType() == wxEVT_MOUSEWHEEL) {
			//?
		}

	} else if (model) {
		if (model->animManager)
			mul *= model->animManager->GetSpeed(); //animSpeed;

		if (event.ButtonDown()) {
			mx = px;
			my = py;

			if (bMouseLight) // going to use vRot to hold our temp light position (technically, our g_modelViewer->lightControl->lights rotation).
				vRot0 = Vec3D(g_modelViewer->lightControl->GetCurrentPos().x,g_modelViewer->lightControl->GetCurrentPos().y,g_modelViewer->lightControl->GetCurrentPos().z);
			else
				vRot0 = model->rot;

			vPos0 = model->pos;

		} else if (event.Dragging()) {
			int dx = mx - px;
			int dy = my - py;

			if (event.LeftIsDown()) {
				if (bMouseLight)
					return;

				model->rot.x = vRot0.x - (dy / 2.0f); // * mul);
				model->rot.y = vRot0.y - (dx / 2.0f); // * mul);

				//viewControl->Refresh();

			} else if (event.RightIsDown()) {
				mul /= 100.0f;

				if (bMouseLight) {
					Vec4D temp = g_modelViewer->lightControl->GetCurrentPos();
					temp.y = vRot0.y + dy*mul;
					temp.x = vRot0.x - dx*mul;
					g_modelViewer->lightControl->SetPos(temp);
					g_modelViewer->lightControl->Update();
				} else {
					model->pos.x = vPos0.x - dx*mul;
					model->pos.y = vPos0.y + dy*mul;

					//viewControl->Refresh();
				}

			} else if (event.MiddleIsDown()) {
				if (!event.m_altDown) {
					mul = (mul / 20.0f) * dy;

					if (bMouseLight) {
						Vec4D temp = g_modelViewer->lightControl->GetCurrentPos();
						temp.z = vRot0.z - mul;
						g_modelViewer->lightControl->SetPos(temp); 
						g_modelViewer->lightControl->Update();
					} else {
						Zoom(mul, false);
						my = py;
					}

				} else {
					mul = (mul / 1200.0f) * dy;
					Zoom(mul, true);
					my = py;
				}
			}

		} else if (event.GetEventType() == wxEVT_MOUSEWHEEL) {
			if (pz != 0) {
				mul = (mul / 120.0f) * pz;
				if (!wxGetKeyState(WXK_ALT)) {
					if (bMouseLight) {
						Vec4D temp = g_modelViewer->lightControl->GetCurrentPos();
						temp.z -= mul / 10.0f;
						g_modelViewer->lightControl->SetPos(temp); 
						g_modelViewer->lightControl->Update();
					} else {
						Zoom(mul, false);
					}
				} else {
					mul /= 50.0f;
					Zoom(mul, true);
				}
			}
		}
	} else if (adt) {
		// Copied from WMO controls.

		if (event.ButtonDown()) {
			mx = px;
			my = py;

		} else if (event.Dragging()) {
			int dx = mx - px;
			int dy = my - py;
			mx = px;
			my = py;

			if (event.LeftIsDown() && event.RightIsDown()) {
				adt->viewpos.y -= dy*mul;
			} else if (event.LeftIsDown()) {
				adt->viewrot.x -= dx*mul/5;
				adt->viewrot.y -= dy*mul/5;
			} else if (event.RightIsDown()) {
				adt->viewrot.x -= dx*mul/5;
				float f = cos(adt->viewrot.y * piover180);
				float sf = sin(adt->viewrot.x * piover180);
				float cf = cos(adt->viewrot.x * piover180);
				adt->viewpos.x -= sf * mul * dy * f;
				adt->viewpos.z += cf * mul * dy * f;
				adt->viewpos.y += sin(adt->viewrot.y * piover180) * mul * dy;
			} else if (event.MiddleIsDown()) {
				//?
			}

		} else if (event.GetEventType() == wxEVT_MOUSEWHEEL) {
			//?
		}
	}


	//if (event.GetEventType() == wxEVT_ENTER_WINDOW)
	//	SetFocus();
}

void ModelCanvas::InitGL()
{
	// Initiate our default OpenGL settings
	SetCurrent();
	video.InitGL();

	GLenum err = 0;

	// If no g_modelViewer->lightControl object, exit for now
	if (!g_modelViewer || !g_modelViewer->lightControl)
		return;

	// Setup lighting
	g_modelViewer->lightControl->Init();
	g_modelViewer->lightControl->UpdateGL();

	init = true;

	// load up our shaders
	InitShaders();

	// init the default view
	InitView();
}

void ModelCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	new_->updateGL();

	// Set this window handler as the reference to draw to.
	SetCurrent();
	wxPaintDC dc(this);

	if (!init)
		InitGL();

	if (video.render) {
		if (wmo)
			RenderWMO();
		else if (model)
			Render();
		else if (adt)
			RenderADT();
	}
}

inline void ModelCanvas::RenderLight(Light *l)
{
	GLUquadricObj *quadratic = gluNewQuadric();		// Storage For Our Quadratic Object & // Create A Pointer To The Quadric Object
	gluQuadricNormals(quadratic, GLU_SMOOTH);		// Create Smooth Normals

	glPushMatrix();

	//glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, l->diffuse);
	glColor4f(l->diffuse.x, l->diffuse.y, l->diffuse.z, 0.5f);

	glTranslatef(l->pos.x, l->pos.y, l->pos.z);

	// rotate the objects to point in the right direction
	//Vec3D rot(l->pos.x, l->pos.y, l->pos.z);
	//float theta = rot.thetaXZ(l->target);
	//glRotatef(theta * rad2deg, 0.0f, 1.0f, 0.0f);
	
	gluSphere(quadratic, 0.15, 8, 8);

	if (l->type == LIGHT_DIRECTIONAL) { // Directional light
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(l->target.x, l->target.y, l->target.z);
		glEnd();

	} else if (l->type == LIGHT_POSITIONAL) {	// Positional Light
		
	} else {	// Spot light
		
	}

	glEnable(GL_LIGHTING);
	glPopMatrix();
}

inline void ModelCanvas::RenderSkybox()
{
	// ************** SKYBOX *************
	glPushMatrix();		// Save the current modelview matrix
	glLoadIdentity();	// Reset it
	
	float fScale = 64.0f / skyModel->rad;
	
	glTranslatef(0.0f, 0.0f, -5.0f);	// Position the sky box
	glScalef(fScale, fScale, fScale);	// Scale it so it looks appropriate
	sky->draw(this);					// Render the skybox

	glPopMatrix();						// load the old modelview matrix that we saved previously
}

inline void ModelCanvas::RenderObjects()
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

inline void ModelCanvas::Render()
{
	glClearColor(vecBGColor.x, vecBGColor.y, vecBGColor.z, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	// (re)set the view
	InitView();

	if (drawSky && skyModel && sky->model)
		RenderSkybox();

	camera.Setup();

	// This is redundant and no longer needed.
	// all lighting stuff needs to be reorganised
	// ************* Absolute Lighting *******************
	// All our lighting related rendering code
	// Use model lighting?
	if (model && (lightType==LIGHT_MODEL_ONLY)) {
		Vec4D la;

		if (model->header.nLights > 0) {
			la = Vec4D(0.0f, 0.0f, 0.0f, 1.0f);
		} else {
			la = Vec4D(1.0f, 1.0f, 1.0f, 1.0f);
		}

		// Set the Model Ambience lighting.
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, la);

	// Dynamic
	} else if (lightType == LIGHT_DYNAMIC) {
		for (int i=0; i<MAX_LIGHTS; i++) {
			if (g_modelViewer->lightControl->lights[i].enabled && !g_modelViewer->lightControl->lights[i].relative) {
				glLightfv(GL_LIGHT0 + i, GL_POSITION, g_modelViewer->lightControl->lights[i].pos);
			}
		}
		
	// Ambient lighting is just a single colour applied to all rendered vertices.
	} else if (lightType==LIGHT_AMBIENT) {
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, g_modelViewer->lightControl->lights[0].diffuse);	// use diffuse, as thats our main 'colour setter'
	}
	// ==============================================

	// This is also redundant
	// The camera class should be taking over this crap
	// *************************
	// setup the view/projection
	if (model) {
		if (useCamera && model->hasCamera) {
			model->cam.setup();
		} else {
			// TODO: Possibly move this into the Model/Attachment/Displayable::draw() routine?
			glTranslatef(model->pos.x, model->pos.y, -model->pos.z);
			glRotatef(model->rot.x, 1.0f, 0.0f, 0.0f);
			glRotatef(model->rot.y, 0.0f, 1.0f, 0.0f);
			glRotatef(model->rot.z, 0.0f, 0.0f, 1.0f);
			// --==--
		}
	}
	// ==========================
			
	// As above for lighting
	// ************* Relative Lighting *******************
	// More lighting code, this is to setup the g_modelViewer->lightControl->lights that are 'relative' to the model.
	if (model && (lightType==LIGHT_DYNAMIC)) { // Else, for all our models, we use the new "lighting control", IF we're not using model only lighting		
		// loop through the g_modelViewer->lightControl->lights of our lighting system checking to see if they are turned on
		// and if so to apply their settings.
		for (int i=0; i<MAX_LIGHTS; i++) {
			if (g_modelViewer->lightControl->lights[i].enabled && g_modelViewer->lightControl->lights[i].relative) {
				glLightfv(GL_LIGHT0 + i, GL_POSITION, g_modelViewer->lightControl->lights[i].pos);
			}
		}
	}

	if (model)
	{
		glEnable(GL_NORMALIZE);
		RenderObjects();
		glDisable(GL_NORMALIZE);
	}

	SwapBuffers();
}

inline void ModelCanvas::RenderWMO()
{
	if (!init)
		InitGL();

	glClearColor(vecBGColor.x, vecBGColor.y, vecBGColor.z, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//SetupProjection(modelsize);
	InitView();

	// Lighting
	Vec4D la;
	// From what I can tell, WoW OpenGL only uses 4 g_modelViewer->lightControl->lights
	for (int i=0; i<4; i++) {
		GLuint light = GL_LIGHT0 + i;
		glLightf(light, GL_CONSTANT_ATTENUATION, 0.0f);
		glLightf(light, GL_LINEAR_ATTENUATION, 0.7f);
		glLightf(light, GL_QUADRATIC_ATTENUATION, 0.03f);
		glDisable(light);
	}
	la = Vec4D(0.35f, 0.35f, 0.35f, 1.0f);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, la);
	glColor3f(1.0f, 1.0f, 1.0f);
	// --==--

	/*
	// TODO: Possibly move this into the Model/Attachment/Displayable::draw() routine?
	// View
	if (model) {
		glTranslatef(model->pos.x, model->pos.y, -model->pos.z);
		glRotatef(model->rot.x, 1.0f, 0.0f, 0.0f);
		glRotatef(model->rot.y, 0.0f, 1.0f, 0.0f);
		glRotatef(model->rot.z, 0.0f, 0.0f, 1.0f);
		// --==--
	}
	*/
	camera.Setup();


	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	root->draw(this);
	//root->drawParticles(true);

	//glFlush();
	//glFinish();
	SwapBuffers();
}

inline void ModelCanvas::RenderADT()
{
	if (!init)
		InitGL();

	glClearColor(vecBGColor.x, vecBGColor.y, vecBGColor.z, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//SetupProjection(modelsize);
	InitView();

	// Lighting
	Vec4D la;
	// From what I can tell, WoW OpenGL only uses 4 g_modelViewer->lightControl->lights
	for (int i=0; i<4; i++) {
		GLuint light = GL_LIGHT0 + i;
		glLightf(light, GL_CONSTANT_ATTENUATION, 0.0f);
		glLightf(light, GL_LINEAR_ATTENUATION, 0.7f);
		glLightf(light, GL_QUADRATIC_ATTENUATION, 0.03f);
		glDisable(light);
	}
	la = Vec4D(0.35f, 0.35f, 0.35f, 1.0f);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, la);
	glColor3f(1.0f, 1.0f, 1.0f);
	// --==--

	/*
	// TODO: Possibly move this into the Model/Attachment/Displayable::draw() routine?
	// View
	if (model) {
		glTranslatef(model->pos.x, model->pos.y, -model->pos.z);
		glRotatef(model->rot.x, 1.0f, 0.0f, 0.0f);
		glRotatef(model->rot.y, 0.0f, 1.0f, 0.0f);
		glRotatef(model->rot.z, 0.0f, 0.0f, 1.0f);
		// --==--
	}
	*/
	camera.Setup();


	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	root->draw(this);
	//root->drawParticles(true);

	//glFlush();
	//glFinish();
	SwapBuffers();

	// cleanup
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

inline void Attachment::draw(ModelCanvas *c)
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

inline void Attachment::drawParticles(bool force)
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
	if (video.render && init) {
		CheckMovement();
		tick();
		Refresh(false);
	}
}

void ModelCanvas::tick()
{
	int ddt = 0;

	// Time stuff
	//time = float();
	ddt = (timeGetTime() - lastTime);// * animSpeed;
	lastTime = timeGetTime();
	// --

	globalTime += (ddt);

	if (model) {
		if (model->animManager && !wmo) {
			if (model->animManager->IsPaused())
				ddt = 0;
			
			if (!model->animManager->IsParticlePaused())
				ddt = model->animManager->GetTimeDiff();
		}
		
		root->tick(ddt);
	}

	if (drawSky && sky && skyModel) {
		sky->tick(ddt);
	}

}

/*
void ModelCanvas::TogglePause()
{
	if (!bPaused) {
		// pause
		bPaused = true;
		pauseTime = timeGetTime();

	} else {
		// unpause
		DWORD t = timeGetTime();
		deltaTime += t - pauseTime;
		if (time==0) deltaTime = t;
		bPaused = false;
	}
}
*/

void ModelCanvas::ResetView()
{
	model->rot = Vec3D(0,-90.0f,0);
	model->pos = Vec3D(0, 0, 5.0f);

	bool isSkyBox = (model->name.substr(0,3)=="Env");
	if (!isSkyBox) {
		if (model->name.find("SkyBox")<model->name.length())
			isSkyBox = true;
	}

	if (isSkyBox) {
		// for skyboxes, don't zoom out ;)
		model->pos.y = model->pos.z = 0.0f;
	} else {
		model->pos.z = model->rad * 1.6f;
		if (model->pos.z < 3.0f) model->pos.z = 3.0f;
		if (model->pos.z > 64.0f) model->pos.z = 64.0f;
		
		//ofsy = (model->anims[model->currentAnim].boxA.y + model->anims[model->currentAnim].boxB.y) * 0.5f;
		model->pos.y = -model->rad * 0.5f;
		if (model->pos.y > 50) model->pos.y = 50;
		if (model->pos.y < -50) model->pos.y = -50;
	}

//	modelsize = model->rad * 2.0f;
	
	if (model->name.substr(0,4)=="Item") 
		model->rot.y = 0; // items look better facing right by default
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

void ModelCanvas::Zoom(float f, bool rel)
{
	if (!model)
		return;
	if (rel) {
		float cosx = cos(model->rot.x * piover180);
		model->pos.x += cos(model->rot.y * piover180) * cosx * f;
		model->pos.y += sin(model->rot.x * piover180) * sin(model->rot.y * piover180) * f;
		model->pos.z += sin(model->rot.y * piover180) * cosx * f;
	} else {
		model->pos.z -= f;
	}
}

// Check for keyboard input
void ModelCanvas::OnKey(wxKeyEvent &event)
{
	if(!model) 
		return;
	
	int keycode = event.GetKeyCode(); 

		if (keycode == '0')
			animControl->SetAnimSpeed(1.0f);
		else if (keycode == '1')
			animControl->SetAnimSpeed(0.1f);
		else if (keycode == '2')
			animControl->SetAnimSpeed(0.2f);
		else if (keycode == '3')
			animControl->SetAnimSpeed(0.3f);
		else if (keycode == '4')
			animControl->SetAnimSpeed(0.4f);
		else if (keycode == '5')
			animControl->SetAnimSpeed(0.5f);
		else if (keycode == '6')
			animControl->SetAnimSpeed(0.6f);
		else if (keycode == '7')
			animControl->SetAnimSpeed(0.7f);
		else if (keycode == '8')
			animControl->SetAnimSpeed(0.8f);
		else if (keycode == '9')
			animControl->SetAnimSpeed(0.9f);
}

void ModelCanvas::CheckMovement()
{
	// Make sure its the canvas that has focus before continuing
	wxWindow *win = wxWindow::FindFocus();
	if(!win)
		return;

	// Its no longer an opengl canvas window, its now just a standard window.
	// wxWindow *gl = wxDynamicCast(win, wxGLCanvas);
	wxWindow *wintest = wxDynamicCast(win, wxWindow);
	if(!wintest)
		return;

	
	if (wxGetKeyState(WXK_NUMPAD8))	// Move forward
		camera.MoveForward(-0.1f);
	if (wxGetKeyState(WXK_NUMPAD2))	// Move Backwards
		camera.MoveForward(0.1f);
	if (wxGetKeyState(WXK_NUMPAD7))	// Rotate left
		camera.RotateY(1.0f);
	if (wxGetKeyState(WXK_NUMPAD9))	// Rotate right
		camera.RotateY(-1.0f);
	if (wxGetKeyState(WXK_NUMPAD5))	// Reset Camera
		camera.Reset();
	if (wxGetKeyState(WXK_NUMPAD4))	// Straff Left
		camera.Strafe(-0.05f);
	if (wxGetKeyState(WXK_NUMPAD6))	// Straff Right
		camera.Strafe(0.05f);

	// M2 Model only stuff below here
	if (!model || !model->animManager)
		return;

	float speed = 1.0f;

	// Time stuff
	if (model)
		speed = ((timeGetTime() - lastTime) * model->animManager->GetSpeed()) / 7.0f;
	else
		speed = (timeGetTime() - lastTime);

	//lastTime = timeGetTime();

	// Turning
	if (wxGetKeyState(WXK_LEFT)) {
		model->rot.y += speed;

		if (model->rot.y > 360) model->rot.y -= 360;
		if (model->rot.y < 0) model->rot.y += 360;
		
	} else if (wxGetKeyState(WXK_RIGHT)) {
		model->rot.y -= speed;

		if (model->rot.y > 360) model->rot.y -= 360;
		if (model->rot.y < 0) model->rot.y += 360;
	}
	// --

	// Moving forward/backward
	//float speed = 0.0f;
	if (model->animated)
		speed *= (model->anims[model->currentAnim].moveSpeed / 160.0f);
	//else
	//	speed *= 0.05f;

	if (wxGetKeyState(WXK_UP))
		Zoom(speed, true);
	else if (wxGetKeyState(WXK_DOWN))
		Zoom(-speed, true);
	// --
}

// Save the scene state,  currently this is just position/rotation/field of view
void ModelCanvas::SaveSceneState(int id)
{
	if (!model)
		return;

	// bounds check
	if (id > -1 && id < 4) {
		sceneState[id].pos = model->pos;
		sceneState[id].rot = model->rot;
		sceneState[id].fov = video.fov;
	}
}

// Load the scene state, as above
void ModelCanvas::LoadSceneState(int id)
{
	if (!model)
		return;

	// bounds check
	if (id > -1 && id < 4) {
		video.fov =  sceneState[id].fov ;
		model->pos = sceneState[id].pos;
		model->rot = sceneState[id].rot;
	}
}

void ModelCanvas::SetCurrent()
{
#ifdef _WINDOWS
	video.SetCurrent();
#else
//	wxGLCanvas::SetCurrent();
	video.render = true;
#endif
}

void ModelCanvas::SwapBuffers()
{
#ifdef _WINDOWS
	video.SwapBuffers();
#else
	wxGLCanvas::SwapBuffers();
#endif
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

