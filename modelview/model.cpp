#include "globalvars.h"
#include "modelviewer.h"
#include "model.h"
#include "mpq.h"

#include <cassert>
#include <algorithm>
#include "util.h"

int globalTime = 0;
extern ModelViewer *g_modelViewer;

AnimManager::AnimManager(ModelAnimation *anim) {
	AnimIDSecondary = -1;
	AnimIDMouth = -1;
	anims = anim;
	AnimParticles = false;

	Count = 1;
	PlayIndex = 0;
	CurLoop = 0;
	animList[0].AnimID = 0;
	animList[0].Loops = 0;

	if (anims != NULL) { 
		Frame = anims[0].timeStart;
		TotalFrames = anims[0].timeEnd - anims[0].timeStart;
	} else {
		Frame = 0;
		TotalFrames = 0;
	}

	Speed = 1.0f;
	mouthSpeed = 1.0f;
	
	Paused = false;
}

AnimManager::~AnimManager() {
	anims = NULL;
}


void AnimManager::AddAnim(unsigned int id, short loops) {
	if (Count > 3)
		return;

	animList[Count].AnimID = id;
	animList[Count].Loops = loops;
	Count++;
}

void AnimManager::Set(short index, unsigned int id, short loops) {
	// error check, we currently only support 4 animations.
	if (index > 3)
		return;

	animList[index].AnimID = id;
	animList[index].Loops = loops;

	// Just an error check for our "auto animate"
	if (index == 0) {
		Count = 1;
		PlayIndex = index;
		Frame = anims[id].timeStart;
		TotalFrames = anims[id].timeEnd - anims[id].timeStart;
	}

	if (index+1 > Count)
		Count = index+1;
}

void AnimManager::Play() {
	PlayIndex = 0;
	//if (Frame == 0 && PlayID == 0) {
		CurLoop = animList[PlayIndex].Loops;
		Frame = anims[animList[PlayIndex].AnimID].timeStart;
		TotalFrames = anims[animList[PlayIndex].AnimID].timeEnd - anims[animList[PlayIndex].AnimID].timeStart;
	//}

	Paused = false;
	AnimParticles = false;
}

void AnimManager::Stop() {
	Paused = true;
	PlayIndex = 0;
	Frame = anims[animList[0].AnimID].timeStart;
	CurLoop = animList[0].Loops;
}

void AnimManager::Pause(bool force) {
	if (Paused && force == false) {
		Paused = false;
		AnimParticles = !Paused;
	} else {
		Paused = true;
		AnimParticles = !Paused;
	}
}

void AnimManager::Next() {
	if(CurLoop == 1) {
		PlayIndex++;
		if (PlayIndex >= Count) {
			Stop();
			return;
		}

		CurLoop = animList[PlayIndex].Loops;
	} else if(CurLoop > 1) {
		CurLoop--;
	} else if(CurLoop == 0) {
		PlayIndex++;
		if (PlayIndex >= Count) {
			PlayIndex = 0;
		}
	}
	
	Frame = anims[animList[PlayIndex].AnimID].timeStart;
#ifdef WotLK
	TotalFrames = GetFrameCount();
#endif
}

void AnimManager::Prev() {
	if(CurLoop >= animList[PlayIndex].Loops) {
		PlayIndex--;

		if (PlayIndex < 0) {
			Stop();
			return;
		}

		CurLoop = animList[PlayIndex].Loops;
	} else if(CurLoop < animList[PlayIndex].Loops) {
		CurLoop++;
	}

	Frame = anims[animList[PlayIndex].AnimID].timeEnd;
#ifdef WotLK
	TotalFrames = GetFrameCount();
#endif
}

int AnimManager::Tick(int time) {
	if((Count < PlayIndex) )
		return -1;

	Frame += int(time*Speed);

	// animate our mouth animation
	if (AnimIDMouth > -1) {
		FrameMouth += (time*mouthSpeed);

		if (FrameMouth >= anims[AnimIDMouth].timeEnd) {
			FrameMouth -= (anims[AnimIDMouth].timeEnd - anims[AnimIDMouth].timeStart);
		} else if (FrameMouth < anims[AnimIDMouth].timeStart) {
			FrameMouth += (anims[AnimIDMouth].timeEnd - anims[AnimIDMouth].timeStart);
		}
	}

	// animate our second (upper body) animation
	if (AnimIDSecondary > -1) {
		FrameSecondary += (time*Speed);

		if (FrameSecondary >= anims[AnimIDSecondary].timeEnd) {
			FrameSecondary -= (anims[AnimIDSecondary].timeEnd - anims[AnimIDSecondary].timeStart);
		} else if (FrameSecondary < anims[AnimIDSecondary].timeStart) {
			FrameSecondary += (anims[AnimIDSecondary].timeEnd - anims[AnimIDSecondary].timeStart);
		}
	}

	if (Frame >= anims[animList[PlayIndex].AnimID].timeEnd) {
		Next();
		return 1;
	} else if (Frame < anims[animList[PlayIndex].AnimID].timeStart) {
		Prev();
		return 1;
	}

	return 0;
}

unsigned int AnimManager::GetFrameCount() {
	return (anims[animList[PlayIndex].AnimID].timeEnd - anims[animList[PlayIndex].AnimID].timeStart);
}


void AnimManager::NextFrame()
{
	//AnimateParticles();
	int id = animList[PlayIndex].AnimID;
	Frame += int((anims[id].timeEnd - anims[id].timeStart) / 60);
	TimeDiff = int((anims[id].timeEnd - anims[id].timeStart) / 60);
}

void AnimManager::PrevFrame()
{
	//AnimateParticles();
	int id = animList[PlayIndex].AnimID;
	Frame -= int((anims[id].timeEnd - anims[id].timeStart) / 60);
	TimeDiff = int((anims[id].timeEnd - anims[id].timeStart) / 60) * -1;
}

void AnimManager::SetFrame(unsigned int f)
{
	//TimeDiff = f - Frame;
	Frame = f;
}

int AnimManager::GetTimeDiff()
{
	int t = TimeDiff;
	TimeDiff = 0;
	return t;
}

void AnimManager::SetTimeDiff(int i)
{
	TimeDiff = i;
}

void AnimManager::Clear() {
	Stop();
	Paused = true;
	PlayIndex = 0;
	Count = 0;
	CurLoop = 0;
	Frame = 0;
}

