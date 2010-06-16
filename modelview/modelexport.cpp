#include <wx/wfstream.h>
#include <math.h>

#include "globalvars.h"
#include "modelexport.h"
#include "modelcanvas.h"

#include "CxImage/ximage.h"

// 2 methods to go,  just export the entire m2 model.
// or use our "drawing" routine to export only whats being drawn.

// SaveTexture
// Used to save composite textures, such as a character's face & body.
void SaveTexture(wxString fn)
{
	// Slash correction, just in case.
	#ifndef _WINDOWS
		fn.Replace(wxT("\\"),wxT("/"));
	#endif
	unsigned char *pixels = NULL;

	GLint width, height;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

	pixels = new unsigned char[width * height * 4];

	glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);

	CxImage *newImage = new CxImage(0);
	newImage->CreateFromArray(pixels, width, height, 32, (width*4), true);


	if (fn.Last() == 'g')
		newImage->Save(fn.mb_str(), CXIMAGE_FORMAT_PNG);
	else
		newImage->Save(fn.mb_str(), CXIMAGE_FORMAT_TGA);

	newImage->Destroy();
	wxDELETE(newImage);
	wxDELETEA(pixels);
}

// SaveTexture2 Function
// Used to export images that are filenames. For composited images, such as a character's face & body texture, use SaveTexture.
// ExportID identifies the exporting function. This is used in the path-generating section.
// Suffixes currently supported: "tga" & "png". Defaults to tga if omitted by exporter.
void SaveTexture2(wxString file, wxString outdir, wxString ExportID, wxString suffix = wxString(wxT("tga")))
{
	// Check to see if we have all our data...
	if (file == wxEmptyString)
		return;
	if (outdir == wxEmptyString)
		return;

	wxFileName fn(file);
	if (fn.GetExt().Lower() != _T("blp"))
		return;
	TextureID temptex = texturemanager.add(std::string(file.mb_str()));
	Texture &tex = *((Texture*)texturemanager.items[temptex]);
	if (tex.w == 0 || tex.h == 0)
		return;

	wxString temp;

	unsigned char *tempbuf = (unsigned char*)malloc(tex.w*tex.h*4);
	tex.getPixels(tempbuf, GL_BGRA_EXT);

	CxImage *newImage = new CxImage(0);
	newImage->AlphaCreate();	// Create the alpha layer
	newImage->IncreaseBpp(32);	// set image to 32bit 
	newImage->CreateFromArray(tempbuf, tex.w, tex.h, 32, (tex.w*4), true);

	wxString fileName = file.AfterLast(SLASH).BeforeLast('.');
	wxString filePath = file.BeforeLast(SLASH);

	// -= Pre-Path Directories =-
	// Add any directories inbetween the target directory and the preserved directory go here.

	// Secure paths for Non-Windows systems
	#ifndef _WINDOWS
		outdir.Replace(wxT("\\"),wxT("/"));
		filePath.Replace(wxT("\\"),wxT("/"));
	#endif

	// Lightwave
	if (ExportID.IsSameAs(wxT("LWO"))){
		if (modelExport_LW_PreserveDir == true){
			MakeDirs(outdir,wxT("Images"));
			outdir += SLASH+wxT("Images")+SLASH;
		}
	// Wavefront OBJ
	}else if (ExportID.IsSameAs(wxT("OBJ"))){
	}

	// Restore WoW's content directories for this image.
	if (modelExport_PreserveDir == true){
		MakeDirs(outdir,filePath);
		outdir += filePath;
		outdir << SLASH;
	}

	// Final Filename
	temp = outdir+fileName+wxT(".")+suffix;
	wxLogMessage(_T("Exporting Image: %s"),temp.c_str());

	//wxLogMessage(_T("Info: Exporting texture to %s..."), temp.c_str());

	// Save image!
	if (suffix == _T("tga"))
		newImage->Save(temp.mb_str(), CXIMAGE_FORMAT_TGA);
	else
		newImage->Save(temp.mb_str(), CXIMAGE_FORMAT_PNG);

	// Clear data we don't need anymore
	free(tempbuf);
	newImage->Destroy();
	wxDELETE(newImage);
	
}

// Limit a value by a min & a max. The Inc controls by how much to reduce for every run.
double Clamp(double value, float min, float max, float inc = PI){
	if (value > 0){
		while (value >= max){
			value -= inc;
		}
	}else if (value < 0){
		while (value <= min){
			value += inc;
		}
	}
	
	return value;
}

