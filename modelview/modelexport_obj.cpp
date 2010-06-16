#include <wx/wfstream.h>
#include <math.h>

#include "globalvars.h"
#include "modelexport.h"
#include "modelcanvas.h"

//#include "CxImage/ximage.h"

void ExportM2toOBJ(Attachment *att, Model *m, const char *fn, bool init)
{
	// Open file
	ofstream f(fn, ios_base::out | ios_base::trunc);

	if (!f.is_open()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}
	LogExportData(_T("OBJ"),wxString(fn, wxConvUTF8).BeforeLast(SLASH),_T("M2"));

	unsigned short numVerts = 0;
	unsigned short numGroups = 0;
	unsigned short numFaces = 0;
	ModelData *verts = NULL;
	GroupData *groups = NULL;

	InitCommon(att, init, verts, groups, numVerts, numGroups, numFaces);
	wxString out;

	// http://people.sc.fsu.edu/~burkardt/data/mtl/mtl.html
	wxString matName(fn, wxConvUTF8);
	matName = matName.BeforeLast('.');
	matName << _T(".mtl");

	ofstream fm(matName.mb_str(), ios_base::out | ios_base::trunc);
	matName = matName.AfterLast('\\');

	fm << "#" << endl;
	fm << "# " << matName.mb_str() << endl;
	fm << "#" << endl;
	fm <<  endl;

	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
			
		if (p.init(m)) {
			wxString texName = GetM2TextureName(m,fn,p,(int)i);

			fm << _T("newmtl ") << texName << endl;
			texName << _T(".tga");
			fm << "illum 2" << endl;
			out = wxString::Format(_T("Kd %.06f %.06f %.06f"), p.ocol.x, p.ocol.y, p.ocol.z);
			fm << out.c_str() << endl;
			out = wxString::Format(_T("Ka %.06f %.06f %.06f"), 0.7f, 0.7f, 0.7f);
			fm << out.c_str() << endl;
			out = wxString::Format(_T("Ks %.06f %.06f %.06f"), p.ecol.x, p.ecol.y, p.ecol.z);
			fm << out.c_str() << endl;
			fm << "Ke 0.000000 0.000000 0.000000" << endl;
			out = wxString::Format(_T("Ns %0.6f"), 0.0f);
			fm << out.c_str() << endl;
			//fm << "Ka " << 0.7f << " " << 0.7f << " " << 0.7f << endl;
			//fm << "Kd " << p.ocol.x << " " << p.ocol.y << " " << p.ocol.z << endl;
			//fm << "Ks " << p.ecol.x << " " << p.ecol.y << " " << p.ecol.z << endl;
			//fm << "Ns " << p.ocol.w << endl;
			fm << "map_Kd " << texName.c_str() << endl << endl;

			
			wxString texFilename(fn, wxConvUTF8);
			texFilename = texFilename.BeforeLast(SLASH);
			texFilename += SLASH;
			texFilename += texName;
			wxLogMessage(_T("Exporting Image: %s"),texFilename.c_str());
			SaveTexture(texFilename);
			
		}
	}

	fm.close();

	f << "# Wavefront OBJ exported by WoW Model Viewer " << APP_VERSION << endl << endl;
	f << "mtllib " << matName.mb_str() << endl << endl;

	// output all the vertice data
	int vertics = 0;
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			//f << "# Chunk Indice Count: " << p.indexCount << endl;

			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				if (m->vertices == NULL || init == true) {
					out = wxString::Format(_T("v %.06f %.06f %.06f"), m->origVertices[a].pos.x, m->origVertices[a].pos.y, m->origVertices[a].pos.z);
					f << out.c_str() << endl;
					//f << "v " << m->origVertices[a].pos.x << " " << m->origVertices[a].pos.y << " " << m->origVertices[a].pos.z << endl;
				} else {
					out = wxString::Format(_T("v %.06f %.06f %.06f"), m->vertices[a].x, m->vertices[a].y, m->vertices[a].z);
					f << out.c_str() << endl;
					//f << "v " << m->vertices[a].x << " " << m->vertices[a].y << " " << m->vertices[a].z << endl;
				}
				vertics ++;
			}
		}
	}
	f << "# " << vertics << " vertices" << endl << endl;

	// output all the texture coordinate data
	int textures = 0;
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		// we don't want to render completely transparent parts
		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				out = wxString::Format(_T("vt %.06f %.06f"), m->origVertices[a].texcoords.x, 1-m->origVertices[a].texcoords.y);
				f << out.c_str() << endl;
				//f << "vt " << m->origVertices[a].texcoords.x << " " << (1 - m->origVertices[a].texcoords.y) << endl;
				textures ++;
			}
		}
	}
	f << "# " << textures << " texture coordinates" << endl << endl;

	// output all the vertice normals data
	int normals = 0;
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				out = wxString::Format(_T("vn %.06f %.06f %.06f"), m->origVertices[a].normal.x, m->origVertices[a].normal.y, m->origVertices[a].normal.z);
				f << out.c_str() << endl;
				//f << "vn " << m->origVertices[a].normal.x << " " << m->origVertices[a].normal.y << " " << m->origVertices[a].normal.z << endl;
				normals ++;
			}
		}
	}
	f << "# " << normals << " normals" << endl << endl;

	int counter=1;
	// output the indice data
	int triangles = 0;
	int triangles_total = 0;
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			wxString FilePath = wxString(fn, wxConvUTF8).BeforeLast(SLASH);
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
				texName << wxString(m->modelname.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) << wxString::Format(_T("_Image_%03i"),i);

			f << "g Geoset_" << i << endl;
			f << "usemtl " << texName << endl;
			f << "s 1" << endl;
			triangles = 0;
			for (unsigned int k=0; k<p.indexCount; k+=3) {
				f << "f ";
				f << counter << "/" << counter << "/" << counter << " ";
				counter ++;
				f << counter << "/" << counter << "/" << counter << " ";
				counter ++;
				f << counter << "/" << counter << "/" << counter << endl;
				counter ++;
				triangles ++;
			}
			f << "# " << triangles << " triangles in group" << endl << endl;
			triangles_total += triangles;
		}
	}

	f << "# " << triangles_total << " triangles total" << endl << endl;
	
	// Close file
	f.close();
}