Model::Model(std::string name, bool forceAnim) : ManagedItem(name), forceAnim(forceAnim)
{
	if (name == "")
		return;

	// replace .MDX with .M2
	wxString tempname(name.c_str(), wxConvUTF8);
	tempname = tempname.BeforeLast(_T('.')).Append(_T(".m2"));

	// Initiate our model variables.
	trans = 1.0f;
	rad = 1.0f;
	pos = Vec3D(0.0f, 0.0f, 0.0f);
	rot = Vec3D(0.0f, 0.0f, 0.0f);

	for (int i=0; i<TEXTURE_MAX; i++) {
		specialTextures[i] = -1;
		replaceTextures[i] = 0;
		useReplaceTextures[i] = false;
	}

	for (int i=0; i<ATT_MAX; i++) 
		attLookup[i] = -1;

	for (int i=0; i<BONE_MAX; i++) 
		keyBoneLookup[i] = -1;


	dlist = 0;
	bounds = 0;
	boundTris = 0;
	showGeosets = 0;

	hasCamera = false;
	hasParticles = false;
	isWMO = false;
	isMount = false;

	showModel = false;
	showBones = false;
	showBounds = false;
	showWireframe = false;
	showParticles = false;
	showTexture = true;

	charModelDetails.Reset();
	
	vbuf = nbuf = tbuf = 0;
	
	origVertices = 0;
	vertices = 0;
	normals = 0;
	texCoords = 0;
	indices = 0;
	
	animtime = 0;
	anim = 0;
	anims = 0;
	animLookups = 0;
	animManager = NULL;
	bones = 0;
	bounds = 0;
	boundTris = 0;
	currentAnim = 0;
	colors = 0;
	globalSequences = 0;
	lights = 0;
	particleSystems = 0;
	ribbons = 0;
	texAnims = 0;
	textures = 0;
	transparency = 0;
	events = 0;
	modelType = MT_NORMAL;
	// --

	MPQFile f((char *)tempname.c_str());
	g_modelViewer->modelOpened->Add(tempname);
	ok = false;
	if (f.isEof() || (f.getSize() < sizeof(ModelHeader))) {
		wxLogMessage(_T("Error: Unable to load model: [%s]"), tempname.c_str());
		// delete this; //?
		f.close();
		return;
	}
	ok = true;
	
	memcpy(&header, f.getBuffer(), sizeof(ModelHeader));
	animated = isAnimated(f) || forceAnim;  // isAnimated will set animGeometry and animTextures

	wxLogMessage(_T("Loading model: %s\n"), tempname.c_str());

	// Error check
	if (header.id[0] != 'M' && header.id[1] != 'D' && header.id[2] != '2' && header.id[3] != '0') {
		wxLogMessage(_T("Error:\t\tInvalid model!  May be corrupted."));
		ok = false;
		f.close();
		return;
	}

#ifdef WotLK
	modelname = tempname;

	if (header.nameOfs != 304 && header.nameOfs != 320) {
		wxLogMessage(_T("Error:\t\tInvalid model nameOfs=%d/%d!  May be corrupted."), header.nameOfs, sizeof(ModelHeader));
		//ok = false;
		//f.close();
		//return;
	}
#else
	if (header.nameOfs != 336) {
		wxLogMessage(_T("Error:\t\tInvalid model nameOfs=%d/%d!  May be corrupted."), header.nameOfs, sizeof(ModelHeader));
		//ok = false;
		//f.close();
		//return;
	}
#endif

	// Error check
	// 9 1 0 0 = WoW 4.0 models
	// 8 1 0 0 = WoW 3.0 models
	// 4 1 0 0 = WoW 2.0 models
	// 0 1 0 0 = WoW 1.0 models
	if (header.version[0] != 4 && header.version[1] != 1 && header.version[2] != 0 && header.version[3] != 0) {
		wxLogMessage(_T("Error:\t\tModel version is incorrect!\n\t\tMake sure you are loading models from World of Warcraft 2.0.1 or newer client."));
		ok = false;
		f.close();

		if (header.version[0] == 0)
			wxMessageBox(wxString::Format(_T("An error occured while trying to load the model %s.\nWoW Model Viewer 0.5.x only supports loading WoW 2.0 models\nModels from WoW 1.12 or earlier are not supported"), tempname.c_str()), _T("Error: Unable to load model"), wxICON_ERROR);

		return;
	}

	if (f.getSize() < header.ofsParticleEmitters) {
		wxLogMessage(_T("Error: Unable to load the Model \"%s\", appears to be corrupted."), tempname.c_str());
	}
	
	if (header.nGlobalSequences) {
		globalSequences = new uint32[header.nGlobalSequences];
		memcpy(globalSequences, (f.getBuffer() + header.ofsGlobalSequences), header.nGlobalSequences * sizeof(uint32));
	}

	if (forceAnim) 
		animBones = true;
	
	if (animated) 
		initAnimated(f);
	else 
		initStatic(f);

	f.close();
	
	// Ready to render.
	showModel = true;
	if (hasParticles)
		showParticles = true;
	alpha = 1.0f;
}

