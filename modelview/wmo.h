#ifndef WMO_H
#define WMO_H

// STL
#include <vector>
#include <set>
#include <string>

// Our headers
#include "manager.h"
#include "vec3d.h"
#include "mpq.h"
#include "model.h"
#include "video.h"
#include "displayable.h"

class WMO;
class WMOGroup;
class WMOInstance;
class WMOManager;
//class Liquid;

struct WMOBatch {
	signed char bytes[12];
	unsigned int indexStart;
	unsigned short indexCount, vertexStart, vertexEnd;
	unsigned char flags, texture;
};

struct WMOVertColor{
	uint8 b, g, r, a;
};

class WMOGroup {
	WMO *wmo;
	int flags;
	GLuint dl,dl_light;
	Vec3D center;
	float rad;
	int num;
	int fog;
	int nDoodads;
	short *ddr;
	//Liquid *lq;
public:
	Vec3D *vertices, *normals;
	Vec2D *texcoords;
	unsigned short *indices;
	int nTriangles, nVertices, nIndices;
	unsigned int *cv;
	unsigned short *materials;
	WMOBatch *batches;
	int nBatches;
	WMOVertColor *VertexColors;
	uint32 *IndiceToVerts;

	Vec3D v1,v2;
	Vec3D b1,b2;
	Vec3D vmin, vmax;
	bool indoor, hascv;
	bool visible;
	bool ok;

	bool outdoorLights;
	std::string name, desc;

	WMOGroup() : dl(0), ddr(0), vertices(NULL), normals(NULL), texcoords(NULL), indices(NULL), materials(NULL), batches(NULL) {}
	~WMOGroup();
	void init(WMO *wmo, MPQFile &f, int num, char *names);
	void initDisplayList();
	void initLighting(int nLR, short *useLights);
	void draw();
	void drawLiquid();
	void drawDoodads(int doodadset);
	void setupFog();
	void cleanup();

	void updateModels(bool load);
};

#define	WMO_MATERIAL_CULL	0x04
struct WMOMaterial {
	int flags;
	int SpecularMode;
	int transparent; // Blending: 0 for opaque, 1 for transparent
	int nameStart; // Start position for the first texture filename in the MOTX data block
	unsigned int color1;
	unsigned int flag1;
	int nameEnd; // Start position for the second texture filename in the MOTX data block
	unsigned int color2;
	unsigned int flag2;
	float f1,f2;
	int dx[5];
	// read up to here -_-
	TextureID tex;
};

enum LightType 
{
	OMNI_LGT,
	SPOT_LGT,
	DIRECT_LGT,
	AMBIENT_LGT
};
struct WMOLight {
	unsigned int lighttype, type, useatten, color;
	Vec3D pos;
	float intensity;
	float attenStart, attenEnd;
	float unk[3];
	float r;

	Vec4D fcolor;

	void init(MPQFile &f);
	void setup(GLint light);

	static void setupOnce(GLint light, Vec3D dir, Vec3D lcol);
};

struct WMOPV {
	Vec3D a,b,c,d;
};

struct WMOPR {
	short portal; // Portal index
	short group; // WMO group index
	short dir; // 1 or -1
	short reserved; // always 0
};

struct WMOVB {
	unsigned short firstVertex;
	unsigned short count;
};

struct WMODoodadSet {
	char name[0x14]; // set name
	int start; // index of first doodad instance in this set
	int size; // number of doodad instances in this set
	int unused; // unused? (always 0)
};

struct WMOLiquidHeader {
	int X, Y, A, B;
	Vec3D pos;
	short type;
};

struct WMOFog {
	unsigned int flags;
	Vec3D pos;
	float r1; // Smaller radius
	float r2; // Larger radius
	float fogend; // This is the distance at which all visibility ceases, and you see no objects or terrain except for the fog color.
	float fogstart; // This is where the fog starts. Obtained by multiplying the fog end value by the fog start multiplier. multiplier (0..1)
	unsigned int color1; // The back buffer is also cleared to this colour 
	float f2; // Unknown (almost always 222.222)
	float f3; // Unknown (-1 or -0.5)
	unsigned int color2;
	// read to here (0x30 bytes)
	Vec4D color;
	void init(MPQFile &f);
	void setup();
};

class WMOModelInstance {
public:
	// header
	Vec3D pos;
	float w;
	Vec3D dir;
	float sc;
	unsigned int d1;
	
	Model *model;
	std::string filename;
	int id;
	unsigned int scale;
	float frot;
	int light;
	Vec3D ldir;
	Vec3D lcol;

	WMOModelInstance() {}
    void init(char *fname, MPQFile &f);
	void draw();

	void loadModel(ModelManager &mm);
	void unloadModel(ModelManager &mm);
};

struct WMOHeader {
	int nTextures; // number of materials
	int nGroups; // number of WMO groups
	int nP; // number of portals
	int nLights; // number of lights
	int nModels; // number of M2 models imported
	int nDoodads; // number of dedicated files
	int nDoodadSets; // number of doodad sets
	unsigned int col; // ambient color? RGB
	int nX; // WMO ID (column 2 in WMOAreaTable.dbc)
	Vec3D v1; // Bounding box corner 1
	Vec3D v2; // Bounding box corner 2
	int LiquidType;
};

class WMO: public ManagedItem, public Displayable {
public:
	//WMOHeader header;
	int nTextures; // number of materials
	int nGroups; // number of WMO groups
	int nP; // number of portals
	int nLights; // number of lights
	int nModels; // number of M2 models imported
	int nDoodads; // number of dedicated files
	int nDoodadSets; // number of doodad sets
	unsigned int col; // ambient color? RGB
	int nX; // WMO ID (column 2 in WMOAreaTable.dbc)
	Vec3D v1; // Bounding box corner 1
	Vec3D v2; // Bounding box corner 2
	int LiquidType;

	WMOGroup *groups;
	WMOMaterial *mat;
	bool ok;
	char *groupnames;
	std::vector<std::string> textures;
	std::vector<std::string> models;
	std::vector<WMOModelInstance> modelis;
	ModelManager loadedModels;

	Vec3D viewpos;
	Vec3D viewrot;

	std::vector<WMOLight> lights;
	std::vector<WMOPV> pvs;
	std::vector<WMOPR> prs;

	std::vector<WMOFog> fogs;

	std::vector<WMODoodadSet> doodadsets;

	Model *skybox;
	int sbid;

	WMO(std::string name);
	~WMO();
	
	int doodadset;
	bool includeDefaultDoodads;
	
	void draw();
	void drawSkybox();
	void drawPortals();
	
	void update(int dt);

	void loadGroup(int id);
	void showDoodadSet(int id);
	void updateModels();
};

/*
class WMOManager: public SimpleManager {
public:
	int add(std::string name);
};


class WMOInstance {
	static std::set<int> ids;
public:
	WMO *wmo;
	Vec3D pos;
	Vec3D pos2, pos3, dir;
	int id, d2, d3;
	int doodadset;

	WMOInstance(WMO *wmo, MPQFile &f);
	void draw();
	//void drawPortals();

	static void reset();
};
*/


#endif