void ExportWMOtoOBJ(WMO *m, const char *fn)
{
	// Open file
	wxString file = wxString(fn, wxConvUTF8);

	if (modelExport_PreserveDir == true){
		wxString Path1, Path2, Name;
		Path1 << file.BeforeLast('\\');
		Name << file.AfterLast('\\');
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast('\\');

		MakeDirs(Path1,Path2);

		file.Empty();
		file << Path1 << SLASH << Path2 << SLASH << Name;
	}

	ofstream f(file.mb_str(), ios_base::out | ios_base::trunc);

	if (!f.is_open()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}
	LogExportData(_T("OBJ"),wxString(fn, wxConvUTF8).BeforeLast(SLASH),_T("WMO"));

	wxString mtlName = file;
	mtlName = mtlName.BeforeLast('.');
	mtlName << _T(".mtl");

	ofstream fm(mtlName.mb_str(), ios_base::out | ios_base::trunc);
	mtlName = mtlName.AfterLast(SLASH);

	fm << "#" << endl;
	fm << "# " << mtlName.mb_str() << endl;
	fm << "#" << endl;
	fm <<  endl;

	wxString *texarray = new wxString[m->nTextures+1];

	// Find a Match for mat->tex and place it into the Texture Name Array.
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			WMOMaterial *mat = &m->mat[batch->texture];

			wxString outname(fn, wxConvUTF8);

			bool nomatch = true;
			for (int t=0;t<=m->nTextures; t++) {
				if (t == mat->tex) {
					texarray[mat->tex] = wxString(m->textures[t-1].c_str(), wxConvUTF8);
					texarray[mat->tex] = texarray[mat->tex].BeforeLast('.');
					nomatch = false;
					break;
				}
			}
			if (nomatch == true){
				outname = outname.AfterLast('\\');
				texarray[mat->tex] = outname << wxString::Format(_T("_Material_%03i"), mat->tex);
			}
		}
	}

	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			WMOMaterial *mat = &m->mat[batch->texture];

			//wxString matName(wxString::Format(_T("Material_%03i"), mat->tex));
			wxString matName = texarray[mat->tex];

			//wxString texName(fn, wxConvUTF8);
			wxString texName = texarray[mat->tex];
			wxString texPath = texName.BeforeLast('\\');
			texName = texName.AfterLast('\\');
			//texName << _T("_") << mat->tex << _T(".tga");
			texName << _T(".tga");

			//fm << "newmtl " << "Material_" << mat->tex+1 << endl;
			// MilkShape3D cann't read long texname
			if (modelExport_PreserveDir == true)
				fm << "newmtl " << texarray[mat->tex] << endl;
			else
				fm << "newmtl " << texarray[mat->tex].AfterLast('\\') << endl;
			fm << "Kd 0.750000 0.750000 0.750000" << endl; // diffuse
			fm << "Ka 0.250000 0.250000 0.250000" << endl; // ambient
			fm << "Ks 0.000000 0.000000 0.000000" << endl; // specular
			//fm << "Ke 0.000000 0.000000 0.000000" << endl;
			fm << "Ns 0.000000" << endl;
			fm << "map_Kd " << texName.c_str() << endl << endl;

			wxString texFilename(fn, wxConvUTF8);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += texName;
			
			if (modelExport_PreserveDir == true){
				wxString Path1, Path2, Name;
				Path1 << wxString(texFilename, wxConvUTF8).BeforeLast('\\');
				Name << texName.AfterLast('\\');
				Path2 << texPath;

				MakeDirs(Path1,Path2);

				texFilename.Empty();
				texFilename << Path1 << '\\' << Path2 << '\\' << Name;
			}

			// setup texture
			glBindTexture(GL_TEXTURE_2D, mat->tex);
			wxLogMessage(_T("Exporting Image: %s"),texFilename.c_str());
			SaveTexture(texFilename);
		}
	}
	fm.close();

	f << "# Wavefront OBJ exported by WoW Model Viewer v0.5.0.8" << endl << endl;
	f << "mtllib " << mtlName.mb_str() << endl << endl;

	// geometric vertices (v)
	// v x y z weight
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			for(int ii=0;ii<batch->indexCount;ii++)
			{
				int a = m->groups[i].indices[batch->indexStart + ii];
				f << "v " << m->groups[i].vertices[a].x << " " << m->groups[i].vertices[a].z << " " << -m->groups[i].vertices[a].y << endl;
			}
		}
	}
	f << endl;

	// texture vertices (vt)
	// vt horizontal vertical depth
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			for(int ii=0;ii<batch->indexCount;ii++)
			{
				int a = m->groups[i].indices[batch->indexStart + ii];
				f << "vt " << m->groups[i].texcoords[a].x << " " << (1 - m->groups[i].texcoords[a].y) << endl;
			}
		}
	}
	f << endl;

	// vertex normals (vn)
	// vn x y z
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			for(int ii=0;ii<batch->indexCount;ii++)
			{
				int a = m->groups[i].indices[batch->indexStart + ii];
				f << "vn " << m->groups[i].normals[a].x << " " << m->groups[i].normals[a].z << " " << -m->groups[i].normals[a].y << endl;
			}
		}
	}
	f << endl;

	// Referencing groups of vertices
	// f v/vt/vn v/vt/vn v/vt/vn v/vt/vn
	int counter = 1;
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			WMOMaterial *mat = &m->mat[batch->texture];

			// batch->texture or mat->tex ?
			f << "g Geoset_" << i << "_" << j << "_tex_" << int(batch->texture) << endl;
			f << "s 1" << endl;
			//f << "usemtl Material_" << mat->tex+1 << endl;
			// MilkShape3D cann't read long texname
			if (modelExport_PreserveDir == true)
				f << "usemtl " << texarray[mat->tex] << endl;
			else
				f << "usemtl " << texarray[mat->tex].AfterLast('\\') << endl;
			for (unsigned int k=0; k<batch->indexCount; k+=3) {
				f << "f ";
				f << counter << "/" << counter << "/" << counter << " ";
				f << (counter+1) << "/" << (counter+1) << "/" << (counter+1) << " ";
				f << (counter+2) << "/" << (counter+2) << "/" << (counter+2) << endl;
				counter += 3;
			}
			f << endl;
		}
	}

	// Close file
	f.close();
	wxDELETEA(texarray);
}