Model::~Model()
{
	if (ok) {
#ifdef _DEBUG
		wxLogMessage(_T("Unloading model: %s\n"), name.c_str());
#endif

		// There is a small memory leak somewhere with the textures.
		// Especially if the texture was built into the model.
		// No matter what I try though I can't find the memory to unload.
		if (header.nTextures) {

			// For character models, the texture isn't loaded into the texture manager, manually remove it
			glDeleteTextures(1, &replaceTextures[1]);
			
			// Clears textures that were loaded from Model::InitCommon()
			for (size_t i=0; i<header.nTextures; i++) {
				if (textures[i]>0)
					texturemanager.del(textures[i]);
			}

			for (size_t i=0; i<TEXTURE_MAX; i++) {
				if (replaceTextures[i] > 0)
					texturemanager.del(replaceTextures[i]);
			}

			
			
			wxDELETEA(textures);
		}

		wxDELETEA(globalSequences);

		wxDELETEA(bounds);
		wxDELETEA(boundTris);
		wxDELETEA(showGeosets);

		wxDELETE(animManager);

		if (animated) {
			// unload all sorts of crap
			// Need this if statement because VBO supported
			// cards have already deleted it.
			if(video.supportVBO) {
				glDeleteBuffersARB(1, &nbuf);
				glDeleteBuffersARB(1, &vbuf);
				glDeleteBuffersARB(1, &tbuf);

				vertices = NULL;
			}

			wxDELETEA(normals);
			wxDELETEA(vertices);
			wxDELETEA(texCoords);

			wxDELETEA(indices);
			wxDELETEA(anims);
			wxDELETEA(animLookups);
			wxDELETEA(origVertices);

			wxDELETEA(bones);
			wxDELETEA(texAnims);
			wxDELETEA(colors);
			wxDELETEA(transparency);
			wxDELETEA(lights);
			wxDELETEA(events);
			wxDELETEA(particleSystems);
			wxDELETEA(ribbons);

		} else {
			glDeleteLists(dlist, 1);
		}
		if (g_modelViewer)
			g_modelViewer->modelOpened->Clear();
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
				if (bb.translation.type || bb.rotation.type || bb.scaling.type || (bb.flags&MODELBONE_BILLBOARD)) {
					if (bb.flags&MODELBONE_BILLBOARD) {
						// if we have billboarding, the model will need per-instance animation
						ind = true;
					}
					animGeometry = true;
					break;
				}
			}
		}
	}

	if (animGeometry) 
		animBones = true;
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

	if (animMisc) 
		animBones = true;

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

	// This data is needed for both VBO and non-VBO cards.
	vertices = new Vec3D[header.nVertices];
	normals = new Vec3D[header.nVertices];

	// Correct the data from the model, so that its using the Y-Up axis mode.
	for (size_t i=0; i<header.nVertices; i++) {
		origVertices[i].pos = fixCoordSystem(origVertices[i].pos);
		origVertices[i].normal = fixCoordSystem(origVertices[i].normal);

		// Set the data for our vertices, normals from the model data
		//if (!animGeometry || !supportVBO) {
			vertices[i] = origVertices[i].pos;
			normals[i] = origVertices[i].normal.normalize();
		//}

		float len = origVertices[i].pos.lengthSquared();
		if (len > rad){ 
			rad = len;
		}
	}

	// model vertex radius
	rad = sqrtf(rad);

	// bounds
	if (header.nBoundingVertices > 0) {
		bounds = new Vec3D[header.nBoundingVertices];
		Vec3D *b = (Vec3D*)(f.getBuffer() + header.ofsBoundingVertices);
		for (size_t i=0; i<header.nBoundingVertices; i++) {
			bounds[i] = fixCoordSystem(b[i]);
		}
	}
	if (header.nBoundingTriangles > 0) {
		boundTris = new uint16[header.nBoundingTriangles];
		memcpy(boundTris, f.getBuffer() + header.ofsBoundingTriangles, header.nBoundingTriangles*sizeof(uint16));
	}

	// textures
	ModelTextureDef *texdef = (ModelTextureDef*)(f.getBuffer() + header.ofsTextures);
	if (header.nTextures) {
		textures = new TextureID[header.nTextures];
		for (size_t i=0; i<header.nTextures; i++) {
			// Error check
			if (i > TEXTURE_MAX-1) {
				wxLogMessage(_T("Critical Error: Model Texture %d over %d"), header.nTextures, TEXTURE_MAX);
				break;
			}
			/*
			Texture Types
			Texture type is 0 for regular textures, nonzero for skinned textures (filename not referenced in the M2 file!) 
			For instance, in the NightElfFemale model, her eye glow is a type 0 texture and has a file name, 
			the other 3 textures have types of 1, 2 and 6. The texture filenames for these come from client database files:

			DBFilesClient\CharSections.dbc
			DBFilesClient\CreatureDisplayInfo.dbc
			DBFilesClient\ItemDisplayInfo.dbc
			(possibly more)
				
			0	 Texture given in filename
			1	 Body + clothes
			2	Cape
			6	Hair, beard
			8	Tauren fur
			11	Skin for creatures #1
			12	Skin for creatures #2
			13	Skin for creatures #3

			Texture Flags
			Value	 Meaning
			1	Texture wrap X
			2	Texture wrap Y
			*/

			char texname[256] = "";
			if (texdef[i].type == TEXTURE_FILENAME) {
				strncpy(texname, (const char*)f.getBuffer() + texdef[i].nameOfs, texdef[i].nameLen);
				texname[texdef[i].nameLen] = 0;
				textures[i] = texturemanager.add(texname);
				TextureList.push_back(texname);
				wxLogMessage(_T("Info: Added %s to the TextureList."), texname);
			} else {
				// special texture - only on characters and such...
				textures[i] = 0;
				//while (texdef[i].type < TEXTURE_MAX && specialTextures[texdef[i].type]!=-1) texdef[i].type++;
				//if (texdef[i].type < TEXTURE_MAX)specialTextures[texdef[i].type] = (int)i;
				specialTextures[i] = texdef[i].type;

				if (modelType == MT_NORMAL){
					wxString tex;
					if (texdef[i].type == TEXTURE_HAIR){
						tex = _T("Hair.blp");
					}else if(texdef[i].type == TEXTURE_BODY){
						tex = _T("Body.blp");
					}else if(texdef[i].type == TEXTURE_CAPE){
						tex = _T("Cape.blp");
					}else if(texdef[i].type == TEXTURE_FUR){
						tex = _T("Fur.blp");
					}else if(texdef[i].type == TEXTURE_ARMORREFLECT){
						tex = _T("Reflection.blp");
					}else if(texdef[i].type == TEXTURE_GAMEOBJECT1){
						tex = _T("ChangableTexture1.blp");
					}else if(texdef[i].type == TEXTURE_GAMEOBJECT2){
						tex = _T("ChangableTexture2.blp");
					}else if(texdef[i].type == TEXTURE_GAMEOBJECT3){
						tex = _T("ChangableTexture3.blp");
					}
					strncpy(texname,(const char*)(tex.c_str()),tex.Len());
					wxLogMessage(_T("Info: Added %s to the TextureList via specialTextures."), texname);
					TextureList.push_back(texname);
				}else{
					wxString tex;
					if (texdef[i].type == TEXTURE_HAIR){
						tex = _T("NHair.blp");
					}else if(texdef[i].type == TEXTURE_BODY){
						tex = _T("NBody.blp");
					}else if(texdef[i].type == TEXTURE_CAPE){
						tex = _T("NCape.blp");
					}else if(texdef[i].type == TEXTURE_FUR){
						tex = _T("NFur");
					}else if(texdef[i].type == TEXTURE_ARMORREFLECT){
						tex = _T("NReflection.blp");
					}else if(texdef[i].type == TEXTURE_GAMEOBJECT1){
						tex = _T("NChangableTexture1.blp");
					}else if(texdef[i].type == TEXTURE_GAMEOBJECT2){
						tex = _T("NChangableTexture2.blp");
					}else if(texdef[i].type == TEXTURE_GAMEOBJECT3){
						tex = _T("NChangableTexture3.blp");
					}
					strncpy(texname,(const char*)(tex.c_str()),tex.Len());
					wxLogMessage(_T("Info: Added %s to the TextureList via specialTextures."), texname);
					TextureList.push_back(texname);
				}

				if (texdef[i].type < TEXTURE_MAX)
					useReplaceTextures[texdef[i].type] = true;

				if (texdef[i].type == TEXTURE_ARMORREFLECT) {
					// a fix for weapons with type-3 textures.
					replaceTextures[texdef[i].type] = texturemanager.add("Item\\ObjectComponents\\Weapon\\ArmorReflect4.BLP");
				}
			}
		}
	}
	/*
	// replacable textures - it seems to be better to get this info from the texture types
	if (header.nTexReplace) {
		size_t m = header.nTexReplace;
		if (m>16) m = 16;
		int16 *texrep = (int16*)(f.getBuffer() + header.ofsTexReplace);
		for (size_t i=0; i<m; i++) specialTextures[i] = texrep[i];
	}
	*/

	// attachments
	// debug code here
	if (header.nAttachments) {
		ModelAttachmentDef *attachments = (ModelAttachmentDef*)(f.getBuffer() + header.ofsAttachments);
		for (size_t i=0; i<header.nAttachments; i++) {
			ModelAttachment att;
			att.model = this;
			att.init(f, attachments[i], globalSequences);
			atts.push_back(att);
		}
	}

	if (header.nAttachLookup) {
		int16 *p = (int16*)(f.getBuffer() + header.ofsAttachLookup);
		if (header.nAttachLookup > ATT_MAX)
			wxLogMessage(_T("Critical Error: Model AttachLookup %d over %d"), header.nAttachLookup, ATT_MAX);
		for (size_t i=0; i<header.nAttachLookup; i++) {
			if (i>ATT_MAX-1)
				break;
			attLookup[i] = p[i];
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

	if (header.nViews) {
		// just use the first LOD/view
		// First LOD/View being the worst?
		// TODO: Add support for selecting the LOD.
		// indices - allocate space, too
		// header.nViews;
		// int viewLOD = 0; // sets LOD to worst
		// int viewLOD = header.nViews - 1; // sets LOD to best
		//setLOD(f, header.nViews - 1); // Set the default Level of Detail to the best possible. 

		// Old method - use this to try to determine a bug.
		// just use the first LOD/view

		// indices - allocate space, too
#ifdef WotLK
		// remove suffix .M2
		lodname = modelname.BeforeLast(_T('.'));
		fullname = lodname;
		lodname.Append(_T("00.skin")); // Lods: 00, 01, 02, 03
		MPQFile g((char *)lodname.c_str());
		g_modelViewer->modelOpened->Add(lodname);
		if (g.isEof()) {
			wxLogMessage(_T("Error: Unable to load Lods: [%s]"), lodname.c_str());
			g.close();
			return;
		}
		
		ModelView *view = (ModelView*)(g.getBuffer());

		if (view->id[0] != 'S' || view->id[1] != 'K' || view->id[2] != 'I' || view->id[3] != 'N') {
			wxLogMessage(_T("Error: Unable to load Lods: [%s]"), lodname.c_str());
			g.close();
			return;
		}

		// Indices,  Triangles
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
#else // not WotLK
		ModelView *view = (ModelView*)(f.getBuffer() + header.ofsViews);

		// Indices,  Triangles
		uint16 *indexLookup = (uint16*)(f.getBuffer() + view->ofsIndex);
		uint16 *triangles = (uint16*)(f.getBuffer() + view->ofsTris);
		nIndices = view->nTris;
		indices = new uint16[nIndices];
		for (size_t i = 0; i<nIndices; i++) {
	        indices[i] = indexLookup[triangles[i]];
		}

		// render ops
		ModelGeoset *ops = (ModelGeoset*)(f.getBuffer() + view->ofsSub);
		ModelTexUnit *tex = (ModelTexUnit*)(f.getBuffer() + view->ofsTex);
#endif // WotLK

		ModelRenderFlags *renderFlags = (ModelRenderFlags*)(f.getBuffer() + header.ofsTexFlags);
		uint16 *texlookup = (uint16*)(f.getBuffer() + header.ofsTexLookup);
		uint16 *texanimlookup = (uint16*)(f.getBuffer() + header.ofsTexAnimLookup);
		int16 *texunitlookup = (int16*)(f.getBuffer() + header.ofsTexUnitLookup);

		
		showGeosets = new bool[view->nSub];
		for (size_t i=0; i<view->nSub; i++) {
			geosets.push_back(ops[i]);
			showGeosets[i] = true;
		}

		for (size_t j = 0; j<view->nTex; j++) {
			ModelRenderPass pass;

			pass.useTex2 = false;
			pass.useEnvMap = false;
			pass.cull = false;
			pass.trans = false;
			pass.unlit = false;
			pass.noZWrite = false;
			pass.billboard = false;

			//pass.texture2 = 0;
			size_t geoset = tex[j].op;
			
			pass.geoset = (int)geoset;

			pass.indexStart = ops[geoset].istart;
			pass.indexCount = ops[geoset].icount;
			pass.vertexStart = ops[geoset].vstart;
			pass.vertexEnd = pass.vertexStart + ops[geoset].vcount;
			
			pass.order = tex[j].shading; //pass.order = 0;
			
			//TextureID texid = textures[texlookup[tex[j].textureid]];
			//pass.texture = texid;
			pass.tex = texlookup[tex[j].textureid];
			
			/*
			// Render Flags
			flags:
			0x01 = Unlit
			0x02 = ? glow effects ? no zwrite?
			0x04 = Two-sided (no backface culling if set)
			0x08 = (probably billboarded)
			0x10 = Disable z-buffer?

			blend:
			Value	 Mapped to	 Meaning
			0	 	0	 		Combiners_Opaque
			1	 	1	 		Combiners_Mod
			2	 	1	 		Combiners_Decal
			3	 	1	 		Combiners_Add
			4	 	1	 		Combiners_Mod2x
			5	 	4	 		Combiners_Fade
			6	 	4	 		Used in the Deeprun Tram subway glass, supposedly (src=dest_color, dest=src_color) (?)
			*/
			// TODO: figure out these flags properly -_-
			ModelRenderFlags &rf = renderFlags[tex[j].flagsIndex];
			
			pass.blendmode = rf.blend;
			//if (rf.blend == 0) // Test to disable/hide different blend types
			//	continue;

			pass.color = tex[j].colorIndex;
			pass.opacity = transLookup[tex[j].transid];

			pass.unlit = (rf.flags & RENDERFLAGS_UNLIT)!= 0;

			// This is wrong but meh.. best I could get it so far.
			//pass.cull = (rf.flags & 0x04)==0 && pass.blendmode!=1 && pass.blendmode!=4 && (rf.flags & 17)!=17;
			//pass.cull = false; // quick test
			pass.cull = (rf.flags & RENDERFLAGS_TWOSIDED)==0 && rf.blend==0;

			pass.billboard = (rf.flags & RENDERFLAGS_BILLBOARD) != 0;

			pass.useEnvMap = (texunitlookup[tex[j].texunit] == -1) && pass.billboard && rf.blend>2; //&& rf.blend<5; // Use environmental reflection effects?

			// Disable environmental mapping if its been unchecked.
			//if (pass.useEnvMap && !video.useEnvMapping)
			//	pass.useEnvMap = false;


			//pass.noZWrite = (texdef[pass.tex].flags & 3)!=0;
			/*
			if (name == "Creature\\Turkey\\turkey.m2") // manual fix as I just bloody give up.
				pass.noZWrite = false;
			else
				pass.noZWrite = (pass.blendmode>1);
			*/
				//pass.noZWrite = (pass.blendmode>1) && !(rf.blend==4 && rf.flags==17);
			pass.noZWrite = (rf.flags & RENDERFLAGS_ZBUFFERED) != 0;

			// ToDo: Work out the correct way to get the true/false of transparency
			pass.trans = (pass.blendmode>0) && (pass.opacity>0);	// Transparency - not the correct way to get transparency

			pass.p = ops[geoset].BoundingBox[0].z;

			// Texture flags
			pass.swrap = (texdef[pass.tex].flags & TEXTURE_WRAPX) != 0; // Texture wrap X
			pass.twrap = (texdef[pass.tex].flags & TEXTURE_WRAPY) != 0; // Texture wrap Y
			
			if (animTextures) {
				// tex[j].flags: Usually 16 for static textures, and 0 for animated textures.	
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

#ifdef WotLK
		g.close();
#endif
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
	wxDELETEA(vertices);
	wxDELETEA(normals);
	wxDELETEA(indices);

	wxDELETEA(colors);
	wxDELETEA(transparency);
}

void Model::initAnimated(MPQFile &f)
{
	if (origVertices) {
		delete [] origVertices;
		origVertices = NULL;
	}

	origVertices = new ModelVertex[header.nVertices];
	memcpy(origVertices, f.getBuffer() + header.ofsVertices, header.nVertices * sizeof(ModelVertex));

	initCommon(f);

	if (header.nAnimations > 0) {
		anims = new ModelAnimation[header.nAnimations];

		#ifndef WotLK
		memcpy(anims, f.getBuffer() + header.ofsAnimations, header.nAnimations * sizeof(ModelAnimation));
		#else
		ModelAnimationWotLK animsWotLK;
		char tempname[256];
		animfiles = new MPQFile[header.nAnimations];
		for(size_t i=0; i<header.nAnimations; i++) {
			memcpy(&animsWotLK, f.getBuffer() + header.ofsAnimations + i*sizeof(ModelAnimationWotLK), sizeof(ModelAnimationWotLK));
			anims[i].animID = animsWotLK.animID;
			anims[i].timeStart = 0;
			anims[i].timeEnd = animsWotLK.length;
			anims[i].moveSpeed = animsWotLK.moveSpeed;
			anims[i].flags = animsWotLK.flags;
			anims[i].probability = animsWotLK.probability;
			anims[i].d1 = animsWotLK.d1;
			anims[i].d2 = animsWotLK.d2;
			anims[i].playSpeed = animsWotLK.playSpeed;
			anims[i].rad = animsWotLK.rad;
			anims[i].NextAnimation = animsWotLK.NextAnimation;
			anims[i].Index = animsWotLK.Index;

			sprintf(tempname, "%s%04d-%02d.anim", (char *)fullname.c_str(), anims[i].animID, animsWotLK.subAnimID);
			if (MPQFile::getSize(tempname) > 0) {
				animfiles[i].openFile(tempname);
				g_modelViewer->modelOpened->Add(wxString(tempname, wxConvUTF8));
			}
		}
		#endif

		animManager = new AnimManager(anims);
	}
	
	if (animBones) {
		// init bones...
		bones = new Bone[header.nBones];
		ModelBoneDef *mb = (ModelBoneDef*)(f.getBuffer() + header.ofsBones);
		for (size_t i=0; i<header.nBones; i++) {
			//if (i==0) mb[i].rotation.ofsRanges = 1.0f;
#ifdef WotLK
			bones[i].model = this;
			bones[i].init(f, mb[i], globalSequences, animfiles);
#else
			bones[i].init(f, mb[i], globalSequences);
#endif
		}

		// Block keyBoneLookup is a lookup table for Key Skeletal Bones, hands, arms, legs, etc.
		if (header.nKeyBoneLookup < BONE_MAX) {
			memcpy(keyBoneLookup, f.getBuffer() + header.ofsKeyBoneLookup, sizeof(int16)*header.nKeyBoneLookup);
		} else {
			memcpy(keyBoneLookup, f.getBuffer() + header.ofsKeyBoneLookup, sizeof(int16)*BONE_MAX);
			wxLogMessage(_T("Error: keyBone number [%d] over [%d]"), header.nKeyBoneLookup, BONE_MAX);
		}
	}

#ifdef WotLK
	// free MPQFile
	if (header.nAnimations > 0) {
		for(size_t i=0; i<header.nAnimations; i++) {
			if(animfiles[i].getSize() > 0)
				animfiles[i].close();
		}
		delete [] animfiles;
	}
#endif

	// Index at ofsAnimations which represents the animation in AnimationData.dbc. -1 if none.
	if (header.nAnimationLookup > 0) {
		animLookups = new int16[header.nAnimationLookup];
		memcpy(animLookups, f.getBuffer() + header.ofsAnimationLookup, sizeof(int16)*header.nAnimationLookup);
	}
	
	const size_t size = (header.nVertices * sizeof(float));
	vbufsize = (3 * size); // we multiple by 3 for the x, y, z positions of the vertex

	texCoords = new Vec2D[header.nVertices];
	for (size_t i=0; i<header.nVertices; i++) 
		texCoords[i] = origVertices[i].texcoords;

	if (video.supportVBO) {
		// Vert buffer
		glGenBuffersARB(1,&vbuf);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbuf);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, vbufsize, vertices, GL_STATIC_DRAW_ARB);
		wxDELETEA(vertices);
		
		// Texture buffer
		glGenBuffersARB(1,&tbuf);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, tbuf);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, 2*size, texCoords, GL_STATIC_DRAW_ARB);
		wxDELETEA(texCoords);
		
		// normals buffer
		glGenBuffersARB(1,&nbuf);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, nbuf);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, vbufsize, normals, GL_STATIC_DRAW_ARB);
		wxDELETEA(normals);
	}

	if (animTextures) {
		texAnims = new TextureAnim[header.nTexAnims];
		ModelTexAnimDef *ta = (ModelTexAnimDef*)(f.getBuffer() + header.ofsTexAnims);
		for (size_t i=0; i<header.nTexAnims; i++)
			texAnims[i].init(f, ta[i], globalSequences);
	}

	if (header.nEvents) {
		ModelEventDef *edefs = (ModelEventDef *)(f.getBuffer()+header.ofsEvents);
		events = new ModelEvent[header.nEvents];
		for (size_t i=0; i<header.nEvents; i++) {
			events[i].init(f, edefs[i], globalSequences);
		}
	}

	// particle systems
	if (header.nParticleEmitters) {
		ModelParticleEmitterDef *pdefs = (ModelParticleEmitterDef *)(f.getBuffer() + header.ofsParticleEmitters);
		particleSystems = new ParticleSystem[header.nParticleEmitters];
		hasParticles = true;
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
		hasCamera = true;
	}

	// init lights
	if (header.nLights) {
		lights = new ModelLight[header.nLights];
		ModelLightDef *lDefs = (ModelLightDef*)(f.getBuffer() + header.ofsLights);
		for (size_t i=0; i<header.nLights; i++) lights[i].init(f, lDefs[i], globalSequences);
	}


	animcalc = false;
}

