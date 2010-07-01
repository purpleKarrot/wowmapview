#ifndef MODELCANVAS_H
#define MODELCANVAS_H

#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// wx
#ifdef _WINDOWS
    #include <GL/glew.h>
    #include <GL/wglew.h>
#elif __WXMAC__ // OSX
    #include <GL/glew.h>
#else
    #include <GL/glew.h>
    #include <GL/glxew.h>
#endif
#include "wx/glcanvas.h"


#include "wx/window.h"
#include "wx/treectrl.h"

// stl
#include <string>

// our headers
#include "model.h"
#include "wmo.h"
#include "util.h"
#include "maptile.h"
//#include "viewcontrol.h"
#include "lightcontrol.h"
#include <GL/glew.h>
//#include "CShader.h"
#include "camera.h"
#include "enums.h"


// custom objects
class AnimControl;
class LightControl;

class ModelViewer;
class ModelCanvas;

struct SceneState {
	Vec3D pos;	// Model Position
	Vec3D rot;	// Model Rotation

	float fov;  // OpenGL Field of View
};


struct Attachment {
	Attachment *parent;
	Displayable *model;

	std::vector<Attachment*> children;

	int id;
	int slot;
	float scale;
	Vec3D rot;
	Vec3D pos;

	Attachment(Attachment *parent, Displayable *model, int id, int slot, float scale=1.0f, float rot=0.0f, Vec3D pos=Vec3D(0.0f, 0.0f, 0.0f)): parent(parent), model(model), id(id), slot(slot), scale(scale), rot(rot), pos(pos)
	{}

	~Attachment()
	{
		delChildren();

		parent = NULL;
		wxDELETE(model);
	}
	
	void setup();
	void setupParticle();
	Attachment* addChild(const char *fn, int id, int slot, float scale=1.0f, float rot=0.0f, Vec3D pos=Vec3D(0.0f, 0.0f, 0.0f));
	Attachment* addChild(Displayable *disp, int id, int slot, float scale=1.0f, float rot=0.0f, Vec3D pos=Vec3D(0.0f, 0.0f, 0.0f));
	void delSlot(int slot);
	void delChildren();
	Model* getModelFromSlot(int slot);

	inline void draw(ModelCanvas *c);
	inline void drawParticles(bool force=false);
	void tick(float dt);
};
class qtcanvas;
class ModelCanvas:
#ifdef _WINDOWS
		public wxWindow
#else
		public wxGLCanvas
#endif
{
	DECLARE_CLASS(ModelCanvas)
    DECLARE_EVENT_TABLE()
	
	float time;//, modelsize;
	SceneState sceneState[4]; // 4 scene states for F1-F4

	GLuint fogTex;

	bool fxBlur, fxGlow, fxFog;

	qtcanvas* new_;

public:
	ModelCanvas(wxWindow *parent, VideoCaps *cap = NULL);
    ~ModelCanvas();

	// GUI Control Panels
	AnimControl *animControl;

	CCamera camera;

	// Event Handlers
    void OnPaint(wxPaintEvent& WXUNUSED(event));
    void OnSize(wxSizeEvent& event);
    void OnMouse(wxMouseEvent& event);
	void OnKey(wxKeyEvent &event);

	void OnEraseBackground(wxEraseEvent& event);
    void OnTimer(wxTimerEvent& event);
	void tick();
	wxTimer timer;

	// OGL related functions
	void InitGL();
	void InitView();
	void InitShaders();
	void UninitShaders();
	void ResetView();
	void ResetViewWMO(int id);

	// Main render routines which call the sub routines
	void Render();
	void RenderWMO();
	void RenderADT();
	void RenderLight(Light *l);

	// Render sub routines
	void RenderSkybox();
	void RenderObjects();

	void SaveSceneState(int id);
	void LoadSceneState(int id);

	void SetCurrent();
	void SwapBuffers();

	// view:
	Vec3D vRot0;
	Vec3D vPos0;
	wxCoord mx, my;

	void Zoom(float f, bool rel = false); // f = amount to zoom, rel = relative to model or not
	void CheckMovement();	// move the character
	
	Attachment* LoadModel(const char* fn);
	Attachment* LoadCharModel(const char* fn);
	void LoadWMO(wxString fn);
	void LoadADT(wxString fn);
	
	// Various toggles
	bool init;
	bool initShaders;
	bool drawSky;
	bool useCamera; //, useLights;

	// These are now handled by each individual model.
	bool bMouseLight; // true if the mouse is set to control the light pos, instead of model

	int lightType;	// MODEL / AMBIENCE / DYNAMIC
	int ignoreMouse;

	// Models / Attachments
	Model *model;
	Model *skyModel;
	WMO *wmo;
	MapTile *adt;

	Attachment *root;
	Attachment *sky;
	Attachment *curAtt;

	// Attachment related functions
	void clearAttachments();
	int addAttachment(const char *model, Attachment *parent, int id, int slot);
	void deleteSlot(int slot);

	// Background colour
	Vec3D vecBGColor;
};

#endif
