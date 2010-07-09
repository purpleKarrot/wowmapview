#ifndef MODEL_H
#define MODEL_H

#include "vec3d.h"
#include "texture.hpp"

class Model;
class Bone;
Vec3D fixCoordSystem(Vec3D v);

#include "mpq.hpp"

#include "modelheaders.h"
#include "quaternion.h"
#include "matrix.h"

#include <vector>

#include "animated.h"
#include "particle.h"


class Bone {
	Animated<Vec3D> trans;
	//Animated<Quaternion> rot;
	Animated<Quaternion, PACK_QUATERNION, Quat16ToQuat32> rot;
	Animated<Vec3D> scale;

public:
	int parent;
	Vec3D pivot, transPivot;
	bool billboard;
	Matrix mat;
	Matrix mrot;

	bool calc;
	void calcMatrix(Bone* allbones, int anim, int time);
	void init(MPQFile &f, ModelBoneDef &b, int *global, MPQFile *animfiles);

};


class TextureAnim {
	Animated<Vec3D> trans, rot, scale;

public:
	Vec3D tval, rval, sval;

	void calc(int anim, int time);
	void init(MPQFile &f, ModelTexAnimDef &mta, int *global);
	void setup(int anim);
};

struct ModelColor {
	Animated<Vec3D> color;
	AnimatedShort opacity;

	void init(MPQFile &f, ModelColorDef &mcd, int *global);
};

struct ModelTransparency {
	AnimatedShort trans;

	void init(MPQFile &f, ModelTransDef &mtd, int *global);
};

// copied from the .mdl docs? this might be completely wrong
enum BlendModes {
	BM_OPAQUE,
	BM_TRANSPARENT,
	BM_ALPHA_BLEND,
	BM_ADDITIVE,
	BM_ADDITIVE_ALPHA,
	BM_MODULATE,
	BM_MODULATE2
};

struct ModelRenderPass {
	uint16_t indexStart, indexCount, vertexStart, vertexEnd;
	//TextureID texture, texture2;
	int tex;
	bool usetex2, useEnvMap, cull, trans, unlit, noZWrite, billboard;
	float p;
	
	int16_t texanim, color, opacity, blendmode;
	int16_t order;

	// Geoset ID
	int geoset;

	// texture wrapping
	bool swrap, twrap;

	// colours
	Vec4D ocol, ecol;

	bool init(Model const& m);
	void deinit();

	bool operator< (const ModelRenderPass &m) const
	{
		//return !trans;
		if (order<m.order) return true;
		else if (order>m.order) return false;
		else return blendmode == m.blendmode ? (p<m.p) : blendmode < m.blendmode;
	}
};

struct ModelLight {
	int type, parent;
	Vec3D pos, tpos, dir, tdir;
	Animated<Vec3D> diffColor, ambColor;
	Animated<float> diffIntensity, ambIntensity;

	void init(MPQFile &f, ModelLightDef &mld, int *global);
	void setup(int time, GLuint l);
};

class Model {

	GLuint dlist;
	GLuint vbuf, nbuf, tbuf;
	size_t vbufsize;
	bool animated;
	bool animGeometry,animTextures,animBones;

	bool forceAnim;
	MPQFile *animfiles;

	void init(MPQFile &f);

	ModelHeader header;
	TextureAnim *texAnims;
	ModelAnimation *anims;
	int *globalSequences;
	ModelColor *colors;
	ModelTransparency *transparency;
	ModelLight *lights;
	ParticleSystem *particleSystems;
	RibbonEmitter *ribbons;

	void drawModel()const;
	void initCommon(MPQFile &f);
	bool isAnimated(MPQFile &f);
	void initAnimated(MPQFile &f);
	void initStatic(MPQFile &f);

	ModelVertex *origVertices;
	mutable Vec3D *vertices, *normals;
	uint16_t *indices;
	size_t nIndices;
	mutable std::vector<ModelRenderPass> passes;

	void animate(int anim)const;
	void calcBones(int anim, int time)const;

	void lightsOn(GLuint lbase)const;
	void lightsOff(GLuint lbase)const;

public:
	Bone *bones;

	// ===============================
	// Toggles
	bool *showGeosets;

	// ===============================
	// Texture data
	// ===============================
	std::vector<wow::texture> textures;
	int specialTextures[TEXTURE_MAX];
	wow::texture replaceTextures[TEXTURE_MAX];
	bool useReplaceTextures[TEXTURE_MAX];


	bool ok;
	bool ind;

	float rad;
	float trans;
	mutable bool animcalc;
	mutable	int anim, animtime;
	std::string fullname;

	Model(std::string name, bool forceAnim=false);
	~Model();
	void draw() const;
	void updateEmitters(float dt)const;

	friend struct ModelRenderPass;
};

namespace wow
{

typedef boost::flyweights::flyweight< //
	boost::flyweights::key_value<std::string, Model> > Model;

} // namespace wow

class ModelInstance {
public:

	void resetAnim();
	void updateEmitters(float dt);

	wow::Model model;

	int id;

	Vec3D pos, dir;
	unsigned int scale;

	float frot,w,sc;

	int light;
	Vec3D ldir;
	Vec4D lcol;

	ModelInstance() {}
	ModelInstance(const wow::Model& m, MPQFile &f);
    void init2(const wow::Model& m, MPQFile &f);
	void draw();
	void draw2(const Vec3D& ofs, const float rot);

};

#endif