void Model::setLOD(MPQFile &f, int index)
{
	/*
	// I thought the view controlled the Level of detail,  but that doesn't seem to be the case.
	// Seems to only control the render order.  Which makes this function useless and not needed :(
	
	// Texture definitions
	ModelTextureDef *texdef = (ModelTextureDef*)(f.getBuffer() + header.ofsTextures);

	// Transparency
	int16 *transLookup = (int16*)(f.getBuffer() + header.ofsTransparencyLookup);

	// Level of Detail View
	ModelView *view = (ModelView*)(f.getBuffer() + header.ofsViews);

	uint16 *indexLookup = (uint16*)(f.getBuffer() + view[index].ofsIndex);
	uint16 *triangles = (uint16*)(f.getBuffer() + view[index].ofsTris);
	nIndices = view->nTris;

	wxDELETEA(indices);

	indices = new uint16[nIndices];
	for (size_t i = 0; i<nIndices; i++) {
        indices[i] = indexLookup[triangles[i]];
	}

	// render ops
	ModelGeoset *ops = (ModelGeoset*)(f.getBuffer() + view[index].ofsSub);
	ModelTexUnit *tex = (ModelTexUnit*)(f.getBuffer() + view[index].ofsTex);
	ModelRenderFlags *renderFlags = (ModelRenderFlags*)(f.getBuffer() + header.ofsTexFlags);
	uint16 *texlookup = (uint16*)(f.getBuffer() + header.ofsTexLookup);
	uint16 *texanimlookup = (uint16*)(f.getBuffer() + header.ofsTexAnimLookup);
	int16 *texunitlookup = (int16*)(f.getBuffer() + header.ofsTexUnitLookup);

	wxDELETEA(showGeosets);

	showGeosets = new bool[view[index].nSub];
	for (size_t i=0; i<view[index].nSub; i++) {
		geosets.push_back(ops[i]);
		showGeosets[i] = true;
	}

	passes.clear();
	for (size_t j=0; j<view[index].nTex; j++) {
		ModelRenderPass pass;

		pass.useTex2 = false;
		pass.useEnvMap = false;
		pass.cull = false;
		pass.Trans = false;
		pass.unlit = false;
		pass.noZWrite = false;
		
		//pass.texture2 = 0;
		size_t geoset = tex[j].op;
		
		pass.geoset = (int)geoset;

		pass.indexStart = ops[geoset].istart;
		pass.indexCount = ops[geoset].icount;
		pass.vertexStart = ops[geoset].vstart;
		pass.vertexEnd = pass.vertexStart + ops[geoset].vcount;

		pass.order = tex[j].order; //pass.order = 0;
		
		//TextureID texid = textures[texlookup[tex[j].textureid]];
		//pass.texture = texid;
		pass.tex = texlookup[tex[j].textureid];
		
		// TODO: figure out these flags properly -_-
		ModelRenderFlags &rf = renderFlags[tex[j].flagsIndex];
		
		pass.blendmode = rf.blend;
		pass.color = tex[j].colorIndex;
		pass.opacity = transLookup[tex[j].transid];

		//if (name == "Creature\\Kelthuzad\\kelthuzad.m2")
			pass.useEnvMap = (texunitlookup[tex[j].texunit] == -1) && ((rf.flags & 0x10) !=0) && rf.blend>2; // Use environmental reflection effects?
		//else
		//	pass.useEnvMap = (texunitlookup[tex[j].texunit] == -1) && ((rf.flags & 0x10) !=0);

		// This is wrong but meh.. best I could get it so far.
		//pass.cull = (rf.flags & 0x04)==0 && pass.blendmode!=1 && pass.blendmode!=4 && (rf.flags & 17)!=17;
		pass.cull = false; // quick test

		pass.unlit = (rf.flags & 0x01)!=0;

		//pass.noZWrite = (texdef[pass.tex].flags & 3)!=0;
		if (name == "Creature\\Turkey\\turkey.m2") // manual fix as I just bloody give up.
			pass.noZWrite = false;
		else
			pass.noZWrite = (pass.blendmode>1) && !(rf.blend==4 && rf.flags==17);

		//pass.noZWrite = false; // quick test

		// ToDo: Work out the correct way to get the true/false of transparency
		pass.Trans = (pass.blendmode>0) && (pass.opacity>0);	// Transparency - not the correct way to get transparency

		pass.p = ops[geoset].v.z;

		pass.swrap = (texdef[pass.tex].flags & 1) != 0;
		pass.twrap = (texdef[pass.tex].flags & 2) != 0;
		

		if (animTextures) {
			//if (tex[j].flags & 16) {
			if (tex[j].flags & 15) {
				pass.texanim = -1; // no texture animation
			} else {
				pass.texanim = texanimlookup[tex[j].texanimid];
			}
		} else {
			pass.texanim = -1; // no texture animation
		}

        passes.push_back(pass);
	}

	// transparent parts come later
	std::sort(passes.begin(), passes.end()); 
	*/
}

