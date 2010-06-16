#include "wmo.h"
#include "world.h"
#include "liquid.h"
#include "shaders.h"

#define PI 3.14159265358f

WMO::WMO(std::string name): name(name), groups(0), nTextures(0), nGroups(0),
	nP(0), nLights(0), nModels(0), nDoodads(0), nDoodadSets(0), nX(0), mat(0), LiquidType(0)
{
	MPQFile f(name.c_str());
	ok = !f.isEof();
	if (!ok) {
		gLog("Error: loading WMO %s\n", name.c_str());
		return;
	}

	gLog("Loading WMO %s\n", name.c_str());

	char fourcc[5];
	uint32 size;
	float ff[3];

	char *ddnames;
	char *groupnames;

	char *texbuf=0;

	while (!f.isEof()) {
		memset(fourcc, 0, 4);
		size = 0;
		f.read(fourcc,4);
		f.read(&size, 4);
		flipcc(fourcc);
		fourcc[4] = 0;

		if (size == 0)
			continue;

		size_t nextpos = f.getPos() + size;

		if (strcmp(fourcc,"MVER")==0) {
		}
		else if (strcmp(fourcc,"MOHD")==0) {
			unsigned int col;
			// Header for the map object. 64 bytes.
			f.read(&nTextures, 4); // number of materials
			f.read(&nGroups, 4); // number of WMO groups
			f.read(&nP, 4); // number of portals
			f.read(&nLights, 4); // number of lights
			f.read(&nModels, 4); // number of M2 models imported
			f.read(&nDoodads, 4); // number of dedicated files (*see below this table!) 
			f.read(&nDoodadSets, 4); // number of doodad sets
			f.read(&col, 4); // ambient color?
			f.read(&nX, 4); // WMO ID (column 2 in WMOAreaTable.dbc)
			f.read(ff,12); // Bounding box corner 1
			v1 = Vec3D(ff[0],ff[1],ff[2]);
			f.read(ff,12); // Bounding box corner 2
			v2 = Vec3D(ff[0],ff[1],ff[2]);
			f.read(&LiquidType, 4); // LiquidType related, see below in the MLIQ chunk.

			groups = new WMOGroup[nGroups];
			mat = new WMOMaterial[nTextures];

		}
		else if (strcmp(fourcc,"MOTX")==0) {
			// textures
			texbuf = new char[size+1];
			f.read(texbuf, size);
			texbuf[size] = 0;
		}
		else if (strcmp(fourcc,"MOMT")==0) {
			for (int i=0; i<nTextures; i++) {
				WMOMaterial *m = &mat[i];
				f.read(m, 0x40); // read 64 bytes, struct WMOMaterial is 68 bytes

				std::string texpath(texbuf+m->nameStart);

				m->tex = wow::Texture(texpath.c_str());
			}
		}
		else if (strcmp(fourcc,"MOGN")==0) {
			groupnames = (char*)f.getPointer();
		}
		else if (strcmp(fourcc,"MOGI")==0) {
			for (int i=0; i<nGroups; i++) {
				groups[i].init(this, f, i, groupnames);

			}
		}
		else if (strcmp(fourcc,"MOSB")==0) {
			if (size>4) {
				std::string path = (char*)f.getPointer();
				if (path.length()) {
					gLog("SKYBOX:\n");

					skybox = wow::Model(path);

					if (!skybox.get().ok) {
						skybox = wow::Model();
					}
				}
			}
		}
		else if (strcmp(fourcc,"MOPV")==0) {
			WMOPV p;
			for (int i=0; i<nP; i++) {
				f.read(ff,12);
				p.a = Vec3D(ff[0],ff[2],-ff[1]);
				f.read(ff,12);
				p.b = Vec3D(ff[0],ff[2],-ff[1]);
				f.read(ff,12);
				p.c = Vec3D(ff[0],ff[2],-ff[1]);
				f.read(ff,12);
				p.d = Vec3D(ff[0],ff[2],-ff[1]);
				pvs.push_back(p);
			}
		}
		else if (strcmp(fourcc,"MOPT")==0) {
		}
		else if (strcmp(fourcc,"MOPR")==0) {
			int nn = (int)size / sizeof(WMOPR);
			WMOPR *pr = (WMOPR*)f.getPointer();
			for (int i=0; i<nn; i++) {
				prs.push_back(*pr++);
			}
		}
		else if (strcmp(fourcc,"MOVV")==0) {
		}
		else if (strcmp(fourcc,"MOVB")==0) {
		}
		else if (strcmp(fourcc,"MOLT")==0) {
			for (int i=0; i<nLights; i++) {
				WMOLight l;
				l.init(f);
				lights.push_back(l);
			}
		}
		else if (strcmp(fourcc,"MODS")==0) {
			for (int i=0; i<nDoodadSets; i++) {
				WMODoodadSet dds;
				f.read(&dds, sizeof(WMODoodadSet));
				doodadsets.push_back(dds);
			}
		}
		else if (strcmp(fourcc,"MODN")==0) {
			if (size) {
				ddnames = (char*)f.getPointer();

				char *p=ddnames,*end=p+size;
				int t=0;
				while (p<end) {
					std::string path(p);
					p+=strlen(p)+1;
					while ((p<end) && (*p==0)) p++;

					models.push_back(path);
				}
				f.seekRelative((int)size);
			}
		}
		else if (strcmp(fourcc,"MODD")==0) {
			nModels = (int)size / 0x28;
			for (int i=0; i<nModels; i++) {
				int ofs;
				f.read(&ofs,4);
				if (!ddnames) // Alfred, Error
					continue;
				wow::Model m(ddnames + ofs);
				ModelInstance mi;
				mi.init2(m,f);
				modelis.push_back(mi);
			}

		}
		else if (strcmp(fourcc,"MFOG")==0) {
			int nfogs = (int)size / 0x30;
			for (int i=0; i<nfogs; i++) {
				WMOFog fog;
				fog.init(f);
				fogs.push_back(fog);
			}
		}
		else if (strcmp(fourcc,"MCVP")==0) {
			gLog("No implement wmo chunk %s [%d].\n", fourcc, size);
		}
		else {
			gLog("No implement wmo chunk %s [%d].\n", fourcc, size);
		}

		f.seek((int)nextpos);
	}

	f.close();
	delete[] texbuf;

	for (int i=0; i<nGroups; i++) 
		groups[i].initDisplayList();

}

