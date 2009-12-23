#include "model.h"
#include "world.h"
#include <cassert>
#include <algorithm>

int globalTime = 0;

Model::Model(std::string name, bool forceAnim) : ManagedItem(name), forceAnim(forceAnim)
{
	// replace .MDX with .M2
	char tempname[256];
	strncpy(tempname,name.c_str(), sizeof(tempname));
	if (tempname[name.length()-1] != '2') {
		tempname[name.length()-2] = '2';
		tempname[name.length()-1] = 0;
	}

	MPQFile f(tempname);
	ok = !f.isEof();

	if (!ok) {
		gLog("Error: loading model [%s]\n", tempname);
		return;
	}
	fullname = tempname;

	memcpy(&header, f.getBuffer(), sizeof(ModelHeader));

	// HACK: these particle systems are way too active and cause horrible horrible slowdowns
	// I'm removing them until I can fix emission speed so it doesn't get this crazy
	if (false
	|| name=="World\\Kalimdor\\Orgrimmar\\Passivedoodads\\Orgrimmarbonfire\\Orgrimmarsmokeemitter.Mdx"	
	//|| name=="World\\Kalimdor\\Orgrimmar\\Passivedoodads\\Orgrimmarbonfire\\Orgrimmarbonfire01.Mdx"	
	) {
        header.nParticleEmitters = 0;
	}

	animated = isAnimated(f) || forceAnim;  // isAnimated will set animGeometry and animTextures

	gLog("Loading model %s%s\n", tempname, animated ? " (animated)" : "");

	// Initiate our model variables.
	trans = 1.0f;

	for (int i=0; i<TEXTURE_MAX; i++) {
		specialTextures[i] = -1;
		replaceTextures[i] = 0;
		useReplaceTextures[i] = false;
	}

	showGeosets = 0;
	vbuf = nbuf = tbuf = 0;

	globalSequences = 0;
	animtime = 0;
	anim = 0;
	colors = 0;
	lights = 0;
	transparency = 0;
	particleSystems = 0;
	ribbons = 0;
	if (header.nGlobalSequences) {
		globalSequences = new int[header.nGlobalSequences];
		memcpy(globalSequences, (f.getBuffer() + header.ofsGlobalSequences), header.nGlobalSequences * 4);
	}

	if (animated) 
		initAnimated(f);
	else 
		initStatic(f);

	f.close();
}

Model::~Model()
{
	if (ok) {
		//gLog("Unloading model %s\n", name.c_str());

		if (header.nTextures) {
			for (size_t i=0; i<header.nTextures; i++) {
				if (textures[i]!=0) {
					//Texture *tex = (Texture*)video.textures.items[textures[i]];
					video.textures.del(textures[i]);
				}
			}
			delete[] textures;
		}

		delete[] globalSequences;
		delete[] showGeosets;

		if (animated) {
			// unload all sorts of crap
			//delete[] vertices;
			//delete[] normals;
			delete[] indices;
			delete[] anims;
			delete[] origVertices;
			if (animBones) delete[] bones;
			if (!animGeometry) {
				glDeleteBuffersARB(1, &nbuf);
			}
			glDeleteBuffersARB(1, &vbuf);
			glDeleteBuffersARB(1, &tbuf);

			if (animTextures) delete[] texAnims;
			if (colors) delete[] colors;
			if (transparency) delete[] transparency;
			if (lights) delete[] lights;

			if (particleSystems) delete[] particleSystems;
			if (ribbons) delete[] ribbons;

		} else {
			glDeleteLists(dlist, 1);
		}
	}
}


bool Model::isAnimated(MPQFile &f)
{
	// see if we have any animated bones
	ModelBoneDef *bo = (ModelBoneDef*)(f.getBuffer() + header.ofsBones);

	animGeometry = false;
	animBones = false;
	ind = false;

	ModelVertex *verts = (ModelVertex*)(f.getBuffer() + header.ofsVertices);
	for (size_t i=0; i<header.nVertices && !animGeometry; i++) {
		for (size_t b=0; b<4; b++) {
			if (verts[i].weights[b]>0) {
				ModelBoneDef &bb = bo[verts[i].bones[b]];
				if (bb.translation.type || bb.rotation.type || bb.scaling.type || (bb.flags&8)) {
					if (bb.flags&8) {
						// if we have billboarding, the model will need per-instance animation
						ind = true;
					}
					animGeometry = true;
					break;
				}
			}
		}
	}

	if (animGeometry) animBones = true;
	else {
		for (size_t i=0; i<header.nBones; i++) {
			ModelBoneDef &bb = bo[i];
			if (bb.translation.type || bb.rotation.type || bb.scaling.type) {
				animBones = true;
				break;
			}
		}
	}

	animTextures = header.nTexAnims > 0;

	bool animMisc = header.nCameras>0 || // why waste time, pretty much all models with cameras need animation
					header.nLights>0 || // same here
					header.nParticleEmitters>0 ||
					header.nRibbonEmitters>0;

	if (animMisc) animBones = true;

	// animated colors
	if (header.nColors) {
		ModelColorDef *cols = (ModelColorDef*)(f.getBuffer() + header.ofsColors);
		for (size_t i=0; i<header.nColors; i++) {
			if (cols[i].color.type!=0 || cols[i].opacity.type!=0) {
				animMisc = true;
				break;
			}
		}
	}

	// animated opacity
	if (header.nTransparency && !animMisc) {
		ModelTransDef *trs = (ModelTransDef*)(f.getBuffer() + header.ofsTransparency);
		for (size_t i=0; i<header.nTransparency; i++) {
			if (trs[i].trans.type!=0) {
				animMisc = true;
				break;
			}
		}
	}

	// guess not...
	return animGeometry || animTextures || animMisc;
}