void Model::calcBones(int anim, int time)
{
	// Reset all bones to 'false' which means they haven't been animated yet.
	for (size_t i=0; i<header.nBones; i++) {
		bones[i].calc = false;
	}

	// Character specific bone animation calculations.
	if (charModelDetails.isChar) {	

		// Animate the "core" rotations and transformations for the rest of the model to adopt into their transformations
		if (keyBoneLookup[BONE_ROOT] > -1)	{
			for (int i=0; i<=keyBoneLookup[BONE_ROOT]; i++) {
				bones[i].calcMatrix(bones, anim, time);
			}
		}

		// Find the close hands animation id
		int closeFistID = 0;
		/*
		for (unsigned int i=0; i<header.nAnimations; i++) {
			if (anims[i].animID==15) {  // closed fist
				closeFistID = i;
				break;
			}
		}
		*/
		// Alfred 2009.07.23 use animLookups to speedup
		if (header.nAnimationLookup >= ANIMATION_HANDSCLOSED && animLookups[ANIMATION_HANDSCLOSED] > 0) // closed fist
			closeFistID = animLookups[ANIMATION_HANDSCLOSED];

		// Animate key skeletal bones except the fingers which we do later.
		// -----
		int a, t;

		// if we have a "secondary animation" selected,  animate upper body using that.
		if (animManager->GetSecondaryID() > -1) {
			a = animManager->GetSecondaryID();
			t = animManager->GetSecondaryFrame();
		} else {
			a = anim;
			t = time;
		}

		for (size_t i=0; i<5; i++) { // only goto 5, otherwise it affects the hip/waist rotation for the lower-body.
			if (keyBoneLookup[i] > -1)
				bones[keyBoneLookup[i]].calcMatrix(bones, a, t);
		}

		if (animManager->GetMouthID() > -1) {
			// Animate the head and jaw
			if (keyBoneLookup[BONE_HEAD] > -1)
					bones[keyBoneLookup[BONE_HEAD]].calcMatrix(bones, animManager->GetMouthID(), animManager->GetMouthFrame());
			if (keyBoneLookup[BONE_JAW] > -1)
					bones[keyBoneLookup[BONE_JAW]].calcMatrix(bones, animManager->GetMouthID(), animManager->GetMouthFrame());
		} else {
			// Animate the head and jaw
			if (keyBoneLookup[BONE_HEAD] > -1)
					bones[keyBoneLookup[BONE_HEAD]].calcMatrix(bones, a, t);
			if (keyBoneLookup[BONE_JAW] > -1)
					bones[keyBoneLookup[BONE_JAW]].calcMatrix(bones, a, t);
		}

		// still not sure what 18-26 bone lookups are but I think its more for things like wrist, etc which are not as visually obvious.
		for (size_t i=18; i<BONE_MAX; i++) {
			if (keyBoneLookup[i] > -1)
				bones[keyBoneLookup[i]].calcMatrix(bones, a, t);
		}
		// =====

		
		
		if (charModelDetails.closeRHand) {
			a = closeFistID;
			t = anims[closeFistID].timeStart+1;
		} else {
			a = anim;
			t = time;
		}

		for (unsigned int i=0; i<5; i++) {
			if (keyBoneLookup[BONE_RFINGER1 + i] > -1) 
				bones[keyBoneLookup[BONE_RFINGER1 + i]].calcMatrix(bones, a, t);
		}

		if (charModelDetails.closeLHand) {
			a = closeFistID;
			t = anims[closeFistID].timeStart+1;
		} else {
			a = anim;
			t = time;
		}

		for (unsigned int i=0; i<5; i++) {
			if (keyBoneLookup[BONE_LFINGER1 + i] > -1)
				bones[keyBoneLookup[BONE_LFINGER1 + i]].calcMatrix(bones, a, t);
		}
	} else {
		for (int i=0; i<keyBoneLookup[BONE_ROOT]; i++) {
			bones[i].calcMatrix(bones, anim, time);
		}

		// The following line fixes 'mounts' in that the character doesn't get rotated, but it also screws up the rotation for the entire model :(
		//bones[18].calcMatrix(bones, anim, time, false);

	}

	// Animate everything thats left with the 'default' animation
	for (size_t i=0; i<header.nBones; i++) {
		bones[i].calcMatrix(bones, anim, time);
	}
}