WMO::~WMO()
{
	if (!ok)
		return;

	gLog("Unloading WMO %s\n", name.c_str());
	if (groups)
		delete[] groups;

	if (mat)
		delete[] mat;
}

void WMO::draw(int doodadset, const Vec3D &ofs, const float rot)const
{
	if (!ok) return;
	
	for (int i=0; i<nGroups; i++) {
		groups[i].draw(ofs, rot);
	}

	if (gWorld->drawdoodads) {
		for (int i=0; i<nGroups; i++) {
			groups[i].drawDoodads(doodadset, ofs, rot);
		}
	}

	for (int i=0; i<nGroups; i++) {
		groups[i].drawLiquid();
	}
}

void WMO::drawSkybox()const
{
	if (skybox!=wow::Model()) {
		// TODO: only draw sky if we are "inside" the WMO... ?

		// We need to clear the depth buffer, because the skybox model can (will?)
		// require it *. This is inefficient - is there a better way to do this?
		// * planets in front of "space" in Caverns of Time
		//glClear(GL_DEPTH_BUFFER_BIT);

		// update: skybox models seem to have an explicit renderop ordering!
		// that saves us the depth buffer clear and the depth testing, too

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glPushMatrix();
		Vec3D o = gWorld->camera;
		glTranslatef(o.x, o.y, o.z);
		const float sc = 2.0f;
		glScalef(sc,sc,sc);
        skybox.get().draw();
		glPopMatrix();
		gWorld->hadSky = true;
		glEnable(GL_DEPTH_TEST);
	}
}

