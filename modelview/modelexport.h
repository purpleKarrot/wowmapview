#ifndef MODELEXPORT_H
#define MODELEXPORT_H

#include "model.h"
#include "modelcanvas.h"

#define RADIAN 57.295779513082320876798154814114

// Structures
struct Vertex3f {
	float x;
	float y;
	float z;
};

struct ModelData {
	Vertex3f vertex;
	float tu;
	float tv;
	Vertex3f normal;
	unsigned short groupIndex;
	char boneid;
	char boneidEx[3];
	char weight[4];
};

struct GroupData {
	ModelRenderPass p;
	Model *m;
};

// Common functions
void LogExportData(wxString FileExtension, wxString Directory, wxString ExportType);
void SaveTexture(wxString fn);
void SaveTexture2(wxString file, wxString outdir, wxString ExportID, wxString suffix);
Vec3D QuaternionToXYZ(Vec3D Dir, float W);
void InitCommon(Attachment *att, bool init, ModelData *&verts, GroupData *&groups, unsigned short &numVerts, unsigned short &numGroups, unsigned short &numFaces);
wxString GetM2TextureName(Model *m, const char *fn, ModelRenderPass p, int PassNumber);

// --== Exporter Functions ==--
// If your exporter doesn't do WMO or M2 files, include a faux function anyways. Then add a non-working
// function at the bottom of modelexport.cpp so it has something to look for. The non-working exporter
// can be disabled in filecontrol.cpp, so it won't ever run.

// Raw Model File
void SaveBaseFile();

// Lightwave
void ExportM2toLWO(Attachment *att, Model *m, const char *fn, bool init);
void ExportWMOtoLWO(WMO *m, const char *fn);
void ExportWMOObjectstoLWO(WMO *m, const char *fn);
void ExportADTtoLWO(MapTile *m, const char *fn);
#ifdef _DEBUG
	void ExportWMOtoLWO2(WMO *m, const char *fn);
#endif

// Wavefront Object
void ExportM2toOBJ(Attachment *att, Model *m, const char *fn, bool init);
void ExportWMOtoOBJ(WMO *m, const char *fn);

// Milkshape
void ExportM2toMS3D(Attachment *att, Model *m, const char *fn, bool init);
void ExportWMOtoMS3D(WMO *m, const char *fn);

// Collada
void ExportM2toCOLLADA(Attachment *att, Model *m, const char *fn, bool init);
void ExportWMOtoCOLLADA(WMO *m, const char *fn);

//3D Studio Max
void ExportM2to3DS(Attachment *att, Model *m, const char *fn, bool init);
void ExportWMOto3DS(WMO *m, const char *fn);

// X3D/XHTML
void ExportM2toX3D(Model *m, const char *fn, bool init);
void ExportM2toXHTML(Model *m, const char *fn, bool init);
void ExportWMOtoX3D(WMO *m, const char *fn);
void ExportWMOtoXHTML(WMO *m, const char *fn);

// Ogre XML
void ExportM2toOgreXml(Model *m, const char *fn, bool init);
void ExportWMOtoOgreXml(WMO *m, const char *fn);


#endif