Vec3D fixCoordSystem(Vec3D v)
{
	return Vec3D(v.x, v.z, -v.y);
}

Vec3D fixCoordSystem2(Vec3D v)
{
	return Vec3D(v.x, v.z, v.y);
}

Quaternion fixCoordSystemQuat(Quaternion v)
{
	return Quaternion(-v.x, -v.z, v.y, v.w);
}


void Model::initCommon(MPQFile &f)
{
	// assume: origVertices already set
	if (!animGeometry) {
		vertices = new Vec3D[header.nVertices];
		normals = new Vec3D[header.nVertices];
	}

	//Vec3D vmin = Vec3D( 9999999.0f, 9999999.0f, 9999999.0f);
	//Vec3D vmax = Vec3D(-9999999.0f,-9999999.0f,-9999999.0f);
	// vertices, normals
	for (size_t i=0; i<header.nVertices; i++) {
		origVertices[i].pos = fixCoordSystem(origVertices[i].pos);
		origVertices[i].normal = fixCoordSystem(origVertices[i].normal);

		if (!animGeometry) {
			vertices[i] = origVertices[i].pos;
			normals[i] = origVertices[i].normal.normalize();
		}

		float len = origVertices[i].pos.lengthSquared();
		if (len > rad){ 
			rad = len;
		}
		/*
		if (origVertices[i].pos.x < vmin.x) vmin.x = origVertices[i].pos.x;
		if (origVertices[i].pos.y < vmin.y) vmin.y = origVertices[i].pos.y;
		if (origVertices[i].pos.z < vmin.z) vmin.z = origVertices[i].pos.z;
		if (origVertices[i].pos.x > vmax.x) vmax.x = origVertices[i].pos.x;
		if (origVertices[i].pos.y > vmax.y) vmax.y = origVertices[i].pos.y;
		if (origVertices[i].pos.z > vmax.z) vmax.z = origVertices[i].pos.z;
		*/
	}
	rad = sqrtf(rad);
	//rad = std::max(vmin.length(),vmax.length());

	// textures
	ModelTextureDef *texdef = (ModelTextureDef*)(f.getBuffer() + header.ofsTextures);
	if (header.nTextures) {
		textures = new TextureID[header.nTextures];
		char texname[256];
		for (size_t i=0; i<header.nTextures; i++) {
			// Error check
			if (i > TEXTURE_MAX-1) {
				gLog("Error: Model Texture %d over %d", header.nTextures, TEXTURE_MAX);
				break;
			}

			if (texdef[i].type == 0) {
				strncpy(texname, (const char*)(f.getBuffer() + texdef[i].nameOfs), texdef[i].nameLen);
				texname[texdef[i].nameLen] = 0;
				textures[i] = video.textures.add(texname);
			} else {
				// special texture - only on characters and such...
                textures[i] = 0;
				specialTextures[i] = texdef[i].type;

				if (texdef[i].type < TEXTURE_MAX)
					useReplaceTextures[texdef[i].type] = true;

				if (texdef[i].type == 3) {
					// a fix for weapons with type-3 textures.
					replaceTextures[texdef[i].type] = video.textures.add("Item\\ObjectComponents\\Weapon\\ArmorReflect4.BLP");
				}
			}
		}
	}

	// init colors
	if (header.nColors) {
		colors = new ModelColor[header.nColors];
		ModelColorDef *colorDefs = (ModelColorDef*)(f.getBuffer() + header.ofsColors);
		for (size_t i=0; i<header.nColors; i++) 
			colors[i].init(f, colorDefs[i], globalSequences);
	}
	// init transparency
	int16 *transLookup = (int16*)(f.getBuffer() + header.ofsTransparencyLookup);
	if (header.nTransparency) {
		transparency = new ModelTransparency[header.nTransparency];
		ModelTransDef *trDefs = (ModelTransDef*)(f.getBuffer() + header.ofsTransparency);
		for (size_t i=0; i<header.nTransparency; i++) 
			transparency[i].init(f, trDefs[i], globalSequences);
	}

	// just use the first LOD/view

	if (header.nViews > 0) {
	// indices - allocate space, too
		std::string lodname = fullname.substr(0, fullname.length()-3);
		fullname = lodname;
		lodname.append("00.skin");
		MPQFile g(lodname.c_str());
		if (g.isEof()) {
			gLog("Error: loading lod [%s]\n", lodname.c_str());
			g.close();
			return;
		}
		ModelView *view = (ModelView*)(g.getBuffer());

		uint16 *indexLookup = (uint16*)(g.getBuffer() + view->ofsIndex);
		uint16 *triangles = (uint16*)(g.getBuffer() + view->ofsTris);
		nIndices = view->nTris;
		indices = new uint16[nIndices];
		for (size_t i = 0; i<nIndices; i++) {
			indices[i] = indexLookup[triangles[i]];
		}

		// render ops
		ModelGeoset *ops = (ModelGeoset*)(g.getBuffer() + view->ofsSub);
		ModelTexUnit *tex = (ModelTexUnit*)(g.getBuffer() + view->ofsTex);
		ModelRenderFlags *renderFlags = (ModelRenderFlags*)(f.getBuffer() + header.ofsTexFlags);
		uint16 *texlookup = (uint16*)(f.getBuffer() + header.ofsTexLookup);
		uint16 *texanimlookup = (uint16*)(f.getBuffer() + header.ofsTexAnimLookup);
		int16 *texunitlookup = (int16*)(f.getBuffer() + header.ofsTexUnitLookup);

		showGeosets = new bool[view->nSub];
		for (size_t i=0; i<view->nSub; i++) {
			showGeosets[i] = true;
		}

		for (size_t j = 0; j<view->nTex; j++) {
			ModelRenderPass pass;

			pass.usetex2 = false;
			pass.useEnvMap = false;
			pass.cull = false;
			pass.trans = false;
			pass.unlit = false;
			pass.noZWrite = false;
			pass.billboard = false;

			size_t geoset = tex[j].op;

			pass.geoset = (int)geoset;

			pass.indexStart = ops[geoset].istart;
			pass.indexCount = ops[geoset].icount;
			pass.vertexStart = ops[geoset].vstart;
			pass.vertexEnd = pass.vertexStart + ops[geoset].vcount;

			pass.order = tex[j].shading;

			//TextureID texid = textures[texlookup[tex[j].textureid]];
			//pass.texture = texid;
			pass.tex = texlookup[tex[j].textureid];
			
			// TODO: figure out these flags properly -_-
			ModelRenderFlags &rf = renderFlags[tex[j].flagsIndex];
			

			pass.blendmode = rf.blend;
			pass.color = tex[j].colorIndex;
			pass.opacity = transLookup[tex[j].transid];

			pass.unlit = (rf.flags & RENDERFLAGS_UNLIT)!=0;
			pass.cull = (rf.flags & RENDERFLAGS_TWOSIDED)==0 && rf.blend==0;

			pass.billboard = (rf.flags & RENDERFLAGS_BILLBOARD) != 0;

			pass.useEnvMap = (texunitlookup[tex[j].texunit] == -1) && pass.billboard && rf.blend>2;
			pass.noZWrite = (rf.flags & RENDERFLAGS_ZBUFFERED) != 0;

			// ToDo: Work out the correct way to get the true/false of transparency
			pass.trans = (pass.blendmode>0) && (pass.opacity>0);	// Transparency - not the correct way to get transparency

			pass.p = ops[geoset].BoundingBox[0].x;

			// Texture flags
			pass.swrap = (texdef[pass.tex].flags & TEXTURE_WRAPX) != 0; // Texture wrap X
			pass.twrap = (texdef[pass.tex].flags & TEXTURE_WRAPY) != 0; // Texture wrap Y

			if (animTextures) {
				if (tex[j].flags & TEXTUREUNIT_STATIC) {
					pass.texanim = -1; // no texture animation
				} else {
					pass.texanim = texanimlookup[tex[j].texanimid];
				}
			} else {
				pass.texanim = -1; // no texture animation
			}

       		passes.push_back(pass);
		}
		g.close();
		// transparent parts come later
		std::sort(passes.begin(), passes.end());
	}

	// zomg done
}