/*
void WMO::drawPortals()
{
	// not used ;)
	glBegin(GL_QUADS);
	for (int i=0; i<nP; i++) {
		glVertex3fv(pvs[i].d);
		glVertex3fv(pvs[i].c);
		glVertex3fv(pvs[i].b);
		glVertex3fv(pvs[i].a);
	}
	glEnd();
}
*/

void WMOLight::init(MPQFile &f)
{
	f.read(&LightType, 1);
	f.read(&type, 1);
	f.read(&useAtten, 1);
	f.read(&pad, 1);
	f.read(&color,4);
	f.read(pos, 12);
	f.read(&intensity, 4);
	f.read(&attenStart, 4);
	f.read(&attenEnd, 4);
	f.read(unk, 4*4);

	pos = Vec3D(pos.x, pos.z, -pos.y);

	fcolor = fromARGB(color);
	fcolor *= intensity;
	fcolor.w = 1.0f;
}

void WMOLight::setup(GLint light)
{
	// not used right now -_-

	GLfloat LightAmbient[] = {0, 0, 0, 1.0f};
	GLfloat LightPosition[] = {pos.x, pos.y, pos.z, 0.0f};

	glLightfv(light, GL_AMBIENT, LightAmbient);
	glLightfv(light, GL_DIFFUSE, fcolor);
	glLightfv(light, GL_POSITION,LightPosition);

	glEnable(light);
}

void WMOLight::setupOnce(GLint light, Vec3D dir, Vec4D lcol)
{
	Vec4D position(dir, 0);
	Vec4D ambient = lcol*0.3f;
	Vec4D diffuse = lcol;

	glLightfv(light, GL_AMBIENT, ambient);
	glLightfv(light, GL_DIFFUSE, diffuse);
	glLightfv(light, GL_POSITION,position);
	
	glEnable(light);
}

void WMOGroup::init(WMO *wmo, MPQFile &f, int num, char *names)
{
	this->wmo = wmo;
	this->num = num;

	// extract group info from f
	f.read(&flags,4);
	float ff[3];
	f.read(ff,12); // Bounding box corner 1
	v1 = Vec3D(ff[0],ff[1],ff[2]);
	f.read(ff,12); // Bounding box corner 2
	v2 = Vec3D(ff[0],ff[1],ff[2]);
	int nameOfs;
	f.read(&nameOfs,4); // name in MOGN chunk (or -1 for no name?)

	// TODO: get proper name from group header and/or dbc?
	if (nameOfs > 0) {
       	name = std::string(names + nameOfs);
	} else 
		name = "(no name)";

	ddr = 0;
	nDoodads = 0;

	lq = 0;
}


struct WMOBatch {
	signed char bytes[12];
	unsigned int indexStart;
	unsigned short indexCount, vertexStart, vertexEnd;
	unsigned char flags, texture;
};

void setGLColor(unsigned int col)
{
	//glColor4ubv((GLubyte*)(&col));
	GLubyte r,g,b,a;
	a = (col & 0xFF000000) >> 24;
	r = (col & 0x00FF0000) >> 16;
	g = (col & 0x0000FF00) >> 8;
	b = (col & 0x000000FF);
    glColor4ub(r,g,b,1);
}

Vec4D colorFromInt(unsigned int col) {
	GLubyte r,g,b,a;
	a = (col & 0xFF000000) >> 24;
	r = (col & 0x00FF0000) >> 16;
	g = (col & 0x0000FF00) >> 8;
	b = (col & 0x000000FF);
	return Vec4D(r/255.0f, g/255.0f, b/255.0f, a/255.0f);
}

