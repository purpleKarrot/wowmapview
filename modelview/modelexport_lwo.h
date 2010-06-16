#ifndef MODELEXPORT_LWO_H
#define MODELEXPORT_LWO_H

#define MAX_POINTS_PER_POLYGON 1023

// -----------------------------------------
// New LW Header Stuff
// -----------------------------------------

int i32;
uint32 u32;
float f32;
uint16 u16;
unsigned char ub;

// Writing Functions
void LW_WriteVX(wxFFileOutputStream &f, uint32 p, uint32 &Size); // Write Lightwave VX Data
void LW_WriteSurface(wxFFileOutputStream &f, wxString surfName, Vec4D Color, float reflect, bool cull, uint32 surfID, wxString comment, uint32 &fileSize);

// To Be Removed, once we get PolyChunk32 fully installed.
struct PolyChunk16 {
	uint16 numVerts;
	uint16 indice[3];
};

// With this, we gotta mask out the un-needed indice bytes if it's less than 0xFF
struct PolyChunk32 {
	uint16 numVerts;
	uint32 indice[3];
};

// Animation Data
struct AnimationData {
	std::vector<Vec3D> Position;
	std::vector<Vec3D> Rotation;
	std::vector<Vec3D> Scale;
	std::vector<uint32> Time;

	void Push(Vec3D position, Vec3D rotation, Vec3D scale, uint32 time){
		Position.push_back(position);
		Rotation.push_back(rotation);
		Scale.push_back(scale);
		Time.push_back(time);
	}
	uint32 Size(){
		return (uint32)Time.size();
	}
};

// Weight Data
struct LWWeight{
	uint16 WeightID;
	float Value;
};

struct LWVertexColor{
	uint32 Index;
	uint8 r, g, b, a;
};

// Point Chunk Data
struct LWPoint {
	Vec3D PointData;
	std::vector<LWWeight> WeightData;
	LWVertexColor VertexColors;
};

// Poly Chunk Data
struct LWPoly {
	PolyChunk32 PolyData;
	uint32 PartTagID;
	uint32 SurfTagID;
};

// -= Lightwave Chunk Structures =-

// Layer Data
struct LWLayer {
	// Layer Data
	wxString Name;					// Name of the Layer

	// Points Block
	unsigned long long PointCount;	// Number of Points in this Layer
	std::vector<LWPoint>Points;		// Various Point Blocks used by this layer.
	Vec3D BoundingBox1;
	Vec3D BoundingBox2;
	// Point UV Data

	// Poly Block
	unsigned long PolyCount;
	std::vector<LWPoly> Polys;
	// Poly UV Data

};

// Clip Data
struct LWClip {
	wxString Filename;	// The Path & Filename of the image to be used in Lightwave
	wxString Source;	// The Source Path & Filename, used in WoW.
	uint32 TagID;		// = Number of Parts + Texture number
};

// Surface Chunk Data
struct LWSurface {
	wxString Name;		// The Surface's Name

	// Colors & Values
	// NYI
};

// The Master Structure for each individual LWO file.
struct LWObject {
	std::vector<wxString> PartNames;	// List of names for all the Parts;
	std::vector<LWLayer> Layers;		// List of Layers (usually 1) that make up the Geometery.
	std::vector<LWClip> Images;			// List of all the Unique Images used in the model.
	std::vector<LWSurface> Surfaces;	// List of the Surfaces used in the model.
};

#endif