void Model::initStatic(MPQFile &f)
{
	origVertices = (ModelVertex*)(f.getBuffer() + header.ofsVertices);

	initCommon(f);

	dlist = glGenLists(1);
	glNewList(dlist, GL_COMPILE);

    drawModel();

	glEndList();

	// clean up vertices, indices etc
	delete[] vertices;
	delete[] normals;
	delete[] indices;

	if (colors) delete[] colors;
	if (transparency) delete[] transparency;
}

void Model::initAnimated(MPQFile &f)
{
	origVertices = new ModelVertex[header.nVertices];
	memcpy(origVertices, f.getBuffer() + header.ofsVertices, header.nVertices * sizeof(ModelVertex));

	glGenBuffersARB(1,&vbuf);
	glGenBuffersARB(1,&tbuf);
	const size_t size = header.nVertices * sizeof(float);
	vbufsize = 3 * size;

	initCommon(f);

	if (header.nAnimations > 0) {
		anims = new ModelAnimation[header.nAnimations];
		memcpy(anims, f.getBuffer() + header.ofsAnimations, header.nAnimations * sizeof(ModelAnimation));

		animfiles = new MPQFile[header.nAnimations];
		char tempname[256];
		for(size_t i=0; i<header.nAnimations; i++) {
			sprintf(tempname, "%s%04d-%02d.anim", fullname.c_str(), anims[i].animID, anims[i].subAnimID);
			if (MPQFile::getSize(tempname) > 0) {
				animfiles[i].openFile(tempname);
			}
		}
	}

	if (animBones) {
		// init bones...
		bones = new Bone[header.nBones];
		ModelBoneDef *mb = (ModelBoneDef*)(f.getBuffer() + header.ofsBones);
		for (size_t i=0; i<header.nBones; i++) {
			bones[i].init(f, mb[i], globalSequences, animfiles);
		}
	}

	if (!animGeometry) {
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbuf);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, vbufsize, vertices, GL_STATIC_DRAW_ARB);
		glGenBuffersARB(1,&nbuf);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, nbuf);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, vbufsize, normals, GL_STATIC_DRAW_ARB);
		delete[] vertices;
		delete[] normals;
	}
	Vec2D *texcoords = new Vec2D[header.nVertices];
	for (size_t i=0; i<header.nVertices; i++) 
		texcoords[i] = origVertices[i].texcoords;
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, tbuf);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, 2*size, texcoords, GL_STATIC_DRAW_ARB);
	delete[] texcoords;

	if (animTextures) {
		texAnims = new TextureAnim[header.nTexAnims];
		ModelTexAnimDef *ta = (ModelTexAnimDef*)(f.getBuffer() + header.ofsTexAnims);
		for (size_t i=0; i<header.nTexAnims; i++) {
			texAnims[i].init(f, ta[i], globalSequences);
		}
	}

	// particle systems
	if (header.nParticleEmitters) {
		ModelParticleEmitterDef *pdefs = (ModelParticleEmitterDef *)(f.getBuffer() + header.ofsParticleEmitters);
		particleSystems = new ParticleSystem[header.nParticleEmitters];
		for (size_t i=0; i<header.nParticleEmitters; i++) {
			particleSystems[i].model = this;
			particleSystems[i].init(f, pdefs[i], globalSequences);
		}
	}

	// ribbons
	if (header.nRibbonEmitters) {
		ModelRibbonEmitterDef *rdefs = (ModelRibbonEmitterDef *)(f.getBuffer() + header.ofsRibbonEmitters);
		ribbons = new RibbonEmitter[header.nRibbonEmitters];
		for (size_t i=0; i<header.nRibbonEmitters; i++) {
			ribbons[i].model = this;
			ribbons[i].init(f, rdefs[i], globalSequences);
		}
	}

	// just use the first camera, meh
	if (header.nCameras>0) {
		ModelCameraDef *camDefs = (ModelCameraDef*)(f.getBuffer() + header.ofsCameras);
		cam.init(f, camDefs[0], globalSequences);
	}

	// init lights
	if (header.nLights) {
		lights = new ModelLight[header.nLights];
		ModelLightDef *lDefs = (ModelLightDef*)(f.getBuffer() + header.ofsLights);
		for (size_t i=0; i<header.nLights; i++) 
			lights[i].init(f, lDefs[i], globalSequences);
	}

	animcalc = false;
}