struct WMOGroupHeader {
    	int nameStart; // Group name (offset into MOGN chunk)
	int nameStart2; // Descriptive group name (offset into MOGN chunk)
	int flags;
	float box1[3]; // Bounding box corner 1 (same as in MOGI)
	float box2[3]; // Bounding box corner 2
	short portalStart; // Index into the MOPR chunk
	short portalCount; // Number of items used from the MOPR chunk
	short batches[4];
	uint8 fogs[4]; // Up to four indices into the WMO fog list
	int32 unk1; // LiquidType related, see below in the MLIQ chunk.
	int32 id; // WMO group ID (column 4 in WMOAreaTable.dbc)
	int32 unk2; // Always 0?
	int32 unk3; // Always 0?
};

struct SMOPoly {
	uint8 flags;
	uint8 mtlId;
};

void WMOGroup::initDisplayList()
{
	Vec3D *vertices, *normals;
	Vec2D *texcoords;
	unsigned short *indices;
	struct SMOPoly *materials;
	WMOBatch *batches;
	//int nBatches;

	WMOGroupHeader gh;

	short *useLights = 0;
	int nLR = 0;

	// open group file
	char temp[256];
	strcpy(temp, wmo->name.c_str());
	temp[wmo->name.length()-4] = 0;
	
	char fname[256];
	sprintf(fname,"%s_%03d.wmo",temp, num);

	MPQFile gf(fname);
	if (gf.isEof()) {
		return;
	}
	gf.seek(0x14); // a header at 0x14

	// read MOGP chunk header
	gf.read(&gh, sizeof(WMOGroupHeader));
	WMOFog &wf = wmo->fogs[gh.fogs[0]];
	if (wf.r2 <= 0) fog = -1; // default outdoor fog..?
	else fog = gh.fogs[0];

	b1 = Vec3D(gh.box1[0], gh.box1[2], -gh.box1[1]);
	b2 = Vec3D(gh.box2[0], gh.box2[2], -gh.box2[1]);

	gf.seek(0x58); // first chunk at 0x58

	char fourcc[5];
	uint32 size = 0;

	unsigned int *cv;
	hascv = false;

	while (!gf.isEof()) {
		memset(fourcc, 0, 4);
		size = 0;
		gf.read(fourcc,4);
		gf.read(&size, 4);
		flipcc(fourcc);
		fourcc[4] = 0;

		if (size == 0)
			continue;

		size_t nextpos = gf.getPos() + size;

		if (strcmp(fourcc,"MOPY")==0) {
			// materials per triangle
			nTriangles = (int)size / 2;
			materials = (struct SMOPoly*)gf.getPointer();
		}
		else if (strcmp(fourcc,"MOVI")==0) {
			// indices
			indices =  (unsigned short*)gf.getPointer();
		}
		else if (strcmp(fourcc,"MOVT")==0) {
			nVertices = (int)size / 12;
			// let's hope it's padded to 12 bytes, not 16...
			vertices =  (Vec3D*)gf.getPointer();
			vmin = Vec3D( 9999999.0f, 9999999.0f, 9999999.0f);
			vmax = Vec3D(-9999999.0f,-9999999.0f,-9999999.0f);
			rad = 0;
			for (int i=0; i<nVertices; i++) {
				Vec3D v(vertices[i].x, vertices[i].z, -vertices[i].y);
				if (v.x < vmin.x) vmin.x = v.x;
				if (v.y < vmin.y) vmin.y = v.y;
				if (v.z < vmin.z) vmin.z = v.z;
				if (v.x > vmax.x) vmax.x = v.x;
				if (v.y > vmax.y) vmax.y = v.y;
				if (v.z > vmax.z) vmax.z = v.z;
			}
			center = (vmax + vmin) * 0.5f;
			rad = (vmax-center).length();
		}
		else if (strcmp(fourcc,"MONR")==0) {
			// Normals. 3 floats per vertex normal, in (X,Z,-Y) order.
			normals =  (Vec3D*)gf.getPointer();
		}
		else if (strcmp(fourcc,"MOTV")==0) {
			// Texture coordinates, 2 floats per vertex in (X,Y) order. The values range from 0.0 to 1.0. Vertices, normals and texture coordinates are in corresponding order, of course.
			texcoords =  (Vec2D*)gf.getPointer();
		}
		else if (strcmp(fourcc,"MOBA")==0) {
			nBatches = (uint32)size / 24;
			batches = (WMOBatch*)gf.getPointer();
		}
		else if (strcmp(fourcc,"MOLR")==0) {
			nLR = (int)size / 2;
			useLights =  (short*)gf.getPointer();
		}
		else if (strcmp(fourcc,"MODR")==0) {
			nDoodads = (int)size / 2;
			ddr = new short[nDoodads];
			gf.read(ddr,size);
		}
		else if (strcmp(fourcc,"MOBN")==0) {
		}
		else if (strcmp(fourcc,"MOBR")==0) {
		}
		else if (strcmp(fourcc,"MOCV")==0) {
			hascv = true;
			cv = (unsigned int*)gf.getPointer();
		}
		else if (strcmp(fourcc,"MLIQ")==0) {
			WMOLiquidHeader hlq;
			gf.read(&hlq, 0x1E);

			lq = new Liquid(hlq.A, hlq.B, Vec3D(hlq.pos.x, hlq.pos.z, -hlq.pos.y));
			lq->initFromWMO(gf, wmo->mat[hlq.type], (flags&0x2000)!=0);
		} else {
			gLog("No implement wmo group chunk %s [%d].\n", fourcc, size);
		}

 		gf.seek((int)nextpos);
	}

	indoor = (flags&8192)!=0;
	initLighting(nLR,useLights);

	GLuint listbase = glGenLists(nBatches);

	for (uint32 b=0; b<nBatches; b++) {

		GLuint list = listbase + b;

		WMOBatch *batch = &batches[b];
		WMOMaterial *mat = &wmo->mat[batch->texture];

		bool overbright = ((mat->flags & 0x10) && !hascv);
		bool spec_shader = (mat->specular && !hascv && !overbright);

		std::pair<GLuint, int> currentList;
		currentList.first = list;
		currentList.second = spec_shader ? 1 : 0;

		glNewList(list, GL_COMPILE);

        // setup texture
		mat->tex.get().bind();

		bool atest = (mat->transparent) != 0;

		if (atest) {
			glEnable(GL_ALPHA_TEST);
			float aval = 0;
            if (mat->flags & 0x80) aval = 0.3f;
			if (mat->flags & 0x01) aval = 0.0f;
			glAlphaFunc(GL_GREATER, aval);
		}

		if (mat->flags & 0x04) glDisable(GL_CULL_FACE);
		else glEnable(GL_CULL_FACE);

		if (spec_shader) {
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colorFromInt(mat->col2));
		} else {
			Vec4D nospec(0,0,0,1);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, nospec);
		}

		if (overbright) {
			// TODO: use emissive color from the WMO Material instead of 1,1,1,1
			GLfloat em[4] = {1,1,1,1};
			glMaterialfv(GL_FRONT, GL_EMISSION, em);
		}
		
		// render
		glBegin(GL_TRIANGLES);
		for (int t=0, i=batch->indexStart; t<batch->indexCount; t++,i++) {
			int a = indices[i];
			if (indoor && hascv) {
	            setGLColor(cv[a]);
			}
			glNormal3f(normals[a].x, normals[a].z, -normals[a].y);
			glTexCoord2fv(texcoords[a]);
			glVertex3f(vertices[a].x, vertices[a].z, -vertices[a].y);
		}
		glEnd();

		if (overbright) {
			GLfloat em[4] = {0,0,0,1};
			glMaterialfv(GL_FRONT, GL_EMISSION, em);
		}

		if (atest) {
			glDisable(GL_ALPHA_TEST);
		}

		glEndList();
		lists.push_back(currentList);
	}

	gf.close();

	indoor = false;
}