// Converts WoW's Radians into a 3D friendly Heading, Pitch, Bank system.
// Tested only with Lightwave. Might have to make minor direction changes here to make it more univsersal.
Vec3D QuaternionToXYZ(Vec3D Dir, float W){
	//-dir.z,dir.x,dir.y	WoW Direction...
	Vec3D vdir(Dir.x,Dir.z,-Dir.y);
	float c,angle_x,angle_y,angle_z,tempx,tempy;

	Vec3D XYZ;
	Matrix m;
	Quaternion q(vdir, W);
	m.quaternionRotate(q);

	angle_y = asin(m.m[0][2]);
	c = cos(angle_y);
	if (fabs(c) > 0.005){		// If not Gimble-locked
		tempx = m.m[2][2];
		tempy = -m.m[1][2];

		angle_x = atan2(tempy,tempx);

		tempx = m.m[0][0] / c;
		tempy = -m.m[0][1] / c;

		angle_z = atan2(tempy,tempx);
	}else{						// If Gimble-lock occured...
		wxString ays, cs;
		ays << angle_y;
		cs << c;
		wxMessageBox(_T("Gimbal Lock Occured!\nPlease send the logfile and the name of the\nobject you were outputting to the development\nteam so they can attempt to fix any errors\ncaused by this."),_T("Gimble Lock Warning"));
		wxLogMessage(_T("Gimbal Lock happened! angle_y=%s, c=%s"), ays.c_str(), cs.c_str());

		angle_x  = 0;
		tempx = m.m[1][1];
		tempy = m.m[1][0];

		angle_z  = atan2(tempy, tempx);
	}

	if (angle_y < 0)
		angle_y = -angle_y;

	// 1.0 = 1 Radian, or 57.295779513082320876798154814114 degrees (180/PI)
	float d180 = (float)PI;	// 180 degrees
	int accuracy = 6;	// to the 6th decimal point...
	XYZ.x = (float)(Clamp(round(angle_x+d180,accuracy),(float)-PI*2,(float)PI*2));			// Heading
	XYZ.y = (float)(Clamp(round(-angle_y,accuracy),(float)-PI*2,(float)PI*2));				// Pitch
	XYZ.z = (float)(-Clamp(round(angle_z+d180,accuracy),(float)-PI*2,(float)PI*2));			// Bank

	return XYZ;
}

void AddCount(Model *m, unsigned short &numGroups, unsigned short &numVerts)
{
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			numGroups++;
			
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				numVerts++;
			}
		}
	}
}

void AddVertices(Model *m, Attachment *att, bool init, ModelData *verts, unsigned short &vertIndex, GroupData *groups, unsigned short &grpIndex)
{
	wxLogMessage(_T("Adding Verticies from %s..."), m->name.c_str());
	int boneID = -1;
	Model *mParent = NULL;

	if (att->parent) {
		mParent = static_cast<Model*>(att->parent->model);
		if (mParent)
			boneID = mParent->attLookup[att->id];
	}

	Vec3D pos(0,0,0);
	Vec3D scale(1,1,1);
	if (boneID>-1) {
		// Note: We still need to rotate the item's points!! Note 2: Rotations should happen BEFORE scale!
		pos = mParent->atts[boneID].pos;
		Bone cbone = mParent->bones[mParent->atts[boneID].bone];
		Matrix mat = cbone.mat;
		if (init == true){
			// InitPose is a reference to the HandsClosed animation (#15), which is the closest to the Initial pose.
			// By using this animation, we'll get the proper scale for the items when in Init mode.
			int InitPose = 15;
			scale = cbone.scale.getValue(InitPose,0);
			if (scale.x == 0 && scale.y == 0 && scale.z == 0){
				scale.x = 1;
				scale.y = 1;
				scale.z = 1;
			}
		}else{
			// Scale takes into consideration only the final size of an object. This means that if a staff it rotated 90 degrees,
			// the final scale will be as if the staff is REALLY short. This should solve itself after we get rotations working.
			scale.x = mat.m[0][0];
			scale.y = mat.m[1][1];
			scale.z = mat.m[2][2];

			// Moves the item to the proper position.
			mat.translation(cbone.transPivot);
			pos.x = mat.m[0][3];
			pos.y = mat.m[1][3];
			pos.z = mat.m[2][3];
		}
	}

	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				//wxLogMessage(_T("Processing vertIndex %i, grpIndex %i"),vertIndex,grpIndex);
				uint16 a = m->indices[b];
				
				if ((init == false)&&(m->vertices)) {
					verts[vertIndex].vertex.x = ((m->vertices[a].x * scale.x) + pos.x);
					verts[vertIndex].vertex.y = ((m->vertices[a].y * scale.y) + pos.y);
					verts[vertIndex].vertex.z = ((m->vertices[a].z * scale.z) + pos.z);

					if (video.supportVBO) {
						verts[vertIndex].normal.x = (m->vertices[m->header.nVertices + a].x + pos.x);
						verts[vertIndex].normal.y = (m->vertices[m->header.nVertices + a].y + pos.y);
						verts[vertIndex].normal.z = (m->vertices[m->header.nVertices + a].z + pos.z);
					} else {
						verts[vertIndex].normal.x = m->normals[a].x;
						verts[vertIndex].normal.y = m->normals[a].y;
						verts[vertIndex].normal.z = m->normals[a].z;
					}
				} else {
					verts[vertIndex].vertex.x = ((m->origVertices[a].pos.x * scale.x) + pos.x);
					verts[vertIndex].vertex.y = ((m->origVertices[a].pos.y * scale.y) + pos.y);
					verts[vertIndex].vertex.z = ((m->origVertices[a].pos.z * scale.z) + pos.z);

					verts[vertIndex].normal.x = m->origVertices[a].normal.x;
					verts[vertIndex].normal.y = m->origVertices[a].normal.y;
					verts[vertIndex].normal.z = m->origVertices[a].normal.z;
				}

				verts[vertIndex].tu = m->origVertices[a].texcoords.x;
				verts[vertIndex].tv = m->origVertices[a].texcoords.y;

				verts[vertIndex].groupIndex = grpIndex;
				verts[vertIndex].boneid = m->origVertices[a].bones[0];

				vertIndex++;
			}
			groups[grpIndex].p = p;
			groups[grpIndex].m = m;
			grpIndex++;
		}
	}
}