void Model::calcBones(int anim, int time)
{
	for (size_t i=0; i<header.nBones; i++) {
		bones[i].calc = false;
	}

	for (size_t i=0; i<header.nBones; i++) {
		bones[i].calcMatrix(bones, anim, time);
	}
}

void Model::animate(int anim)
{
	ModelAnimation &a = anims[anim];
	int t = globalTime; //(int)(gWorld->animtime /* / a.playSpeed*/);
	int tmax = a.length;
	t %= tmax;
	animtime = t;
	this->anim = anim;

	if (animBones) {
		calcBones(anim, t);
	}

	if (animGeometry) {

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbuf);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, 2*vbufsize, NULL, GL_STREAM_DRAW_ARB);
		vertices = (Vec3D*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY);

		// transform vertices
		ModelVertex *ov = origVertices;
		for (size_t i=0,k=0; i<header.nVertices; ++i,++ov) {
			Vec3D v(0,0,0), n(0,0,0);

			for (size_t b=0; b<4; b++) {
				if (ov->weights[b]>0) {
					Vec3D tv = bones[ov->bones[b]].mat * ov->pos;
					Vec3D tn = bones[ov->bones[b]].mrot * ov->normal;
					v += tv * ((float)ov->weights[b] / 255.0f);
					n += tn * ((float)ov->weights[b] / 255.0f);
				}
			}

			/*
			vertices[i] = v;
			normals[i] = n;
			*/
			vertices[i] = v;
			vertices[header.nVertices + i] = n.normalize(); // shouldn't these be normal by default?
		}

        glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);

	}

	for (size_t i=0; i<header.nLights; i++) {
		if (lights[i].parent>=0) {
			lights[i].tpos = bones[lights[i].parent].mat * lights[i].pos;
			lights[i].tdir = bones[lights[i].parent].mrot * lights[i].dir;
		}
	}

	for (size_t i=0; i<header.nParticleEmitters; i++) {
		// random time distribution for teh win ..?
		int pt = (t + (int)(tmax*particleSystems[i].tofs)) % tmax;
		particleSystems[i].setup(anim, pt);
	}

	for (size_t i=0; i<header.nRibbonEmitters; i++) {
		ribbons[i].setup(anim, t);
	}

	if (animTextures) {
		for (size_t i=0; i<header.nTexAnims; i++) {
			texAnims[i].calc(anim, t);
		}
	}
}