void WMOGroup::initLighting(int nLR, short *useLights)
{
	//dl_light = 0;
	// "real" lighting?
	if ((flags&0x2000) && hascv) {

		Vec3D dirmin(1,1,1);
		float lenmin;
		int lmin;

		for (int i=0; i<nDoodads; i++) {
			lenmin = 999999.0f*999999.0f;
			lmin = 0;
			ModelInstance &mi = wmo->modelis[ddr[i]];
			for (int j=0; j<wmo->nLights; j++) {
				WMOLight &l = wmo->lights[j];
				Vec3D dir = l.pos - mi.pos;
				float ll = dir.lengthSquared();
				if (ll < lenmin) {
					lenmin = ll;
					dirmin = dir;
					lmin = j;
				}
			}
			mi.light = lmin;
			mi.ldir = dirmin;
		}

		outdoorLights = false;
	} else {
		outdoorLights = true;
	}
}

void WMOGroup::draw(const Vec3D& ofs, const float rot)
{
	visible = false;
	// view frustum culling
	Vec3D pos = center + ofs;
	rotate(ofs.x,ofs.z,&pos.x,&pos.z,rot*PI/180.0f);
	if (!gWorld->frustum.intersectsSphere(pos,rad)) return;
	float dist = (pos - gWorld->camera).length() - rad;
	if (dist >= gWorld->culldistance) return;
	visible = true;
	
	if (hascv) {
		glDisable(GL_LIGHTING);
		gWorld->outdoorLights(false);
	} else {
		if (gWorld->lighting) {
			if (gWorld->skies->hasSkies()) {
				gWorld->outdoorLights(true);
			} else {
				// set up some kind of default outdoor light... ?
				glEnable(GL_LIGHT0);
				glDisable(GL_LIGHT1);
				glLightfv(GL_LIGHT0, GL_AMBIENT, Vec4D(0.4f,0.4f,0.4f,1));
				glLightfv(GL_LIGHT0, GL_DIFFUSE, Vec4D(0.8f,0.8f,0.8f,1));
				glLightfv(GL_LIGHT0, GL_POSITION, Vec4D(1,1,1,0));
			}
		} else glDisable(GL_LIGHTING);
	}
	setupFog();

	
	//glCallList(dl);
	glDisable(GL_BLEND);
	glColor4f(1,1,1,1);
	for (unsigned int i=0; i<nBatches; i++) {
		bool useshader = (gWorld->useshaders && lists[i].second);
		if (useshader) wmoShader->bind();
		glCallList(lists[i].first);
		if (useshader) wmoShader->unbind();
	}

	glColor4f(1,1,1,1);
	glEnable(GL_CULL_FACE);

	if (hascv) {
		if (gWorld->lighting) {
			glEnable(GL_LIGHTING);
			//glCallList(dl_light);
		}
	}


}

