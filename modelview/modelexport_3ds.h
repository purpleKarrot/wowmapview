#ifndef MODELEXPORT_3DS_H
#define MODELEXPORT_3DS_H

#pragma pack(push)
#pragma pack(2)

//------ Primary chunk, at the beginning of each file
#define MAIN3DS       0x4D4D	// "MM"

//------ Main Chunks
//OBJECTINFO   =      long("0x3D3D",16);      #This gives the version of the mesh and is found right before the material and object information
#define EDIT3DS			0x3D3D  // this is the start of the editor config
#define VERSION3DS		0x0002	// This gives the version of the .3ds file
#define EDITKEYFRAME3DS 0xB000  // this is the start of the keyframer config

//------ sub defines of EDIT3DS / OBJECTINFO
#define EDIT_MATERIAL 0xAFFF // This stores the texture info
#define EDIT_CONFIG1  0x0100
#define EDIT_CONFIG2  0x3E3D
#define EDIT_VIEW_P1  0x7012
#define EDIT_VIEW_P2  0x7011
#define EDIT_VIEW_P3  0x7020
#define EDIT_VIEW1    0x7001
#define EDIT_BACKGR   0x1200
#define EDIT_AMBIENT  0x2100
#define EDIT_OBJECT   0x4000 // This stores the faces, vertices, etc...
#define MESH_VERSION  0x3D3E

//#define EDIT_UNKNW01  0x1100
//#define EDIT_UNKNW02  0x1201
//#define EDIT_UNKNW03  0x1300
//#define EDIT_UNKNW04  0x1400
//#define EDIT_UNKNW05  0x1420
//#define EDIT_UNKNW06  0x1450
//#define EDIT_UNKNW07  0x1500
//#define EDIT_UNKNW08  0x2200
//#define EDIT_UNKNW09  0x2201
//#define EDIT_UNKNW10  0x2210
//#define EDIT_UNKNW11  0x2300
//#define EDIT_UNKNW12  0x2302 
//#define EDIT_UNKNW13  0x3000
//#define EDIT_UNKNW14  0xAFFF

//------ sub defines of EDIT_MATERIAL
#define MATNAME			0xA000 // This holds the material name
#define MATAMBIENT		0xA010 // Ambient color of the object/material
#define MATDIFFUSE		0xA020 // This holds the color of the object/material
#define MATSPECULAR		0xA030 // SPecular color of the object/material
#define MATSHINESS		0xA040 // Material - Shininess
#define MATSHINSTRENGTH	0xA041 // Material - Shine Strength
#define MATTRANSPARENCY	0xA050 // Material - Transparency
#define	MATFALLOFF		0xA052
#define	MATBLUR			0xA053
#define	MATTYPE			0xA100 // Material - Type (Flat,Gourad, Phong, Metal)
#define MATMAP			0xA200 // This is a header for a new material
#define MATMAPFILE		0xA300 // This holds the file name of the texture
#define	MATFLAGS		0xA351 // Material - Texture Options
#define	MATSCALEX		0xA354 // Material - Texture U Scale
#define	MATSCALEY		0xA356 // Material - Texture V Scale
#define	MATOFFSETX		0xA35A // Material - Texture V Offset
#define	MATOFFSETY		0xA35C // Material - Texture V Offset

//------ sub defines of EDIT_OBJECT
#define OBJ_MESH		0x4100 // This lets us know that we are reading a new object
#define	OBJ_VERTEX		0x4110 // Vertex List
#define	OBJ_VERTEX_OPT	0x4111 // Vertex Options
#define	OBJ_FACE		0x4120 // Face List
#define	OBJ_MAT_DESC	0x4130 // Material Desc
#define	OBJ_UV_MAP		0x4140 // UV MAP List
#define OBJ_LIGHT		0x4600 // This lets un know we are reading a light object
#define OBJ_CAMERA		0x4700 // This lets un know we are reading a camera object

//#define OBJ_UNKNWN01  0x4010
//#define OBJ_UNKNWN02  0x4012 //---- Could be shadow

//------ sub defines of OBJ_CAMERA
//#define CAM_UNKNWN01	0x4710 
#define CAM_RANGES		0x4720 // The camera range values

//------ sub defines of OBJ_LIGHT
#define LIT_OFF       0x4620
#define LIT_SPOT      0x4610
#define LIT_UNKNWN01  0x465A

 //------ sub defines of OBJ_MESH
#define MESH_VERTICES	0x4110 // The objects vertices
#define MESH_FACEL2		0x4111 // ?
#define MESH_FACEL1		0x4120 // The objects faces
#define MESH_MATERIAL	0x4130 // This is found if the object has a material, either texture map or color
#define MESH_UV			0x4140 // The UV texture coordinates
#define MESH_SMOOTH		0x4150 // ?
#define MESH_LOCAL		0x4160 // The Object Matrix - Trans Matrix
#define MESH_VISIBLE	0x4165 // ?

//------ sub defs of KEYF3DS
#define KEYF_UNKNWN01 0xB009
#define KEYF_UNKNWN02 0xB00A
#define KEYF_FRAMES   0xB008
#define KEYF_OBJDES   0xB002

//------  these define the different color / material chunk types
#define COL_RGB	0x0010
#define COL_TRU	0x0011
#define RGB1	0x0011
#define RGB2	0x0012
#define COL_UNK	0x0013

//------ defines for viewport chunks
#define TOP           0x0001
#define BOTTOM        0x0002
#define LEFT          0x0003
#define RIGHT         0x0004
#define FRONT         0x0005
#define BACK          0x0006
#define USER          0x0007
#define CAMERA        0x0008 // 0xFFFF is the actual code read from file
#define LIGHT         0x0009
#define DISABLED      0x0010
#define BOGUS         0x0011

#ifdef _MSC_VER
#define ALIGN_2_S __declspec(align(2))
#define ALIGN_2_E
#else
#define ALIGN_2_S
#define ALIGN_2_E __attribute__((aligned(2)))
#endif
ALIGN_2_S
struct MAX3DS_CHUNK {
	unsigned short id;
	unsigned int size;

	MAX3DS_CHUNK() : size(6) {};
}
ALIGN_2_E;


// Index / Indice into the vertex list
ALIGN_2_S
struct MAX3DS_FACE {
	unsigned short x;
	unsigned short y;
	unsigned short z;
}
ALIGN_2_E;

ALIGN_2_S
struct MAX3DS_HEADER {
	MAX3DS_CHUNK primary;
	MAX3DS_CHUNK version;
	unsigned int nVersion;

	MAX3DS_HEADER() {
		primary.id = MAIN3DS;

		version.id = VERSION3DS;
		version.size = 10;
		nVersion = 3;

		primary.size += version.size;
	};

}
ALIGN_2_E;

struct ColorRGBs {
	char r;
	char g;
	char b;
};

#pragma pack(pop)

#endif