void InitCommon(Attachment *att, bool init, ModelData *&verts, GroupData *&groups, unsigned short &numVerts, unsigned short &numGroups, unsigned short &numFaces)
{
	unsigned short vertIndex = 0;
	unsigned short grpIndex = 0;

	if (!att)
		return;

	wxLogMessage(_T("Counting Verticies via InitCommon..."));

	Model *m = NULL;
	if (att->model) {
		m = static_cast<Model*>(att->model);
		if (!m)
			return;

		AddCount(m, numGroups, numVerts);
	}

	// children:
	for (size_t i=0; i<att->children.size(); i++) {
		Model *mAtt = static_cast<Model*>(att->children[i]->model);
		if (mAtt)
			AddCount(mAtt, numGroups, numVerts);

		Attachment *att2 = att->children[i];
		for (size_t j=0; j<att2->children.size(); j++) {
			Model *mAttChild = static_cast<Model*>(att2->children[j]->model);
			if (mAttChild)
				AddCount(mAttChild, numGroups, numVerts);
		}
	}

	numFaces = (numVerts / 3);

	verts = new ModelData[numVerts];
	//indices = new float[numVerts];
	groups = new GroupData[numGroups];

	wxLogMessage(_T("Num Verts: %i, Num Faces: %i, Num Groups: %i"), numVerts, numFaces, numGroups);
	wxLogMessage(_T("Adding Verticies via InitCommon..."));

	if (m)
		AddVertices(m, att, init, verts, vertIndex, groups, grpIndex);

	// children:
	for (size_t i=0; i<att->children.size(); i++) {
		Model *mAtt = static_cast<Model*>(att->children[i]->model);
		if (mAtt)
			AddVertices(mAtt, att->children[i], init, verts, vertIndex, groups, grpIndex);

		Attachment *att2 = att->children[i];
		for (size_t j=0; j<att2->children.size(); j++) {
			Model *mAttChild = static_cast<Model*>(att2->children[j]->model);
			if (mAttChild)
				AddVertices(mAttChild, att2->children[j], init, verts, vertIndex, groups, grpIndex);
		}
	}
	#ifdef _DEBUG
		wxLogMessage(_T("Vert[0] BoneID: %i"),verts[0].boneid);
	#endif

	wxLogMessage(_T("Finished InitCommon Function."));
}

