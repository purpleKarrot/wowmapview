#include <wx/wfstream.h>

#include "globalvars.h"
#include "modelexport.h"
#include "modelexport_3ds.h"
#include "modelcanvas.h"

//#include "CxImage/ximage.h"



// 3D Studio Max
// http://gpwiki.org/index.php/Loading_3ds_files
// http://www.martinreddy.net/gfx/3d/3DS.spec
// http://www.martinreddy.net/gfx/3d/MLI.spec
// http://gpwiki.org/index.php/C:Load3DS
void ExportM2to3DS(Attachment *att, Model *m, const char *fn, bool init)
{
	if (!m)
		return;

	//ofstream f(fn, ios::out | ios::binary | ios::trunc);
	wxFFileOutputStream f(wxString(fn, wxConvUTF8), wxT("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}
	LogExportData(_T("3DS"),wxString(fn, wxConvUTF8).BeforeLast(SLASH),_T("M2"));

	unsigned short numVerts = 0;
	unsigned short numGroups = 0;
	unsigned short numFaces = 0;
	ModelData *verts = NULL;
	GroupData *groups = NULL;

	// ------------------------------------

	InitCommon(att, init, verts, groups, numVerts, numGroups, numFaces);

	// Primary chunk
	MAX3DS_HEADER header;

	// EDIT3DS chunk
	MAX3DS_CHUNK chunk1;
	chunk1.id = EDIT3DS; // 0x3D3D

	// Now write the data to file
	f.Write(&header, sizeof(MAX3DS_HEADER));
	f.Write(&chunk1, sizeof(MAX3DS_CHUNK)); // EDIT3DS
	chunk1.size = sizeof(MAX3DS_CHUNK);

	// EDIT_CONFIG2 chunk
	MAX3DS_CHUNK chunk2_1;
	chunk2_1.id = MESH_VERSION; // 0x3D3E
	
	int meshVersion = 3;
	chunk2_1.size += sizeof(int);

	f.Write(&chunk2_1, sizeof(MAX3DS_CHUNK)); // MESH_VERSION
	f.Write(&meshVersion, sizeof(int));
	chunk1.size += chunk2_1.size;

	for (unsigned short i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			// EDIT_MATERIAL (0xAFFF)
			MAX3DS_CHUNK chunk2_2;
			chunk2_2.id = EDIT_MATERIAL; // 0xAFFF
			f.Write(&chunk2_2, sizeof(MAX3DS_CHUNK)); // EDIT_MATERIAL
			
			// MATNAME (0xA000) (See mli Doc) 
			// http://www.martinreddy.net/gfx/3d/MLI.spec
			MAX3DS_CHUNK chunk2_2_1;
			chunk2_2_1.id = MATNAME; // 0xA000
			wxString matName = wxString::Format(_T("Material_%i"), i);
			matName.Append(_T('\0'));
			chunk2_2_1.size = sizeof(MAX3DS_CHUNK) + (unsigned int)matName.length();
			chunk2_2.size = sizeof(MAX3DS_CHUNK) + chunk2_2_1.size;
			f.Write(&chunk2_2_1, sizeof(MAX3DS_CHUNK)); // MATNAME
			f.Write(matName.data(), matName.length());

			//COL_RGB  
			//chunk2_2_2.size += 18;
			MAX3DS_CHUNK chunk2_2_2;
			chunk2_2_2.id = MATAMBIENT; // 0xA010
			MAX3DS_CHUNK chunk2_2_2_1;
			chunk2_2_2_1.id = RGB1;
			ColorRGBs rgb2_2_2_1;
			rgb2_2_2_1.r = 178;
			rgb2_2_2_1.g = 178;
			rgb2_2_2_1.b = 178;
			chunk2_2_2_1.size += sizeof(ColorRGBs);
			MAX3DS_CHUNK chunk2_2_2_2;
			chunk2_2_2_2.id = RGB2;
			ColorRGBs rgb2_2_2_2;
			rgb2_2_2_2.r = 178;
			rgb2_2_2_2.g = 178;
			rgb2_2_2_2.b = 178;
			chunk2_2_2_2.size += sizeof(ColorRGBs);
			chunk2_2_2.size += chunk2_2_2_1.size;
			chunk2_2_2.size += chunk2_2_2_2.size;
			chunk2_2.size += chunk2_2_2.size;
			f.Write(&chunk2_2_2, sizeof(MAX3DS_CHUNK)); // MATAMBIENT
			f.Write(&chunk2_2_2_1, sizeof(MAX3DS_CHUNK)); // RGB1
			f.Write(&rgb2_2_2_1, sizeof(ColorRGBs));
			f.Write(&chunk2_2_2_2, sizeof(MAX3DS_CHUNK)); // RGB2
			f.Write(&rgb2_2_2_2, sizeof(ColorRGBs));

			//chunk2_2_3.size += 18;
			MAX3DS_CHUNK chunk2_2_3;
			chunk2_2_3.id = MATDIFFUSE; // 0xA020
			MAX3DS_CHUNK chunk2_2_3_1;
			chunk2_2_3_1.id = RGB1; // 0x0011
			ColorRGBs rgb2_2_3_1;
			rgb2_2_3_1.r = 255;
			rgb2_2_3_1.g = 255;
			rgb2_2_3_1.b = 255;
			chunk2_2_3_1.size += sizeof(ColorRGBs);
			MAX3DS_CHUNK chunk2_2_3_2;
			chunk2_2_3_2.id = RGB2; // 0x0012
			ColorRGBs rgb2_2_3_2;
			rgb2_2_3_2.r = 255;
			rgb2_2_3_2.g = 255;
			rgb2_2_3_2.b = 255;
			chunk2_2_3_2.size += sizeof(ColorRGBs);
			chunk2_2_3.size += chunk2_2_3_1.size;
			chunk2_2_3.size += chunk2_2_3_2.size;
			chunk2_2.size += chunk2_2_3.size;
			f.Write(&chunk2_2_3, sizeof(MAX3DS_CHUNK)); // MATDIFFUSE
			f.Write(&chunk2_2_3_1, sizeof(MAX3DS_CHUNK)); // RGB1
			f.Write(&rgb2_2_3_1, sizeof(ColorRGBs));
			f.Write(&chunk2_2_3_2, sizeof(MAX3DS_CHUNK)); // RGB2
			f.Write(&rgb2_2_3_2, sizeof(ColorRGBs));

			//chunk2_2_4.size += 18;
			MAX3DS_CHUNK chunk2_2_4;
			chunk2_2_4.id = MATSPECULAR; // 0xA030
			MAX3DS_CHUNK chunk2_2_4_1;
			chunk2_2_4_1.id = RGB1; // 0x0011
			ColorRGBs rgb2_2_4_1;
			rgb2_2_4_1.r = 0;
			rgb2_2_4_1.g = 0;
			rgb2_2_4_1.b = 0;
			chunk2_2_4_1.size += sizeof(ColorRGBs);
			MAX3DS_CHUNK chunk2_2_4_2;
			chunk2_2_4_2.id = RGB2; // 0x0012
			ColorRGBs rgb2_2_4_2;
			rgb2_2_4_2.r = 0;
			rgb2_2_4_2.g = 0;
			rgb2_2_4_2.b = 0;
			chunk2_2_4_2.size += sizeof(ColorRGBs);
			chunk2_2_4.size += chunk2_2_4_1.size;
			chunk2_2_4.size += chunk2_2_4_2.size;
			chunk2_2.size += chunk2_2_4.size;
			f.Write(&chunk2_2_4, sizeof(MAX3DS_CHUNK)); // MATDIFFUSE
			f.Write(&chunk2_2_4_1, sizeof(MAX3DS_CHUNK)); // RGB1
			f.Write(&rgb2_2_4_1, sizeof(ColorRGBs));
			f.Write(&chunk2_2_4_2, sizeof(MAX3DS_CHUNK)); // RGB2
			f.Write(&rgb2_2_4_2, sizeof(ColorRGBs));

			{
			chunk2_2.size += 0x0e;
			char aa[] = {0x40, 0xa0, 0x0e, 0, 0, 0, 0x30, 0, 0x08, 0, 0, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x0e;
			char aa[] = {0x41, 0xa0, 0x0e, 0, 0, 0, 0x30, 0, 0x08, 0, 0, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x0e;
			char aa[] = {0x50, 0xa0, 0x0e, 0, 0, 0, 0x30, 0, 0x08, 0, 0, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x0e;
			char aa[] = {0x52, 0xa0, 0x0e, 0, 0, 0, 0x30, 0, 0x08, 0, 0, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x0e;
			char aa[] = {0x53, 0xa0, 0x0e, 0, 0, 0, 0x30, 0, 0x08, 0, 0, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x08;
			char aa[] = {0x00, 0xa1, 0x08, 0, 0, 0, 0x3, 0};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x0e;
			char aa[] = {0x84, 0xa0, 0x0e, 0, 0, 0, 0x30, 0, 0x08, 0, 0, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x0a;
			char aa[] = {0x87, 0xa0, 0x0a, 0, 0, 0, 0, 0, 0x80, 0x3f};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x06;
			char aa[] = {0x8c, 0xa0, 0x06, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			MAX3DS_CHUNK chunk2_2_6;
			chunk2_2_6.id = MATMAPFILE; // 0xA300
			wxString mapName = GetM2TextureName(m,fn,p,i);
			mapName = mapName.AfterLast('\\').BeforeLast('.');
			mapName << wxT(".tga");
			mapName.Append(_T('\0'));
			chunk2_2_6.size = sizeof(MAX3DS_CHUNK) + (unsigned int)mapName.length();

			// save texture to file
			wxString texFilename(fn, wxConvUTF8);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += mapName;
			wxLogMessage(_T("Exporting Image: %s"),texFilename.c_str());
			SaveTexture(texFilename);

            // conatins 0xa300, 0xa351
            unsigned int n = 0x0e;
            n += (chunk2_2_6.size+0x08);               
			chunk2_2.size += n;
			char aa[] = {0x00, 0xa2, n, 0, 0, 0, 0x30, 0, 8, 0, 0, 0, 0x64, 0};
			f.Write(aa, sizeof(aa));
            
			f.Write(&chunk2_2_6, sizeof(MAX3DS_CHUNK)); // MATMAPNAME
			f.Write(mapName.data(), mapName.length());

			{
			char aa[] = {0x51, 0xa3, 0x08, 0, 0, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			int off_t = 0-chunk2_2.size;
			f.SeekO(off_t, wxFromCurrent);
			f.Write(&chunk2_2, sizeof(MAX3DS_CHUNK)); // EDIT_MATERIAL
			f.SeekO(0, wxFromEnd);
			chunk1.size += chunk2_2.size;
		}
	}

	//EDIT_CONFIG1 chunk
	MAX3DS_CHUNK chunk2_3;
	chunk2_3.id = EDIT_CONFIG1; // 0x0100
	float config1 = 1.0f;
	chunk2_3.size += sizeof(float);

	f.Write(&chunk2_3, sizeof(MAX3DS_CHUNK)); // EDIT_CONFIG1
	f.Write(&config1, sizeof(float));
	chunk1.size += chunk2_3.size;

	{
	chunk1.size += 0x0a;
	char aa[] = {0x0, 0x14, 0x0a, 0, 0, 0, 0, 0, 0x80, 0x3f};
	f.Write(aa, sizeof(aa));
	}

	{
	chunk1.size += 0x08;
	char aa[] = {0x20, 0x14, 0x08, 0, 0, 0, 0, 2};
	f.Write(aa, sizeof(aa));
	}

	{
	chunk1.size += 0x0a;
	char aa[] = {0x50, 0x14, 0x0a, 0, 0, 0, 0, 0, 0x40, 0x40};
	f.Write(aa, sizeof(aa));
	}

	{
	chunk1.size += 0xa;
	char aa[] = {0x60, 0x14, 0x0a, 0, 0, 0, 0, 0, 0x80, 0x3f};
	f.Write(aa, sizeof(aa));
	}

	{
	chunk1.size += 0x12;
	char aa[] = {0x0, 0x15, 0x12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	f.Write(aa, sizeof(aa));
	}

	{
	chunk1.size += 0x2a;
	char aa[] = {0x0, 0x21, 0x2a, 0, 0, 0, 0x10, 0, 0x12, 0, 0, 0, 0x9a, 0x99, 0x99, 0x3e,
		0x9a, 0x99, 0x99, 0x3e, 0x9a, 0x99, 0x99, 0x3e, 0x13, 0, 0x12, 0, 0, 0, 0x9a, 0x99,
		0x99, 0x3e, 0x9a, 0x99, 0x99, 0x3e, 0x9a, 0x99, 0x99, 0x3e};
	f.Write(aa, sizeof(aa));
	}


	for (unsigned short i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			
			// EDIT OBJECT chunk
			MAX3DS_CHUNK chunk2_4;
			chunk2_4.id = EDIT_OBJECT; // 0x4000
			
			// Model name
			wxString modName = wxString::Format(_T("Geoset_%i"), i);
			modName.Append(_T('\0'));
			chunk2_4.size += (unsigned int)modName.length();
			
			// OBJ_MESH chunk
			MAX3DS_CHUNK chunk3;
			chunk3.id = OBJ_MESH; // 0x4100
			
			// The objects vertices
			MAX3DS_CHUNK chunk4_1;
			chunk4_1.id = MESH_VERTICES; // 0x4110
			chunk4_1.size += (sizeof(unsigned short) + p.indexCount*3*4);
			
			// The UV Map
			MAX3DS_CHUNK chunk4_2;
			chunk4_2.id = MESH_UV; // 0x4140
			chunk4_2.size += (sizeof(unsigned short) + p.indexCount*2*4);

			// The objects faces
			MAX3DS_CHUNK chunk4_3;
			chunk4_3.id = MESH_FACEL1; // 0x4120
			numFaces = p.indexCount / 3;
			chunk4_3.size += (sizeof(unsigned short) + numFaces*4*2);

			// Material
			wxString matName = wxString::Format(_T("Material_%i"), i);
			matName.Append(_T('\0'));
			MAX3DS_CHUNK chunk4_3_1;
			chunk4_3_1.id = MESH_MATERIAL; // 0x4130
			chunk4_3_1.size += (unsigned int)matName.length();
			chunk4_3_1.size += (sizeof(unsigned short) + numFaces*2);
			chunk4_3.size += chunk4_3_1.size;

			// Smooth
			MAX3DS_CHUNK chunk4_3_2;
			chunk4_3_2.id = MESH_SMOOTH; // 0x4150
			chunk4_3_2.size += (numFaces*4);
			chunk4_3.size += chunk4_3_2.size;

			chunk3.size += chunk4_1.size;
			chunk3.size += chunk4_2.size;
			chunk3.size += chunk4_3.size;
			chunk2_4.size += chunk3.size;
			chunk1.size += chunk2_4.size;

			f.Write(&chunk2_4, sizeof(MAX3DS_CHUNK));
			f.Write(modName.data(), modName.length());
			f.Write(&chunk3, sizeof(MAX3DS_CHUNK));

			// Mesh chunk Vertice count, and vertices 0x4110
			f.Write(&chunk4_1, sizeof(MAX3DS_CHUNK));
			f.Write(&p.indexCount, sizeof(unsigned short));
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				Vec3D vert;
				if (init == false) {
					vert.x = (m->vertices[a].x);
					vert.y = (m->vertices[a].y);
					vert.z = (m->vertices[a].z);
				} else {
					vert.x = (m->origVertices[a].pos.x);
					vert.y = (m->origVertices[a].pos.y);
					vert.z = (m->origVertices[a].pos.z);
				}
				f.Write(reinterpret_cast<char *>(&vert.x), 4);
				f.Write(reinterpret_cast<char *>(&vert.y), 4);
				f.Write(reinterpret_cast<char *>(&vert.z), 4);
			}

			// The UV Map 0x4140
			f.Write(&chunk4_2, sizeof(MAX3DS_CHUNK));
			f.Write(&p.indexCount, sizeof(unsigned short));
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				float x = m->origVertices[a].texcoords.x;
				float y = (1 - m->origVertices[a].texcoords.y);
				f.Write(reinterpret_cast<char *>(&x), 4);
				f.Write(reinterpret_cast<char *>(&y), 4);
			}		

			// 0x4160
			// 0x4165

			// Face List 0x4120
			f.Write(&chunk4_3, sizeof(MAX3DS_CHUNK));
			int count = 0, unkid = 0;
			f.Write(&numFaces, sizeof(unsigned short));
			for (uint16 k=0; k<p.indexCount; k+=3) {
				f.Write(&count, 2);
				count++;
				f.Write(&count, 2);
				count++;
				f.Write(&count, 2);
				count++;
				f.Write(&unkid, 2);
			}

			// Face List - Material 0x4130
			f.Write(&chunk4_3_1, sizeof(MAX3DS_CHUNK));
			f.Write(matName.data(), matName.length());
			f.Write(&numFaces, sizeof(unsigned short));
			count = 0;
			for (uint16 k=0; k<p.indexCount; k+=3) {
				f.Write(&count, 2);
				count++;
			}

			// Face List - Smooth 0x4150
			f.Write(&chunk4_3_2, sizeof(MAX3DS_CHUNK));
			count = 1;
			for (uint16 k=0; k<p.indexCount; k+=3) {
				f.Write(&count, 4);
			}
		}
	}
	header.primary.size += chunk1.size;

	f.SeekO(0, wxFromStart);
	f.Write(&header, sizeof(MAX3DS_HEADER));
	f.Write(&chunk1, sizeof(MAX3DS_CHUNK)); // EDIT3DS
	f.SeekO(0, wxFromEnd);

	f.Close();
}