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
#include <GL/glew.h>
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
#include "enums.h"


// custom objects
class AnimControl;

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

    void OnTimer(wxTimerEvent& event);
	wxTimer timer;

	// OGL related functions
	void InitGL();
	void InitView();

	void InitShaders();
	void UninitShaders();

	void ResetViewWMO(int id);

	// Main render routines which call the sub routines
	void RenderWMO();
	void RenderADT();

	// Render sub routines
	void RenderObjects();

	void SetCurrent() // deactivate parent functionality!
	{
	}

	Attachment* LoadModel(const char* fn);
	Attachment* LoadCharModel(const char* fn);
	void LoadWMO(wxString fn);
	void LoadADT(wxString fn);
	
	// Various toggles
	bool init;
	bool initShaders;
	bool useCamera;

	// Models / Attachments
	Model *model;
	WMO *wmo;
	MapTile *adt;

	Attachment *root;
	Attachment *curAtt;

	// Attachment related functions
	void clearAttachments();
	int addAttachment(const char *model, Attachment *parent, int id, int slot);
	void deleteSlot(int slot);
};

#endif