void WMOGroup::drawDoodads(int doodadset, const Vec3D& ofs, const float rot)
{
	if (!visible) return;
	if (nDoodads==0) return;

	gWorld->outdoorLights(outdoorLights);
	setupFog();

	// draw doodads
	glColor4f(1,1,1,1);
	for (int i=0; i<nDoodads; i++) {
		short dd = ddr[i];
		bool inSet;
		// apparently, doodadset #0 (defaultGlobal) should always be visible
		inSet = ( ((dd >= wmo->doodadsets[doodadset].start) && (dd < (wmo->doodadsets[doodadset].start+wmo->doodadsets[doodadset].size))) 
			|| ( (dd >= wmo->doodadsets[0].start) && ((dd < (wmo->doodadsets[0].start+wmo->doodadsets[0].size) )) ) );
		if (inSet) {
 		//if ((dd >= wmo->doodadsets[doodadset].start) && (dd < (wmo->doodadsets[doodadset].start+wmo->doodadsets[doodadset].size))) {

			ModelInstance &mi = wmo->modelis[dd];

			if (!outdoorLights) {
				WMOLight::setupOnce(GL_LIGHT2, mi.ldir, mi.lcol);
			}

			wmo->modelis[dd].draw2(ofs,rot);
		}
	}

	glDisable(GL_LIGHT2);

	glColor4f(1,1,1,1);

}