bool ModelRenderPass::init(Model *m)
{
	// May aswell check that we're going to render the geoset before doing all this crap.
	if (m->showGeosets[geoset]) {

		// COLOUR
		// Get the colour and transparency and check that we should even render
		ocol = Vec4D(1.0f, 1.0f, 1.0f, m->trans);
		ecol = Vec4D(0.0f, 0.0f, 0.0f, 0.0f);

		//if (m->trans == 1.0f)
		//	return false;

		// emissive colors
		if (color!=-1 && m->colors[color].color.uses(0)) {
			Vec3D c = m->colors[color].color.getValue(0,m->animtime);
			if (m->colors[color].opacity.uses(m->anim)) {
				float o = m->colors[color].opacity.getValue(m->anim,m->animtime);
				ocol.w = o;
			}

			if (unlit) {
				ocol.x = c.x; ocol.y = c.y; ocol.z = c.z;
			} else {
				ocol.x = ocol.y = ocol.z = 0;
			}

			ecol = Vec4D(c, ocol.w);
			glMaterialfv(GL_FRONT, GL_EMISSION, ecol);
		}

		// opacity
		if (opacity!=-1) {
			if (m->transparency[opacity].trans.uses(0))
				ocol.w *= m->transparency[opacity].trans.getValue(0, m->animtime);
		}

		// exit and return false before affecting the opengl render state
		if (!((ocol.w > 0) && (color==-1 || ecol.w > 0)))
			return false;

		// TEXTURE
		// bind to our texture
		GLuint bindtex = 0;
		if (m->specialTextures[tex]==-1) 
			bindtex = m->textures[tex];
		else 
			bindtex = m->replaceTextures[m->specialTextures[tex]];
		
		glBindTexture(GL_TEXTURE_2D, bindtex);
		// --
		
		// TODO: Add proper support for multi-texturing.

		// blend mode
		switch (blendmode) {
		case BM_OPAQUE:	// 0
			break;
		case BM_TRANSPARENT: // 1
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GEQUAL,0.7f);
			break;
		case BM_ALPHA_BLEND: // 2
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case BM_ADDITIVE: // 3
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_COLOR, GL_ONE);
			break;
		case BM_ADDITIVE_ALPHA: // 4
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		case BM_MODULATE: // 5
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
			break;
		case BM_MODULATE2: // 6
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR); 
			break;
		default:
			gLog("Error: Unknown blendmode: %d\n", blendmode);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		//if (cull)
		//	glEnable(GL_CULL_FACE);

		// Texture wrapping around the geometry
		if (swrap)
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		if (twrap)
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

		// no writing to the depth buffer.
		if (noZWrite)
			glDepthMask(GL_FALSE);

		if (unlit) {
			glDisable(GL_LIGHTING);
			// unfogged = unlit?
			glDisable(GL_FOG);
		}

		// Environmental mapping, material, and effects
		if (useEnvMap) {
			// Turn on the 'reflection' shine, using 18.0f as that is what WoW uses based on the reverse engineering
			// This is now set in InitGL(); - no need to call it every render.
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 18.0f);
		
			// env mapping
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
		
			const GLint maptype = GL_SPHERE_MAP;
			//const GLint maptype = GL_REFLECTION_MAP_ARB;
		
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, maptype);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, maptype);
		}
		
		if (texanim!=-1) {
			glMatrixMode(GL_TEXTURE);
			glPushMatrix();
		
			m->texAnims[texanim].setup(texanim);
		}

		// color
		glColor4fv(ocol);
		//glMaterialfv(GL_FRONT, GL_SPECULAR, ocol);

		// don't use lighting on the surface
		if (unlit)
			glDisable(GL_LIGHTING);

		if (blendmode<=1 && ocol.w<1.0f)
			glEnable(GL_BLEND);

		return true;
	}

	return false;
}