// Get Proper Texture Names for an M2 File
wxString GetM2TextureName(Model *m, const char *fn, ModelRenderPass p, int PassNumber){
	wxString texName = wxString(m->TextureList[p.tex].c_str(), wxConvUTF8).BeforeLast(_T('.'));
	wxString texPath = texName.BeforeLast(SLASH);
	if (m->modelType == MT_CHAR){
		texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_") + texName.AfterLast(SLASH);
	}else if ((texName.Find(SLASH) <= 0)&&(texName == _T("Cape"))){
		texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_Replacable");
		texPath = wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
	}else if (texName.Find(SLASH) <= 0){
		texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_") + texName;
		texPath = wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
	}else{
		texName = texName.AfterLast(SLASH);
	}

	if (texName.Length() == 0)
		texName << wxString(m->modelname.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) << wxString::Format(_T("_Image_%03i"),PassNumber);

	return texName;
}

// Write out some debug info
void LogExportData(wxString FileExtension, wxString Directory, wxString ExportType){
	wxLogMessage(_T("\n\n========================================================================\n   Exporting Model...\n========================================================================\n"));
	wxLogMessage(_T("Exporting to Directory: %s"),Directory.c_str());
	wxLogMessage(_T("Exporting File Type: %s"),FileExtension.c_str());
	wxLogMessage(_T("Original Model File Type: %s"), ExportType.c_str());
	wxLogMessage(_T("Export Init Mode: %s"),(modelExportInitOnly==true?"True":"False"));
	wxLogMessage(_T("Preserve Directories: %s"),(modelExport_PreserveDir==true?"True":"False"));
	wxLogMessage(_T("Use WMV Position & Rotation: %s"),(modelExport_UseWMVPosRot==true?"True":"False"));

	// Lightwave Options
	if (FileExtension == _T("LWO")){
		wxLogMessage(_T("Preserve Lightwave Directories: %s"),(modelExport_LW_PreserveDir==true?"True":"False"));
		wxLogMessage(_T("Export Lights: %s"),(modelExport_LW_ExportLights==true?"True":"False"));
		wxLogMessage(_T("Export Doodads: %s"),(modelExport_LW_ExportDoodads==true?"True":"False"));
		wxString XDDas;
		switch (modelExport_LW_DoodadsAs) {
			case 0:
				XDDas = _T("Nulls");
				break;
			case 1:
				XDDas = _T("Objects");
				break;
			case 2:
				XDDas = _T("A Single Object");
				break;
			case 3:
				XDDas = _T("A Single Object, Per Group");
				break;
		}
		wxLogMessage(_T("Export Doodads as: %s"),XDDas.c_str());
	// X3D Options
	}else if (FileExtension == _T("X3D")){
		wxLogMessage(_T("Export Animation: %s"),(modelExport_X3D_ExportAnimation==true?"True":"False"));
		wxLogMessage(_T("Center Model: %s"),(modelExport_X3D_CenterModel==true?"True":"False"));
	}
}

// Disabled Collada Exporters
void ExportM2toCOLLADA(Attachment *att, Model *m, const char *fn, bool init){}
void ExportWMOtoCOLLADA(WMO *m, const char *fn){}

void SaveBaseFile(){
	if (g_fileControl->fileTree->HasChildren(g_fileControl->CurrentItem)){
		wxLogMessage(_T("File has children. Cancelling Save Fuction..."));
		return;
	}
	FileTreeData *data = (FileTreeData*)g_fileControl->fileTree->GetItemData(g_fileControl->CurrentItem);
	wxString modelfile(data->fn.c_str(), wxConvUTF8);

	//modelfile = g_fileControl->fileTree->GetParent()+_T("\\")+g_fileControl->fileTree->GetItemText(g_fileControl->CurrentItem);
	wxLogMessage(_T("Original Model File Name: %s"),modelfile.c_str());

	if (modelfile.IsEmpty())
		return;
	MPQFile f(modelfile.mb_str());
	if (f.isEof()) {
		wxLogMessage(_T("Error: Could not extract %s\n"), modelfile.c_str());
		f.close();
		return;
	}
	wxFileName fn(modelfile);

	FILE *hFile = NULL;
	wxString filename;
	filename = wxFileSelector(wxT("Please select your file to export"), wxGetCwd(), fn.GetName(), fn.GetExt(), fn.GetExt()+_T(" files (.")+fn.GetExt()+_T(")|*.")+fn.GetExt());

	if ( !filename.empty() )
	{
		hFile = fopen(filename.mb_str(), "wb");
	}
	if (hFile) {
		fwrite(f.getBuffer(), 1, f.getSize(), hFile);
		fclose(hFile);
	}
	f.close();
}