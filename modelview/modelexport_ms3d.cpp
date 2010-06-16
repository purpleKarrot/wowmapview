#include <wx/wfstream.h>

#include "globalvars.h"
#include "modelexport.h"
#include "modelexport_ms3d.h"
#include "modelcanvas.h"

//#include "CxImage/ximage.h"

// MilkShape 3D
void ExportM2toMS3D(Attachment *att, Model *m, const char *fn, bool init)
{
	wxFFileOutputStream f(wxString(fn, wxConvUTF8), wxT("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}
	LogExportData(_T("MS3D"),wxString(fn, wxConvUTF8).BeforeLast(SLASH),_T("M2"));
	unsigned short numVerts = 0;
	unsigned short numFaces = 0;
	unsigned short numGroups = 0;
	ModelData *verts = NULL;
	GroupData *groups = NULL;

	InitCommon(att, init, verts, groups, numVerts, numGroups, numFaces);
	//wxLogMessage(_T("Num Verts: %i, Num Faces: %i, Num Groups: %i"), numVerts, numFaces, numGroups);
	//wxLogMessage(_T("Vert[0] BoneID: %i, Group[0].m.name = %s"),verts[0].boneid, groups[0].m->name);
	wxLogMessage(_T("Init Common Complete."));

	// Write the header
	ms3d_header_t header;
	strncpy(header.id, "MS3D000000", sizeof(header.id));
	header.version = 4;

	// Header
	f.Write(reinterpret_cast<char *>(&header), sizeof(ms3d_header_t));
	wxLogMessage(_T("Header Data Written."));
	// Vertex Count
	f.Write(reinterpret_cast<char *>(&numVerts), sizeof(numVerts));
	//wxLogMessage(_T("NumVerts: %i"),numVerts);
	
	// Write Vertex data?
	for (unsigned int i=0; i<numVerts; i++) {
		ms3d_vertex_t vert;
		vert.boneId = verts[i].boneid;
		vert.flags = 0; //SELECTED;
		vert.referenceCount = 0; // what the?
		vert.vertex[0] = verts[i].vertex.x;
		vert.vertex[1] = verts[i].vertex.y;
		vert.vertex[2] = verts[i].vertex.z;
		f.Write(reinterpret_cast<char *>(&vert), sizeof(ms3d_vertex_t));
	}
	wxLogMessage(_T("Vertex Data Written."));
	// ---------------------------

	// Triangle Count
	f.Write(reinterpret_cast<char *>(&numFaces), sizeof(numFaces));
	//wxLogMessage(_T("NumFaces: %i"),numFaces);

	// Write Triangle Data?
	for (unsigned int i=0; i<(unsigned int)numVerts; i+=3) {
		ms3d_triangle_t tri;
		tri.flags = 0; //SELECTED;
		tri.groupIndex = (unsigned char)verts[i].groupIndex;
		tri.smoothingGroup = 1; // 1 - 32

		for (int j=0; j<3; j++) {
			tri.vertexIndices[j] = i+j;
			tri.s[j] = verts[i+j].tu;
			tri.t[j] = verts[i+j].tv;
			
			tri.vertexNormals[j][0] = verts[i+j].normal.x;
			tri.vertexNormals[j][1] = verts[i+j].normal.y;
			tri.vertexNormals[j][2] = verts[i+j].normal.z;
		}

		f.Write(reinterpret_cast<char *>(&tri), sizeof(ms3d_triangle_t));
	}
	wxLogMessage(_T("Triangle Data Written."));
	// ---------------------------

	// Number of groups
	f.Write(reinterpret_cast<char *>(&numGroups), sizeof(numGroups));
	//wxLogMessage(_T("NumGroups: %i"),numGroups);

	unsigned short indiceCount = 0;
	for (unsigned short i=0; i<(unsigned int)numGroups; i++) {
		wxString groupName(wxString::Format(_T("Geoset_%i"), i));

		const char flags = 0; // SELECTED
		f.Write(&flags, sizeof(flags));

		char name[32];
		strncpy(name, groupName.mb_str(), sizeof(name));
		f.Write(name, sizeof(name));

		unsigned short faceCount = groups[i].p.indexCount / 3;
		f.Write(reinterpret_cast<char *>(&faceCount), sizeof(faceCount));
		
		for (int k=0; k<faceCount; k++) {
			//triIndices[k] = indiceCount;
			f.Write(reinterpret_cast<char *>(&indiceCount), sizeof(indiceCount));
			indiceCount++;
		}

		unsigned char gIndex = (char)i;
		f.Write(reinterpret_cast<char *>(&gIndex), sizeof(gIndex));
	}
	wxLogMessage(_T("Group Data Written."));

	// Number of materials (pretty much identical to groups, each group has its own material)
	f.Write(reinterpret_cast<char *>(&numGroups), sizeof(numGroups));
	
	for (unsigned short i=0; i<(unsigned int)numGroups; i++) {
		wxString matName(wxString::Format(_T("Material_%i"), i));

		ModelRenderPass p = groups[i].p;
		if (p.init(groups[i].m)) {
			ms3d_material_t mat;
			memset(mat.alphamap, '\0', sizeof(mat.alphamap));

			strncpy(mat.name, matName.mb_str(), sizeof(mat.name));
			mat.ambient[0] = 0.7f;
			mat.ambient[1] = 0.7f;
			mat.ambient[2] = 0.7f;
			mat.ambient[3] = 1.0f;
			mat.diffuse[0] = p.ocol.x;
			mat.diffuse[1] = p.ocol.y;
			mat.diffuse[2] = p.ocol.z;
			mat.diffuse[3] = p.ocol.w;
			mat.specular[0] = 0.0f;
			mat.specular[1] = 0.0f;
			mat.specular[2] = 0.0f;
			mat.specular[3] = 1.0f;
			mat.emissive[0] = p.ecol.x;
			mat.emissive[1] = p.ecol.y;
			mat.emissive[2] = p.ecol.z;
			mat.emissive[3] = p.ecol.w;
			mat.transparency = p.ocol.w;

			if (p.useEnvMap) {
				mat.shininess = 30.0f;
				mat.mode = 1;
			} else {
				mat.shininess = 0.0f;
				mat.mode = 0;
			}
/*
			unsigned int bindtex = 0;
			if (groups[i].m->specialTextures[p.tex]==-1) 
				bindtex = groups[i].m->textures[p.tex];
			else 
				bindtex = groups[i].m->replaceTextures[groups[i].m->specialTextures[p.tex]];
*/
			wxString texName = GetM2TextureName(m,fn,p,i);
			texName << wxT(".tga");
			strncpy(mat.texture, texName.mb_str(), sizeof(mat.texture));

			f.Write(reinterpret_cast<char *>(&mat), sizeof(ms3d_material_t));

			wxString texFilename(fn, wxConvUTF8);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += texName;
			wxLogMessage(_T("Exporting Image: %s"),texFilename.c_str());
			SaveTexture(texFilename);
		}
	}
	wxLogMessage(_T("Material Data Written."));

#if 0
	// save some keyframe data
	float fps = 1.0f;
	float fCurTime = 0.0f;
	int totalFrames = 0;

	f.Write(reinterpret_cast<char *>(&fps), sizeof(fps));
	f.Write(reinterpret_cast<char *>(&fCurTime), sizeof(fCurTime));
	f.Write(reinterpret_cast<char *>(&totalFrames), sizeof(totalFrames));
	
	
	// number of joints
	unsigned short numJoints = 0; //(unsigned short)m->header.nBones;

	f.Write(reinterpret_cast<char *>(&numJoints), sizeof(numJoints));
#else
	// TODO
	// save some keyframe data
	float fps = 1.0f; //m->anims[m->anim].playSpeed;
	float fCurTime = 0.0f;
	int totalFrames = 0; // (m->anims[m->anim].timeEnd - m->anims[m->anim].timeStart);

	f.Write(reinterpret_cast<char *>(&fps), sizeof(fps));
	f.Write(reinterpret_cast<char *>(&fCurTime), sizeof(fCurTime));
	f.Write(reinterpret_cast<char *>(&totalFrames), sizeof(totalFrames));
	
	// number of joints

	unsigned short numJoints = (unsigned short)m->header.nBones;

	f.Write(reinterpret_cast<char *>(&numJoints), sizeof(numJoints));

	for (int i=0; i<numJoints; i++) {
		ms3d_joint_t joint;

		joint.flags = 0; // SELECTED
		memset(joint.name, '\0', sizeof(joint.name));
		snprintf(joint.name, sizeof(joint.name), "Bone_%i_%i", m->anim, i);
		memset(joint.parentName, '\0', sizeof(joint.parentName));

		joint.rotation[0] = 0; // m->bones[i].pivot.x;
		joint.rotation[1] = 0; // m->bones[i].pivot.y;
		joint.rotation[2] = 0; // m->bones[i].pivot.z;

		joint.position[0] = m->bones[i].transPivot.x;
		joint.position[1] = m->bones[i].transPivot.y;
		joint.position[2] = m->bones[i].transPivot.z;

		int parent = m->bones[i].parent;
		if (parent > -1) {
			snprintf(joint.parentName, sizeof(joint.parentName), "Bone_%i_%i", m->anim, parent);

			joint.position[0] -= m->bones[parent].transPivot.x;
			joint.position[1] -= m->bones[parent].transPivot.y;
			joint.position[2] -= m->bones[parent].transPivot.z;
		}

		joint.numKeyFramesRot = 0; //(unsigned short)m->bones[i].rot.data[m->anim].size();
		joint.numKeyFramesTrans = 0; //(unsigned short)m->bones[i].trans.data[m->anim].size();

		f.Write(reinterpret_cast<char *>(&joint), sizeof(ms3d_joint_t));

		if (joint.numKeyFramesRot > 0) {
			ms3d_keyframe_rot_t *keyFramesRot = new ms3d_keyframe_rot_t[joint.numKeyFramesRot];
			for (int j=0; j<joint.numKeyFramesRot; j++) {
				keyFramesRot[j].time = m->bones[i].rot.times[m->anim][j]; // Error, time in seconds;
				keyFramesRot[j].rotation[0] = m->bones[i].rot.data[m->anim][j].x;
				keyFramesRot[j].rotation[1] = m->bones[i].rot.data[m->anim][j].y;
				keyFramesRot[j].rotation[2] = m->bones[i].rot.data[m->anim][j].z;
			}

			f.Write(reinterpret_cast<char *>(keyFramesRot), sizeof(ms3d_keyframe_rot_t) * joint.numKeyFramesRot);
			wxDELETEA(keyFramesRot);
		}

		if (joint.numKeyFramesTrans > 0) {
			ms3d_keyframe_pos_t *keyFramesTrans = new ms3d_keyframe_pos_t[joint.numKeyFramesTrans];
			for (unsigned int j=0; j<joint.numKeyFramesTrans; j++) {
				keyFramesTrans[j].time = m->bones[i].trans.times[m->anim][j]; // Error,time in seconds;;
				keyFramesTrans[j].position[0] = m->bones[i].trans.data[m->anim][j].x;
				keyFramesTrans[j].position[1] = m->bones[i].trans.data[m->anim][j].y;
				keyFramesTrans[j].position[2] = m->bones[i].trans.data[m->anim][j].z;
				if (parent > -1) {
					keyFramesTrans[j].position[0] -= m->bones[parent].transPivot.x;
					keyFramesTrans[j].position[1] -= m->bones[parent].transPivot.y;
					keyFramesTrans[j].position[2] -= m->bones[parent].transPivot.z;
					if (m->bones[parent].trans.data[m->anim].size() > j) {
						keyFramesTrans[j].position[0] -= m->bones[parent].trans.data[m->anim][j].x;
						keyFramesTrans[j].position[1] -= m->bones[parent].trans.data[m->anim][j].y;
						keyFramesTrans[j].position[2] -= m->bones[parent].trans.data[m->anim][j].z;
					}
				}
			}

			f.Write(reinterpret_cast<char *>(keyFramesTrans), sizeof(ms3d_keyframe_pos_t) * joint.numKeyFramesTrans);
			wxDELETEA(keyFramesTrans);
		}
	}
#endif
	f.Close();
	wxLogMessage(_T("Finished Milkshape Export."));

	if (verts){
		//wxLogMessage("verts found. Deleting...");
		wxDELETEA(verts);
	}
	if (groups){
		//wxLogMessage("groups found. Deleting...");
		wxDELETEA(groups);
	}

	//wxLogMessage(_T("Finished Milkshape Cleanup.\n"));
}