void ModelRenderPass::deinit()
{
	switch (blendmode) {
	case BM_OPAQUE:
		break;
	case BM_TRANSPARENT:
		break;
	case BM_ALPHA_BLEND:
		//glDepthMask(GL_TRUE);
		break;
	default:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // default blend func
	}
	if (noZWrite) {
		glDepthMask(GL_TRUE);
	}
	if (texanim!=-1) {
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
	if (unlit) {
		glEnable(GL_LIGHTING);
		if (gWorld && gWorld->drawfog) glEnable(GL_FOG);
	}
	if (useEnvMap) {
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
	}
	if (usetex2) {
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0);
	}
	//glColor4f(1,1,1,1); //???
}

void Model::drawModel()
{
	// assume these client states are enabled: GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_TEXTURE_COORD_ARRAY

	if (animated) {

		if (animGeometry) {

			glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbuf);

			glVertexPointer(3, GL_FLOAT, 0, 0);
			glNormalPointer(GL_FLOAT, 0, GL_BUFFER_OFFSET(vbufsize));

		} else {
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbuf);
			glVertexPointer(3, GL_FLOAT, 0, 0);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, nbuf);
			glNormalPointer(GL_FLOAT, 0, 0);
		}

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, tbuf);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
		
		//glTexCoordPointer(2, GL_FLOAT, sizeof(ModelVertex), &origVertices[0].texcoords);
	}

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc (GL_GREATER, 0.3f);

	for (size_t i=0; i<passes.size(); i++) {
		ModelRenderPass &p = passes[i];

		if (p.init(this)) {
			// we don't want to render completely transparent parts
		
			// render
			if (animated) {
				//glDrawElements(GL_TRIANGLES, p.indexCount, GL_UNSIGNED_SHORT, indices + p.indexStart);
				// a GDC OpenGL Performace Tuning paper recommended glDrawRangeElements over glDrawElements
				// I can't notice a difference but I guess it can't hurt
				if ( supportVBO &&  supportDrawRangeElements) {
					glDrawRangeElements(GL_TRIANGLES, p.vertexStart, p.vertexEnd, p.indexCount, GL_UNSIGNED_SHORT, indices + p.indexStart);
				//} else if (!supportVBO) {
				//	glDrawElements(GL_TRIANGLES, p.indexCount, GL_UNSIGNED_SHORT, indices + p.indexStart); 
				} else {
					glBegin(GL_TRIANGLES);
					for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
						uint16 a = indices[b];
						glNormal3fv(normals[a]);
						glTexCoord2fv(origVertices[a].texcoords);
						glVertex3fv(vertices[a]);
					}
					glEnd();
				}
			} else {
				glBegin(GL_TRIANGLES);
				for (size_t k = 0, b=p.indexStart; k<p.indexCount; k++,b++) {
					uint16 a = indices[b];
					glNormal3fv(normals[a]);
					glTexCoord2fv(origVertices[a].texcoords);
					glVertex3fv(vertices[a]);
				}
				glEnd();
			}

			p.deinit();
		}

	}
	// done with all render ops

	glAlphaFunc (GL_GREATER, 0.0f);
	glDisable (GL_ALPHA_TEST);

	GLfloat czero[4] = {0,0,0,1};
	glMaterialfv(GL_FRONT, GL_EMISSION, czero);
	glColor4f(1,1,1,1);
	glDepthMask(GL_TRUE);
}

void TextureAnim::calc(int anim, int time)
{
	if (trans.uses(anim)) {
		tval = trans.getValue(anim, time);
	}
	if (rot.uses(anim)) {
        rval = rot.getValue(anim, time);
	}
	if (scale.uses(anim)) {
        sval = scale.getValue(anim, time);
	}
}

void TextureAnim::setup(int anim)
{
	glLoadIdentity();
	if (trans.uses(anim)) {
		glTranslatef(tval.x, tval.y, tval.z);
	}
	if (rot.uses(anim)) {
		glRotatef(rval.x, 0, 0, 1); // this is wrong, I have no idea what I'm doing here ;)
	}
	if (scale.uses(anim)) {
		glScalef(sval.x, sval.y, sval.z);
	}
}

void ModelCamera::init(MPQFile &f, ModelCameraDef &mcd, int *global)
{
	ok = true;
    nearclip = mcd.nearclip;
	farclip = mcd.farclip;
	fov = mcd.fov;
	pos = fixCoordSystem(mcd.pos);
	target = fixCoordSystem(mcd.target);
	tPos.init(mcd.transPos, f, global);
	tTarget.init(mcd.transTarget, f, global);
	rot.init(mcd.rot, f, global);
	tPos.fix(fixCoordSystem);
	tTarget.fix(fixCoordSystem);
}

void ModelCamera::setup(int time)
{
	if (!ok) return;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov * 34.5f, (GLfloat)video.xres/(GLfloat)video.yres, nearclip, farclip);

	Vec3D p = pos + tPos.getValue(0, time);
	Vec3D t = target + tTarget.getValue(0, time);

	Vec3D u(0,1,0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(p.x, p.y, p.z, t.x, t.y, t.z, u.x, u.y, u.z);
	//float roll = rot.getValue(0, time) / PI * 180.0f;
	//glRotatef(roll, 0, 0, 1);
}