void WMOGroup::drawLiquid()
{
	if (!visible) return;

	// draw liquid
	// TODO: culling for liquid boundingbox or something
	if (lq) {
		setupFog();
		if (outdoorLights) {
			gWorld->outdoorLights(true);
		} else {
			// TODO: setup some kind of indoor lighting... ?
			gWorld->outdoorLights(false);
			glEnable(GL_LIGHT2);
			glLightfv(GL_LIGHT2, GL_AMBIENT, Vec4D(0.1f,0.1f,0.1f,1));
			glLightfv(GL_LIGHT2, GL_DIFFUSE, Vec4D(0.8f,0.8f,0.8f,1));
			glLightfv(GL_LIGHT2, GL_POSITION, Vec4D(0,1,0,0));
		}
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glDepthMask(GL_TRUE);
		glColor4f(1,1,1,1);
		lq->draw();
		glDisable(GL_LIGHT2);
	}
}

void WMOGroup::setupFog()
{
	if (outdoorLights || fog==-1) {
		gWorld->setupFog();
	} else {
		wmo->fogs[fog].setup();
	}
}



WMOGroup::~WMOGroup()
{
	if (nBatches && lists.size()) glDeleteLists(lists[0].first, nBatches);

	if (nDoodads) delete[] ddr;
	if (lq) delete lq;
}


void WMOFog::init(MPQFile &f)
{
	f.read(this, 0x30);
	color = Vec4D( ((color1 & 0x00FF0000) >> 16)/255.0f, ((color1 & 0x0000FF00) >> 8)/255.0f,
					(color1 & 0x000000FF)/255.0f, ((color1 & 0xFF000000) >> 24)/255.0f);
	float temp;
	temp = pos.y;
	pos.y = pos.z;
	pos.z = -temp;
	fogstart = fogstart * fogend;
}

void WMOFog::setup()
{
	if (gWorld->drawfog) {
		glFogfv(GL_FOG_COLOR, color);
		glFogf(GL_FOG_START, fogstart);
		glFogf(GL_FOG_END, fogend);

		glEnable(GL_FOG);
	} else {
		glDisable(GL_FOG);
	}
}

WMOInstance::WMOInstance(wow::WMO const& wmo, MPQFile &f) : wmo (wmo)
{

	float ff[3];
    	f.read(&id, 4); // unique identifier for this instance
	f.read(ff,12); // Position (X,Y,Z)
	pos = Vec3D(ff[0],ff[1],ff[2]);
	f.read(ff,12); // Orientation (A,B,C)
	dir = Vec3D(ff[0],ff[1],ff[2]);
	f.read(ff,12); // extends
	pos2 = Vec3D(ff[0],ff[1],ff[2]);
	f.read(ff,12);
	pos3 = Vec3D(ff[0],ff[1],ff[2]);
	f.read(&flags,2);
	f.read(&doodadset,2); // Doodad set index
	f.read(&nameset, 2); // Name set
	f.read(&unk, 2);
}

void WMOInstance::draw()
{
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);

	float rot = -90.0f + dir.y;

	// TODO: replace this with a single transform matrix calculated at load time

	glRotatef(dir.y - 90.0f, 0, 1, 0);
	glRotatef(-dir.x, 0, 0, 1);
	glRotatef(dir.z, 1, 0, 0);

	wmo.get().draw(doodadset,pos,-rot);

	glPopMatrix();
}

/*
void WMOInstance::drawPortals()
{
	if (ids.find(id) != ids.end()) return;
	ids.insert(id);

	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);

	glRotatef(dir.y - 90.0f, 0, 1, 0);
	glRotatef(-dir.x, 0, 0, 1);
	glRotatef(dir.z, 1, 0, 0);

	wmo->drawPortals();
	glPopMatrix();
}
*/