void Model::animate(int anim)
{
	int t=0;
	
	ModelAnimation &a = anims[anim];
	int tmax = (a.timeEnd-a.timeStart);
	if (tmax==0) 
		tmax = 1;

	if (isWMO == true) {
		t = globalTime;
		t %= tmax;
		t += a.timeStart;
	} else
		t = animManager->GetFrame();
	
	this->animtime = t;
	this->anim = anim;

	if (animBones) // && (!animManager->IsPaused() || !animManager->IsParticlePaused()))
		calcBones(anim, t);

	if (animGeometry) {

		if (video.supportVBO)	{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbuf);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, 2*vbufsize, NULL, GL_STREAM_DRAW_ARB);
			vertices = (Vec3D*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY);

			// Something has been changed in the past couple of days that is causing nasty bugs
			// this is an extra error check to prevent the program from crashing.
			if (!vertices) {
				wxLogMessage(_T("Critical Error: void Model::animate(int anim), Vertex Buffer is null"));
				return;
			}
		}

		// transform vertices
		ModelVertex *ov = origVertices;
		for (size_t i=0; i<header.nVertices; ++i,++ov) { //,k=0
			Vec3D v(0,0,0), n(0,0,0);

			for (size_t b=0; b<4; b++) {
				if (ov->weights[b]>0) {
					Vec3D tv = bones[ov->bones[b]].mat * ov->pos;
					Vec3D tn = bones[ov->bones[b]].mrot * ov->normal;
					v += tv * ((float)ov->weights[b] / 255.0f);
					n += tn * ((float)ov->weights[b] / 255.0f);
				}
			}

			vertices[i] = v;
			if (video.supportVBO)
				vertices[header.nVertices + i] = n.normalize(); // shouldn't these be normal by default?
			else
				normals[i] = n;
		}

        if (video.supportVBO) 
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
		int pt = a.timeStart + (t + (int)(tmax*particleSystems[i].tofs)) % tmax;
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
		if (color!=-1 && m->colors && m->colors[color].color.uses(0)) {
#ifdef WotLK /* Alfred 2008.10.02 buggy opacity make model invisable, TODO */
			Vec3D c = m->colors[color].color.getValue(0,m->animtime);
			if (m->colors[color].opacity.uses(m->anim)) {
				float o = m->colors[color].opacity.getValue(m->anim,m->animtime);
				ocol.w = o;
			}
#else
			Vec3D c = m->colors[color].color.getValue(m->anim,m->animtime);
			float o = m->colors[color].opacity.getValue(m->anim,m->animtime);
			ocol.w = o;
#endif

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
#ifdef WotLK /* Alfred 2008.10.02 buggy opacity make model invisable, TODO */
			if (m->transparency && m->transparency[opacity].trans.uses(0))
				ocol.w *= m->transparency[opacity].trans.getValue(0, m->animtime);
#else
			ocol.w *= m->transparency[opacity].trans.getValue(m->anim, m->animtime);
#endif
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

		// ALPHA BLENDING
		// blend mode
		switch (blendmode) {
		case BM_OPAQUE:	// 0			
			break;
		case BM_TRANSPARENT: // 1
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GEQUAL,0.7f);
			
			/*
			// Tex settings
			glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE,1.000000);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA,GL_CONSTANT);
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
			*/
			
			break;
		case BM_ALPHA_BLEND: // 2
			//glEnable(GL_ALPHA_TEST);
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

			/*
			glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE,1.000000);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA,GL_CONSTANT);
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
			*/
			
			break;
		case BM_MODULATE:	// 5
 			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
			
			/*
			// Texture settings.
			glTexEnvf(GL_TEXTURE_ENV,GL_RGB_SCALE,1.000000);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB,GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_INTERPOLATE);
			glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE,1.000000);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA,GL_CONSTANT);
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
			*/

			break;
		case BM_MODULATEX2:	// 6, not sure if this is right
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);	
			
			/*
			// Texture settings.
			glTexEnvf(GL_TEXTURE_ENV,GL_RGB_SCALE,1.000000);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB,GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_INTERPOLATE);
			glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE,1.000000);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA,GL_CONSTANT);
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
			*/
			
			break;
		default:
			wxLogMessage(_T("[Error] Unknown blendmode: %d\n"), blendmode);
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
		glDisable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GEQUAL,0.04f);

		/*
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
		glTexEnvf(GL_TEXTURE_ENV,GL_RGB_SCALE,1.000000);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB,GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
		glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE,1.000000);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA,GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
		*/

		break;
	
	case BM_ALPHA_BLEND:
		glDisable(GL_BLEND);
		break;
	case BM_ADDITIVE: // 3
 		glDisable(GL_BLEND);
		break;
	case BM_ADDITIVE_ALPHA: // 4
		//glDisable(GL_ALPHA_TEST);
 		glDisable(GL_BLEND);
		break;
	
	case BM_MODULATE: // 5
		glDisable(GL_BLEND);
		break;

	case BM_MODULATEX2: // 6
		glDisable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	default:
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // default blend func

		/*
		// Default texture settings.
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
		glTexEnvf(GL_TEXTURE_ENV,GL_RGB_SCALE, 1.000000);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB, GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB, GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB, GL_MODULATE);
		glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE, 1.000000);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA, GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA, GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA, GL_REPLACE);
		*/
	}


	if (noZWrite)
		glDepthMask(GL_TRUE);

	if (texanim!=-1) {
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

	if (unlit)
		glEnable(GL_LIGHTING);

	//if (billboard)
	//	glPopMatrix();

	if (cull)
		glDisable(GL_CULL_FACE);

	if (useEnvMap) {
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
	}

	if (swrap)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	if (twrap)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/*
	if (useTex2) {
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0);
	}
	*/

	if (opacity!=-1 || color!=-1) {
		GLfloat czero[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		glMaterialfv(GL_FRONT, GL_EMISSION, czero);
		
		//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		//glMaterialfv(GL_FRONT, GL_AMBIENT, ocol);
		//ocol = Vec4D(1.0f, 1.0f, 1.0f, 1.0f);
		//glMaterialfv(GL_FRONT, GL_DIFFUSE, ocol);
	}
}


inline void Model::drawModel()
{
	// assume these client states are enabled: GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_TEXTURE_COORD_ARRAY
	if (video.supportVBO && animated)	{
		// bind / point to the vertex normals buffer
		if (animGeometry) {
			glNormalPointer(GL_FLOAT, 0, GL_BUFFER_OFFSET(vbufsize));
		} else {
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, nbuf);
			glNormalPointer(GL_FLOAT, 0, 0);
		}

		// Bind the vertex buffer
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbuf);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		// Bind the texture coordinates buffer
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, tbuf);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
		
	} else if (animated) {
		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glNormalPointer(GL_FLOAT, 0, normals);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	}
	
	// Display in wireframe mode?
	if (showWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// Render the various parts of the model.
	for (size_t i=0; i<passes.size(); i++) {
		ModelRenderPass &p = passes[i];

		if (p.init(this)) {
			// we don't want to render completely transparent parts

			// render
			if (animated) {
				//glDrawElements(GL_TRIANGLES, p.indexCount, GL_UNSIGNED_SHORT, indices + p.indexStart);
				// a GDC OpenGL Performace Tuning paper recommended glDrawRangeElements over glDrawElements
				// I can't notice a difference but I guess it can't hurt
				if (video.supportVBO && video.supportDrawRangeElements) {
					glDrawRangeElements(GL_TRIANGLES, p.vertexStart, p.vertexEnd, p.indexCount, GL_UNSIGNED_SHORT, indices + p.indexStart);
				
				//} else if (!video.supportVBO) {
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
	
	if (showWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// done with all render ops
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

void ModelCamera::init(MPQFile &f, ModelCameraDef &mcd, uint32 *global)
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
	gluPerspective(fov * 34.5f, (GLfloat)video.xRes/(GLfloat)video.yRes, nearclip, farclip*5);

	Vec3D p = pos + tPos.getValue(0, time);
	Vec3D t = target + tTarget.getValue(0, time);

	Vec3D u(0,1,0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(p.x, p.y, p.z, t.x, t.y, t.z, u.x, u.y, u.z);
	//float roll = rot.getValue(0, time) / PI * 180.0f;
	//glRotatef(roll, 0, 0, 1);
}

void ModelColor::init(MPQFile &f, ModelColorDef &mcd, uint32 *global)
{
	color.init(mcd.color, f, global);
	opacity.init(mcd.opacity, f, global);
}

void ModelTransparency::init(MPQFile &f, ModelTransDef &mcd, uint32 *global)
{
	trans.init(mcd.trans, f, global);
}

void ModelLight::init(MPQFile &f, ModelLightDef &mld, uint32 *global)
{
	tpos = pos = fixCoordSystem(mld.pos);
	tdir = dir = Vec3D(0,1,0); // no idea
	type = mld.type;
	parent = mld.bone;
	ambColor.init(mld.ambientColor, f, global);
	ambIntensity.init(mld.ambientIntensity, f, global);
	diffColor.init(mld.diffuseColor, f, global);
	diffIntensity.init(mld.diffuseIntensity, f, global);
	AttenStart.init(mld.attenuationStart, f, global);
	AttenEnd.init(mld.attenuationEnd, f, global);
	UseAttenuation.init(mld.useAttenuation, f, global);
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
		wxLogMessage(_T("Error: Light type %d is unknown."), type);
	}
	//gLog("Light %d (%f,%f,%f) (%f,%f,%f) [%f,%f,%f]\n", l-GL_LIGHT4, ambcol.x, ambcol.y, ambcol.z, diffcol.x, diffcol.y, diffcol.z, p.x, p.y, p.z);
	glLightfv(l, GL_POSITION, p);
	glLightfv(l, GL_DIFFUSE, diffcol);
	glLightfv(l, GL_AMBIENT, ambcol);
	glEnable(l);
}

void TextureAnim::init(MPQFile &f, ModelTexAnimDef &mta, uint32 *global)
{
	trans.init(mta.trans, f, global);
	rot.init(mta.rot, f, global);
	scale.init(mta.scale, f, global);
}

#ifdef WotLK
void Bone::init(MPQFile &f, ModelBoneDef &b, uint32 *global, MPQFile *animfiles)
{
	calc = false;

	parent = b.parent;
	pivot = fixCoordSystem(b.pivot);
	billboard = (b.flags & MODELBONE_BILLBOARD) != 0;
	//billboard = false;

	boneDef = b;
	
	trans.init(b.translation, f, global, animfiles);
	rot.init(b.rotation, f, global, animfiles);
	scale.init(b.scaling, f, global, animfiles);
	trans.fix(fixCoordSystem);
	rot.fix(fixCoordSystemQuat);
	scale.fix(fixCoordSystem2);
}
#else
void Bone::init(MPQFile &f, ModelBoneDef &b, uint32 *global)
{
	calc = false;

	parent = b.parent;
	pivot = fixCoordSystem(b.pivot);
	billboard = (b.flags & MODELBONE_BILLBOARD) != 0;
	//billboard = false;

	boneDef = b;
	
	trans.init(b.translation, f, global);
	rot.init(b.rotation, f, global);
	scale.init(b.scaling, f, global);
	trans.fix(fixCoordSystem);
	rot.fix(fixCoordSystemQuat);
	scale.fix(fixCoordSystem2);
}
#endif

void ModelAttachment::init(MPQFile &f, ModelAttachmentDef &mad, uint32 *global)
{
	pos = fixCoordSystem(mad.pos);
	bone = mad.bone;
	id = mad.id;
}

void ModelAttachment::setup()
{
	Matrix m = model->bones[bone].mat;
	m.transpose();
	glMultMatrixf(m);
	glTranslatef(pos.x, pos.y, pos.z);
}

void ModelAttachment::setupParticle()
{
	Matrix m = model->bones[bone].mat;
	m.transpose();
	glMultMatrixf(m);
	glTranslatef(pos.x, pos.y, pos.z);
}

void Bone::calcMatrix(Bone *allbones, int anim, int time, bool rotate)
{
	if (calc)
		return;

	Matrix m;
	Quaternion q;

	bool tr = rot.uses(anim) || scale.uses(anim) || trans.uses(anim) || billboard;
	if (tr) {
		m.translation(pivot);
		
		if (trans.uses(anim)) {
			Vec3D tr = trans.getValue(anim, time);
			m *= Matrix::newTranslation(tr);
		}

		if (rot.uses(anim) && rotate) {
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

	if (parent > -1) {
		allbones[parent].calcMatrix(allbones, anim, time, rotate);
		mat = allbones[parent].mat * m;
	} else mat = m;

	// transform matrix for normal vectors ... ??
	if (rot.uses(anim) && rotate) {
		if (parent>=0)
			mrot = allbones[parent].mrot * Matrix::newQuatRotate(q);
		else
			mrot = Matrix::newQuatRotate(q);
	} else mrot.unit();

	transPivot = mat * pivot;

	calc = true;
}


inline void Model::draw()
{
	if (!ok)
		return;

	if (!animated) {
		if(showModel)
			glCallList(dlist);

	} else {
		if (ind) {
			animate(currentAnim);
		} else {
			if (!animcalc) {
				animate(currentAnim);
				//animcalc = true; // Not sure what this is really for but it breaks WMO animation
			}
		}

		if(showModel)
			drawModel();
	}
}

// These aren't really needed in the model viewer.. only wowmapviewer
void Model::lightsOn(GLuint lbase)
{
	// setup lights
	for (unsigned int i=0, l=lbase; i<header.nLights; i++) 
		lights[i].setup(animtime, l++);
}

// These aren't really needed in the model viewer.. only wowmapviewer
void Model::lightsOff(GLuint lbase)
{
	for (unsigned int i=0, l=lbase; i<header.nLights; i++) 
		glDisable(l++);
}

// Updates our particles within models.
void Model::updateEmitters(float dt)
{
	if (!ok || !showParticles || !bShowParticle) 
		return;

	for (size_t i=0; i<header.nParticleEmitters; i++) {
		particleSystems[i].update(dt);
	}
}


// Draws the "bones" of models  (skeletal animation)
void Model::drawBones()
{
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	for (size_t i=0; i<header.nBones; i++) {
	//for (size_t i=30; i<40; i++) {
		if (bones[i].parent != -1) {
			glVertex3fv(bones[i].transPivot);
			glVertex3fv(bones[bones[i].parent].transPivot);
		}
	}
	glEnd();
	glEnable(GL_DEPTH_TEST);
}

// Sets up the models attachments
void Model::setupAtt(int id)
{
	int l = attLookup[id];
	if (l>-1)
		atts[l].setup();
}

// Sets up the models attachments
void Model::setupAtt2(int id)
{
	int l = attLookup[id];
	if (l>=0)
		atts[l].setupParticle();
}

// Draws the Bounding Volume, which is used for Collision detection.
void Model::drawBoundingVolume()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLES);
	for (size_t i=0; i<header.nBoundingTriangles; i++) {
		size_t v = boundTris[i];
		if (v < header.nBoundingVertices)
			glVertex3fv(bounds[v]);
		else 
			glVertex3f(0,0,0);
	}
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// Renders our particles into the pipeline.
void Model::drawParticles()
{
	// draw particle systems
	for (size_t i=0; i<header.nParticleEmitters; i++) {
		if (particleSystems != NULL)
			particleSystems[i].draw();
	}

	// draw ribbons
	for (size_t i=0; i<header.nRibbonEmitters; i++) {
		if (ribbons != NULL)
			ribbons[i].draw();
	}
}

// Adds models to the model manager, used by WMO's
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

// Resets the animation back to default.
void ModelManager::resetAnim()
{
	for (std::map<int, ManagedItem*>::iterator it = items.begin(); it != items.end(); ++it) {
		((Model*)it->second)->animcalc = false;
	}
}

// same as other updateEmitter except does it for the all the models being managed - for WMO's
void ModelManager::updateEmitters(float dt)
{
	for (std::map<int, ManagedItem*>::iterator it = items.begin(); it != items.end(); ++it) {
		((Model*)it->second)->updateEmitters(dt);
	}
}

void ModelManager::clear()
{
	for (std::map<int, ManagedItem*>::iterator it = items.begin(); it != items.end(); ++it) {
		doDelete(it->first);
		delete it->second;
	}
	items.clear();
	names.clear();
}

void ModelEvent::init(MPQFile &f, ModelEventDef &me, uint32 *globals)
{
	def = me;
}