void ModelColor::init(MPQFile &f, ModelColorDef &mcd, int *global)
{
	color.init(mcd.color, f, global);
	opacity.init(mcd.opacity, f, global);
}

void ModelTransparency::init(MPQFile &f, ModelTransDef &mcd, int *global)
{
	trans.init(mcd.trans, f, global);
}

void ModelLight::init(MPQFile &f, ModelLightDef &mld, int *global)
{
	tpos = pos = fixCoordSystem(mld.pos);
	tdir = dir = Vec3D(0,1,0); // no idea
	type = mld.type;
	parent = mld.bone;
	ambColor.init(mld.ambColor, f, global);
	ambIntensity.init(mld.ambIntensity, f, global);
	diffColor.init(mld.color, f, global);
	diffIntensity.init(mld.intensity, f, global);
}

void ModelLight::setup(int time, GLuint l)
{
	Vec4D ambcol(ambColor.getValue(0, time) * ambIntensity.getValue(0, time), 1.0f);
	Vec4D diffcol(diffColor.getValue(0, time) * diffIntensity.getValue(0, time), 1.0f);
	Vec4D p;
	if (type==MODELLIGHT_DIRECTIONAL) {
		// directional
		p = Vec4D(tdir, 0.0f);
	} else if (type==MODELLIGHT_POINT) {
		// point
		p = Vec4D(tpos, 1.0f);
	} else {
		p = Vec4D(tpos, 1.0f);
		gLog("Error: Light type %d is unknown.", type);
	}
	//gLog("Light %d (%f,%f,%f) (%f,%f,%f) [%f,%f,%f]\n", l-GL_LIGHT4, ambcol.x, ambcol.y, ambcol.z, diffcol.x, diffcol.y, diffcol.z, p.x, p.y, p.z);
	glLightfv(l, GL_POSITION, p);
	glLightfv(l, GL_DIFFUSE, diffcol);
	glLightfv(l, GL_AMBIENT, ambcol);
	glEnable(l);
}

void TextureAnim::init(MPQFile &f, ModelTexAnimDef &mta, int *global)
{
	trans.init(mta.trans, f, global);
	rot.init(mta.rot, f, global);
	scale.init(mta.scale, f, global);
}

void Bone::init(MPQFile &f, ModelBoneDef &b, int *global, MPQFile *animfiles)
{
	parent = b.parent;
	pivot = fixCoordSystem(b.pivot);
	billboard = (b.flags & MODELBONE_BILLBOARD) != 0;

	trans.init(b.translation, f, global, animfiles);
	rot.init(b.rotation, f, global, animfiles);
	scale.init(b.scaling, f, global, animfiles);
	trans.fix(fixCoordSystem);
	rot.fix(fixCoordSystemQuat);
	scale.fix(fixCoordSystem2);
}

void Bone::calcMatrix(Bone *allbones, int anim, int time)
{
	if (calc) return;
	Matrix m;
	Quaternion q;

	bool tr = rot.uses(anim) || scale.uses(anim) || trans.uses(anim) || billboard;
	if (tr) {
		m.translation(pivot);
		
		if (trans.uses(anim)) {
			Vec3D tr = trans.getValue(anim, time);
			m *= Matrix::newTranslation(tr);
		}
		if (rot.uses(anim)) {
			q = rot.getValue(anim, time);
			m *= Matrix::newQuatRotate(q);
		}
		if (scale.uses(anim)) {
			Vec3D sc = scale.getValue(anim, time);
			m *= Matrix::newScale(sc);
		}
		if (billboard) {
			float modelview[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

			Vec3D vRight = Vec3D(modelview[0], modelview[4], modelview[8]);
			Vec3D vUp = Vec3D(modelview[1], modelview[5], modelview[9]); // Spherical billboarding
			//Vec3D vUp = Vec3D(0,1,0); // Cylindrical billboarding
			vRight = vRight * -1;
			m.m[0][2] = vRight.x;
			m.m[1][2] = vRight.y;
			m.m[2][2] = vRight.z;
			m.m[0][1] = vUp.x;
			m.m[1][1] = vUp.y;
			m.m[2][1] = vUp.z;
		}

		m *= Matrix::newTranslation(pivot*-1.0f);
		
	} else m.unit();

	if (parent>=0) {
		allbones[parent].calcMatrix(allbones, anim, time);
		mat = allbones[parent].mat * m;
	} else mat = m;

	// transform matrix for normal vectors ... ??
	if (rot.uses(anim)) {
		if (parent>=0) {
			mrot = allbones[parent].mrot * Matrix::newQuatRotate(q);
		} else mrot = Matrix::newQuatRotate(q);
	} else mrot.unit();

	transPivot = mat * pivot;

	calc = true;
}


void Model::draw()
{
	if (!ok) return;

	if (!animated) {
		glCallList(dlist);
	} else {
		if (ind) animate(0);
		else {
			if (!animcalc) {
				animate(0);
				animcalc = true;
			}
		}
		lightsOn(GL_LIGHT4);
        drawModel();
		lightsOff(GL_LIGHT4);

		// effects are unfogged..?
		glDisable(GL_FOG);

		// draw particle systems
		for (size_t i=0; i<header.nParticleEmitters; i++) {
			particleSystems[i].draw();
		}

		// draw ribbons
		for (size_t i=0; i<header.nRibbonEmitters; i++) {
			ribbons[i].draw();
		}

		if (gWorld && gWorld->drawfog) glEnable(GL_FOG);
	}
}

void Model::lightsOn(GLuint lbase)
{
	// setup lights
	for (unsigned int i=0, l=lbase; i<header.nLights; i++) lights[i].setup(animtime, l++);
}

void Model::lightsOff(GLuint lbase)
{
	for (unsigned int i=0, l=lbase; i<header.nLights; i++) glDisable(l++);
}

void Model::updateEmitters(float dt)
{
	if (!ok) return;
	for (size_t i=0; i<header.nParticleEmitters; i++) {
		particleSystems[i].update(dt);
	}
}

int ModelManager::add(std::string name)
{
	int id;
	if (names.find(name) != names.end()) {
		id = names[name];
		items[id]->addref();
		return id;
	}
	// load new
	Model *model = new Model(name);
	id = nextID();
    do_add(name, id, model);
    return id;
}

void ModelManager::resetAnim()
{
	for (std::map<int, ManagedItem*>::iterator it = items.begin(); it != items.end(); ++it) {
		((Model*)it->second)->animcalc = false;
	}
}

void ModelManager::updateEmitters(float dt)
{
	for (std::map<int, ManagedItem*>::iterator it = items.begin(); it != items.end(); ++it) {
		((Model*)it->second)->updateEmitters(dt);
	}
}

ModelInstance::ModelInstance(Model *m, MPQFile &f) : model (m)
{
	float ff[3];
	unsigned int d1;
    f.read(&d1, 4);
	f.read(ff,12);
	pos = Vec3D(ff[0],ff[1],ff[2]);
	f.read(ff,12);
	dir = Vec3D(ff[0],ff[1],ff[2]);
	f.read(&scale,4);
	// scale factor - divide by 1024. blizzard devs must be on crack, why not just use a float?
	sc = scale / 1024.0f;
}

void ModelInstance::init2(Model *m, MPQFile &f)
{
	// MODD
	model = m;
	float ff[3],temp;
	f.read(ff,12); // Position (X,Z,-Y)
	pos = Vec3D(ff[0],ff[1],ff[2]);
	temp = pos.z;
	pos.z = -pos.y;
	pos.y = temp;
	f.read(&w,4); // W component of the orientation quaternion
	f.read(ff,12); // X, Y, Z components of the orientaton quaternion
	dir = Vec3D(ff[0],ff[1],ff[2]);
	f.read(&sc,4); // Scale factor
	unsigned int d1;
	f.read(&d1,4); // (B,G,R,A) Lightning-color. 
	lcol = fromARGB(d1);
}

void ModelInstance::draw()
{
	//if ((pos - gWorld->camera).lengthSquared() > (gWorld->modeldrawdistance2+(model->rad*model->rad*sc))) return;
	float dist = (pos - gWorld->camera).length() - model->rad;
	if (dist > gWorld->modeldrawdistance) return;
	if (!gWorld->frustum.intersectsSphere(pos, model->rad*sc)) return;

	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);

	glRotatef(dir.y - 90.0f, 0, 1, 0);
	glRotatef(-dir.x, 0, 0, 1);
	glRotatef(dir.z, 1, 0, 0);

	glScalef(sc,sc,sc);

	model->draw();
	glPopMatrix();
}

void glQuaternionRotate(const Vec3D& vdir, float w)
{
	Matrix m;
	Quaternion q(vdir, w);
	m.quaternionRotate(q);
	glMultMatrixf(m);
}

void ModelInstance::draw2(const Vec3D& ofs, const float rot)
{
	Vec3D tpos(ofs + pos);
	rotate(ofs.x,ofs.z,&tpos.x,&tpos.z,rot*PI/180.0f);
	if ( (tpos - gWorld->camera).lengthSquared() > (gWorld->doodaddrawdistance2*model->rad*sc) ) return;
	if (!gWorld->frustum.intersectsSphere(tpos, model->rad*sc)) return;

	glPushMatrix();

	glTranslatef(pos.x, pos.y, pos.z);
	Vec3D vdir(-dir.z,dir.x,dir.y);
	glQuaternionRotate(vdir,w);
	glScalef(sc,-sc,-sc);

	model->draw();
	glPopMatrix();
}

