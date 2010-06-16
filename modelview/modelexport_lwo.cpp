#include <wx/wfstream.h>

#include "globalvars.h"
#include "modelexport.h"
#include "modelexport_lwo.h"
#include "modelcanvas.h"

//#include "CxImage/ximage.h"

//---------------------------------------------
// Scene Writing Functions
//---------------------------------------------

// Writes a single Key for an envelope.
void WriteLWSceneEnvKey(ofstream &fs, uint32 Chan, float value, float time, uint32 spline = 0)
{
	fs << _T("  Key ");				// Announces the start of a Key
	fs << value;				// The Key's Value;
	fs << _T(" " << time);			// Time, in seconds, a float. This can be negative, zero or positive. Keys are listed in the envelope in increasing time order.
	fs << _T(" " << spline);		// The curve type, an integer: 0 - TCB, 1 - Hermite, 2 - 1D Bezier (obsolete, equivalent to Hermite), 3 - Linear, 4 - Stepped, 5 - 2D Bezier
	fs << _T(" 0 0 0 0 0 0 \n");	// Curve Data 1-6, all 0s for now.
}

// Writes an entire channel with only 1 key.
// Use WriteLWSceneEnvArray for writing animations.
void WriteLWSceneEnvChannel(ofstream &fs, uint32 ChanNum, float value, float time, uint32 spline = 0)
{
	fs << _T("Channel " << ChanNum << "\n");	// Channel Number
	fs << _T("{ Envelope\n");
	fs << _T("  1\n");						// Number of Keys in this envelope.
	WriteLWSceneEnvKey(fs,ChanNum,value,time,spline);
	fs << _T("  Behaviors 1 1\n");			// Pre/Post Behaviors. Defaults to 1 - Constant.
	fs << _T("}\n");
}

// Used for writing the keyframes of an animation.
void WriteLWSceneEnvArray(ofstream &fs, uint32 ChanNum, std::vector<float> value, std::vector<float> time, std::vector<uint8> spline)
{
	fs << _T("Channel " << ChanNum << "\n");
	fs << _T("{ Envelope\n");
	fs << _T("  " << value.size() << "\n");
	for (uint32 n=0;n<value.size();n++){
		uint32 thisspline = 0;
		if (spline[n]) {
			thisspline = spline[n];
		}
		WriteLWSceneEnvKey(fs,ChanNum,value[n],time[n],thisspline);
	}

	fs << _T("  Behaviors 1 1\n");
	fs << _T("}\n");
}

// Writes the "Plugin" information for a scene object, light, camera &/or bones.
void WriteLWScenePlugin(ofstream &fs, wxString type, uint32 PluginCount, wxString PluginName, wxString Data = wxEmptyString)
{
	fs << _T("Plugin " << type << " " << PluginCount << " " << PluginName << "\n" << Data << "EndPlugin\n");
}

// Writes an Object or Null Object to the scene file.
void WriteLWSceneObject(ofstream &fs, wxString Filename, AnimationData AnimData, uint32 &ItemNumber, int Visibility = 7, bool isNull = false, int32 ParentNum = -1, wxString Label=wxEmptyString)
{
	bool isLabeled = false;
	bool isParented = false;
	if (!Label.IsEmpty())
		isLabeled = true;
	if (ParentNum > -1)
		isParented = true;
	if (Visibility > 7)
		Visibility = 7;
	if (Visibility < 0)
		Visibility = 0;

	if (isNull == true){
		fs << _T("AddNullObject");
	}else{
		fs << _T("LoadObjectLayer 1");
	}
	fs << _T(" 1" << wxString::Format(_T("%07x"),ItemNumber) << " " << Filename << "\nChangeObject 0\n");
	if (isLabeled)
		fs << _T("// " << Label << "\n");
	fs << _T("ShowObject "<<Visibility<<" -1 0.376471 0.878431 0.941176 \nGroup 0\nObjectMotion\nNumChannels 9\n");
	std::vector<uint8>splines;
	std::vector<float>Time;
	std::vector<float>PosX;
	std::vector<float>PosY;
	std::vector<float>PosZ;
	std::vector<float>RotX;
	std::vector<float>RotY;
	std::vector<float>RotZ;
	std::vector<float>ScaX;
	std::vector<float>ScaY;
	std::vector<float>ScaZ;
	for (unsigned int c=0;c<AnimData.Size();c++){
		splines.push_back(0);
		Time.push_back(AnimData.Time[c]);
		PosX.push_back(AnimData.Position[c].x);
		PosY.push_back(AnimData.Position[c].y);
		PosZ.push_back(-AnimData.Position[c].z);
		RotX.push_back(AnimData.Rotation[c].x);
		RotY.push_back(AnimData.Rotation[c].y);
		RotZ.push_back(AnimData.Rotation[c].z);
		ScaX.push_back(AnimData.Scale[c].x);
		ScaY.push_back(AnimData.Scale[c].y);
		ScaZ.push_back(AnimData.Scale[c].z);
	}
	// Position
	//WriteLWSceneEnvChannel(fs,1,Pos.y,0);
	WriteLWSceneEnvArray(fs,0,PosX,Time,splines);
	WriteLWSceneEnvArray(fs,1,PosY,Time,splines);
	WriteLWSceneEnvArray(fs,2,PosZ,Time,splines);
	// Rotation
	WriteLWSceneEnvArray(fs,3,RotX,Time,splines);
	WriteLWSceneEnvArray(fs,4,RotY,Time,splines);
	WriteLWSceneEnvArray(fs,5,RotZ,Time,splines);
	// Scale
	WriteLWSceneEnvArray(fs,6,ScaX,Time,splines);
	WriteLWSceneEnvArray(fs,7,ScaY,Time,splines);
	WriteLWSceneEnvArray(fs,8,ScaZ,Time,splines);

	fs << _T("PathAlignLookAhead 0.033\nPathAlignMaxLookSteps 10\nPathAlignReliableDist 0.001\n");
	if (isParented == true)
		fs << _T("ParentItem 1" << wxString::Format(_T("%07x"),ParentNum) << "\n");
	fs << _T("IKInitialState 0\nSubPatchLevel 3 3\nShadowOptions 7\n");

	fs << _T("\n");
	ItemNumber++;
}

// Writes an Object's Bone to the scene file.
void WriteLWSceneBone(ofstream &fs, wxString BoneName, int BoneType, Vec3D Pos, Vec3D Rot, float Length, uint32 BoneNumber, uint32 ParentObject, int32 ParentNum = -1)
{
	bool isParented = false;
	if (ParentNum > -1){
		isParented = true;
	}

	fs << _T("AddBone 4") << wxString::Format(_T("%07x"),BoneNumber) << _T("\nBoneName " << BoneName << "\n");
	fs << _T("ShowBone 1 -1 0.376471 0.878431 0.941176\nBoneActive 1" << "\n");
	fs << _T("BoneStrength 1\nScaleBoneStrength 1" << "\n");
	fs << _T("BoneRestPosition "<<Pos.x<<" "<<Pos.y<<" "<<Pos.z<< "\n");
	fs << _T("BoneRestDirection "<<Rot.x<<" "<<Rot.y<<" "<<Rot.z<< "\n");
	fs << _T("BoneRestLength "<<Length<< "\n");
	fs << _T("BoneType " << BoneType<< "\n");
	fs << _T("BoneMotion\nNumChannels 9\n");
	// Position
	WriteLWSceneEnvChannel(fs,0,Pos.x,0);
	WriteLWSceneEnvChannel(fs,1,Pos.y,0);
	WriteLWSceneEnvChannel(fs,2,Pos.z,0);
	// Rotation
	WriteLWSceneEnvChannel(fs,3,Rot.x,0);
	WriteLWSceneEnvChannel(fs,4,Rot.y,0);
	WriteLWSceneEnvChannel(fs,5,Rot.z,0);
	// Scale
	WriteLWSceneEnvChannel(fs,6,1,0);
	WriteLWSceneEnvChannel(fs,7,1,0);
	WriteLWSceneEnvChannel(fs,8,1,0);

	fs << _T("PathAlignLookAhead 0.033\nPathAlignMaxLookSteps 10\nPathAlignReliableDist 0.001\n");
	if (isParented == true){
		fs << _T("ParentItem 4" << wxString::Format(_T("%07x"),ParentNum) << "\n");
	}else{
		fs << _T("ParentItem 1" << wxString::Format(_T("%07x"),ParentObject) << "\n");
	}

	fs << _T("IKInitialState 0");

	fs << _T("\n");
}

// Write a Light to the Scene File
void WriteLWSceneLight(ofstream &fs, uint32 &lcount, Vec3D LPos, uint32 Ltype, Vec3D Lcolor, float Lintensity, bool useAtten, float AttenEnd, float defRange = 2.5, wxString prefix = wxEmptyString, uint32 ParentNum = -1)
{
	bool isParented = false;
	if (ParentNum > -1)
		isParented = true;
	if (!prefix.IsEmpty())
		prefix = _T(" "+prefix);
	if ((useAtten == true)&&(AttenEnd<=0))
		useAtten = false;


	fs << _T("AddLight 2" << wxString::Format(_T("%07x"),lcount) << "\n");
	//modelname[0] = toupper(modelname[0]);
	fs << _T("LightName " << prefix << "Light " << lcount+1 << "\n");
	fs << _T("ShowLight 1 -1 0.941176 0.376471 0.941176\n");	// Last 3 Numbers are Layout Color for the Light.
	fs << _T("LightMotion\n");
	fs << _T("NumChannels 9\n");
	// Position
	WriteLWSceneEnvChannel(fs,0,LPos.x,0);
	WriteLWSceneEnvChannel(fs,1,LPos.y,0);
	WriteLWSceneEnvChannel(fs,2,-LPos.z,0);
	// Rotation
	WriteLWSceneEnvChannel(fs,3,0,0);
	WriteLWSceneEnvChannel(fs,4,0,0);
	WriteLWSceneEnvChannel(fs,5,0,0);
	// Scale
	WriteLWSceneEnvChannel(fs,6,1,0);
	WriteLWSceneEnvChannel(fs,7,1,0);
	WriteLWSceneEnvChannel(fs,8,1,0);

	if (isParented)
		fs << _T("ParentItem 1" << wxString::Format(_T("%07x"),ParentNum) << "\n");

	// Light Color Reducer
	// Some lights have a color channel greater than 255. This reduces all the colors, but increases the intensity, which should keep it looking the way Blizzard intended.
	while ((Lcolor.x > 1)||(Lcolor.y > 1)||(Lcolor.z > 1)) {
		Lcolor.x = Lcolor.x * 0.99;
		Lcolor.y = Lcolor.y * 0.99;
		Lcolor.z = Lcolor.z * 0.99;
		Lintensity = Lintensity / 0.99;
	}

	fs << _T("LightColor " << Lcolor.x << " " << Lcolor.y << " " << Lcolor.z << "\n");
	fs << _T("LightIntensity " << Lintensity << "\n");

	// Process Light type & output!
	switch (Ltype) {
		// Omni Lights
		case 1:
		default:
			// Default to an Omni (Point) light.
			fs << _T("LightType 1\n");

			if (useAtten == true) {
				// Use Inverse Distance for the default Light Falloff Type. Should better simulate WoW Lights, until I can write a WoW light plugin for Lightwave...
				fs << _T("LightFalloffType 2\nLightRange " << AttenEnd << "\n");
			}else{
				// Default to these settings, which look pretty close...
				fs << _T("LightFalloffType 2\nLightRange " << defRange << "\n");
			}
			fs << _T("ShadowType 1\nShadowColor 0 0 0\n");
			WriteLWScenePlugin(fs,_T("LightHandler"),1,_T("PointLight"));
	}
	fs << _T("\n");
	lcount++;
}

// Data Writing Functions

// VX is Lightwave Shorthand for any Point Number, because Lightwave stores points differently if they're over a certain threshold.
void LW_WriteVX(wxFFileOutputStream &f, uint32 p, uint32 &Size){
	if (p < 0xFF00){
		uint16 indice = MSB2(p & 0x0000FFFF);
		f.Write(reinterpret_cast<char *>(&indice),2);
		Size += 2;
	}else{
		uint32 indice = MSB4<uint32>(p + 0xFF000000);
		f.Write(reinterpret_cast<char *>(&indice), 4);
		Size += 4;
	}
}

void LW_WriteSurface(wxFFileOutputStream &f, wxString surfName, Vec4D Color, float reflect, bool cull, uint32 surfID, wxString comment, uint32 &fileSize){
	#ifdef _DEBUG
		wxLogMessage(_T("LW Write Surface Vars:\nSurfName: %s\nColor: %f/%f/%f\nReflect Value: %f\nCulling: %s\nSurfaceID: %i\nComment: \"%s\""),surfName,Color.x,Color.y,Color.z,(reflect*10),(cull?_T("True"):_T("False")),surfID,comment);
	#endif
	int off_t;

	uint32 surfaceDefSize = 0;
	f.Write(_T("SURF"), 4);
	u32 = MSB4<uint32>(surfaceDefSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileSize += 8;

	surfName.Append(_T('\0'));
	if (fmod((float)surfName.length(), 2.0f) > 0)
		surfName.Append(_T('\0'));

	surfName.Append(_T('\0')); // ""
	surfName.Append(_T('\0')); // Evens out the Code.
	f.Write(surfName.data(), (int)surfName.length());

	surfaceDefSize += (uint32)surfName.length();

	// Surface Attributes
	// COLOUR, size 4, bytes 2
	f.Write(_T("COLR"), 4);
	u16 = 14; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);

	// value
	f32 = MSB4<float>(Color.x);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f32 = MSB4<float>(Color.y);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f32 = MSB4<float>(Color.z);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 20;

	// LUMI
	f.Write(_T("LUMI"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 0;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 12;

	// DIFF
	f.Write(_T("DIFF"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 1;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 12;

	// SPEC
	f.Write(_T("SPEC"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = reflect/2;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 12;

	// REFL
	f.Write(_T("REFL"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 0.0f;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 12;

	// TRAN
	f.Write(_T("TRAN"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = MSB4<float>(reflect);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 12;

	// GLOSSINESS
	f.Write(_T("GLOS"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	// Value
	// Set to 20%, because that seems right.
	f32 = 0.2f;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	surfaceDefSize += 12;

	// SMAN (Smoothing)
	f.Write(_T("SMAN"), 4);
	u16 = 4; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	// Smoothing is done in radiens. PI = 180 degree smoothing.
	f32 = (float)PI;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	surfaceDefSize += 10;

	// RFOP
	f.Write(_T("RFOP"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 8;

	// TROP
	f.Write(_T("TROP"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 8;

	// SIDE
	f.Write(_T("SIDE"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	if (cull == false){
		u16 = 3;
	}
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 8;

	// --
	// BLOK
	uint16 blokSize = 0;
	f.Write(_T("BLOK"), 4);
	f.Write(reinterpret_cast<char *>(&blokSize), 2);
	surfaceDefSize += 6;

	// IMAP
	f.Write(_T("IMAP"), 4);
	u16 = 50-8; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 0x80;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// CHAN
	f.Write(_T("CHAN"), 4);
	u16 = 4; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f.Write(_T("COLR"), 4);
	blokSize += 10;

	// OPAC
	f.Write(_T("OPAC"), 4);
	u16 = 8; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 1.0;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 14;

	// ENAB
	f.Write(_T("ENAB"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// NEGA
	f.Write(_T("NEGA"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 0;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;
/*
	// AXIS
	// This is only needed for Planar images. Everything but ADTs uses UV data.
	f.Write(_T("AXIS"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;
*/
	// TMAP
	f.Write(_T("TMAP"), 4);
	u16 = 98; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 6;

	// CNTR
	f.Write(_T("CNTR"), 4);
	u16 = 14; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 0.0;
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 20;

	// SIZE
	f.Write(_T("SIZE"), 4);
	u16 = 14; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 1.0;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 20;

	// ROTA
	f.Write(_T("ROTA"), 4);
	u16 = 14; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 0.0;
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 20;

	// FALL
	f.Write(_T("FALL"), 4);
	u16 = 16; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 0.0;
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 22;

	// OREF
	f.Write(_T("OREF"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// CSYS
	f.Write(_T("CSYS"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// end TMAP

	// PROJ
	f.Write(_T("PROJ"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 5;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// AXIS
	f.Write(_T("AXIS"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 2;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// IMAG
	f.Write(_T("IMAG"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = surfID;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// WRAP
	f.Write(_T("WRAP"), 4);
	u16 = 4; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 10;

	// WRPW
	f.Write(_T("WRPW"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 1;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 12;

	// WRPH
	f.Write(_T("WRPH"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 1;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 12;

	// VMAP
	f.Write(_T("VMAP"), 4);
	u16 = 8; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	wxString t = _T("Texture");
	t.Append(_T('\0'));
	f.Write(t.data(), t.length());
	blokSize += 14;

	// AAST
	f.Write(_T("AAST"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 1;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	blokSize += 12;

	// PIXB
	f.Write(_T("PIXB"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// Fix Blok Size
	surfaceDefSize += blokSize;
	off_t = -2-blokSize;
	f.SeekO(off_t, wxFromCurrent);
	u16 = MSB2(blokSize);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f.SeekO(0, wxFromEnd);
	// ================

	// CMNT
	f.Write(_T("CMNT"), 4);
	comment.Append(_T('\0'));
	if (fmod((float)comment.length(), 2.0f) > 0)
		comment.Append(_T('\0'));
	u16 = (uint16)comment.length(); // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f.Write(comment.data(), comment.length());
	surfaceDefSize += 6 + (uint32)comment.length();

	f.Write(_T("VERS"), 4);
	u16 = 4;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 950;
	f32 = MSB4<int32>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	surfaceDefSize += 10;
			
	// Fix Surface Size
	fileSize += surfaceDefSize;
	off_t = -4-surfaceDefSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(surfaceDefSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
}


//---------------------------------------------
// M2 Functions
//---------------------------------------------

/* LWO2

Links to helpful documents:
http://gpwiki.org/index.php/LWO
http://home.comcast.net/~erniew/lwsdk/docs/filefmts/lwo2.html
*/

//---------------------------------------------
// --==M2 Bones Scene File==--
//---------------------------------------------
// Exports a Model to a Lightwave Scene File.
void ExportM2toScene(Model *m, const char *fn, bool init){
	// Should we generate a scene file?
	// Wll only generate if there are bones, lights, or a Camera in the model.
	bool doreturn = false;
	if ((m->header.nLights == 0) && (m->header.nBones == 0)){
		doreturn = true;
	}
	if (modelExport_UseWMVPosRot == true)
		doreturn = false;
	if (m->hasCamera == true){
		doreturn = false;
	}
	if (doreturn == true){
		wxLogMessage(_T("M2 Scene Export: Did not find any reason to export a scene. Stopping Scene export."));
		return;
	}
	wxLogMessage(_T("M2 Scene Export Values:\n  nLights: %i\n  nBones: %i\n  hasCamera: %s\n  Use WMV Pos/Rot: %s\n  doreturn: %s"),m->header.nLights,m->header.nBones,(m->hasCamera?_T("true"):_T("false")),(modelExport_UseWMVPosRot?_T("true"):_T("false")),(doreturn?_T("true"):_T("false")));

	// Open file
	wxString SceneName = wxString(fn, wxConvUTF8).BeforeLast(_T('.'));
	SceneName << _T(".lws");

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << SceneName.BeforeLast(SLASH);
		Name << SceneName.AfterLast(SLASH);

		MakeDirs(Path,_T("Scenes"));

		SceneName.Empty();
		SceneName << Path << SLASH << _T("Scenes") << SLASH << Name;
	}
	if (modelExport_PreserveDir == true && m->modelType != MT_CHAR){
		wxString Path1, Path2, Name;
		Path1 << SceneName.BeforeLast(SLASH);
		Name << SceneName.AfterLast(SLASH);
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);

		MakeDirs(Path1,Path2);

		SceneName.Empty();
		SceneName << Path1 << SLASH << Path2 << SLASH << Name;
	}

	ofstream fs(SceneName.mb_str(), ios_base::out | ios_base::trunc);

	if (!fs.is_open()) {
		wxMessageBox(_T("Unable to open the scene file for exporting."),_T("Scene Export Failure"));
		wxLogMessage(_T("Error: Unable to open file \"%s\". Could not export the scene."), SceneName.c_str());
		return;
	}
	wxLogMessage(_T("Opened %s for writing..."),SceneName.c_str());
	SceneName = SceneName.AfterLast(SLASH);

	// File Top
	fs << _T("LWSC\n");
	fs << _T("5\n\n"); // I think this is a version-compatibility number...

	uint32 RangeEnd = 0;

	uint32 mcount = 0; // Model Count
	uint32 lcount = 0; // Light Count
	uint32 bcount = 0; // Bone Count

	Vec3D ZeroPos(0,0,0);
	Vec3D ZeroRot(0,0,0);
	Vec3D OneScale(1,1,1);

	// Objects/Doodads go here

	// Exported Object
	int ModelID = mcount;
	wxString Obj = wxString(fn, wxConvUTF8).AfterLast(SLASH);
	wxString objFilename = wxEmptyString;
	if (modelExport_LW_PreserveDir == true){
		objFilename << _T("Objects") << SLASH;
	}
	if (modelExport_PreserveDir == true){
		objFilename += wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
		objFilename << SLASH;
		objFilename.Replace(_T("\\"),_T("/"));
	}
	objFilename += Obj;

	AnimationData ObjData;
	Vec3D ObjPos = ZeroPos; //m->pos;
	Vec3D ObjRot = ZeroRot; //m->rot;
	if (modelExport_UseWMVPosRot == true){
		ObjPos = m->pos;
		ObjRot = m->rot;
	}
	float temp;
	temp = ObjRot.y;
	ObjRot.y = ObjRot.z;
	ObjRot.z = temp;
	ObjData.Push(ObjPos,(ObjRot/(float)RADIAN),OneScale,0);

	WriteLWSceneObject(fs,objFilename,ObjData,mcount);

	// Export Bones
	if (m->header.nBones > 0){
		fs << _T("BoneFalloffType 5\nFasterBones 1\n");
		for (uint16 x=0;x<m->header.nBones;x++){
			Bone *cbone = &m->bones[x];
			Vec3D Pos = cbone->pivot;
			if (cbone->parent > -1)
				Pos -= m->bones[cbone->parent].pivot;
			if (init == false){
				Pos = cbone->transPivot;
				if (cbone->parent > -1)
					Pos -= m->bones[cbone->parent].transPivot;
			}
			Pos.z = -Pos.z;
			WriteLWSceneBone(fs, wxString::Format(_T("Bone_%03i"),x), 1, Pos, Vec3D(0,0,0), 0.25, x, ModelID, cbone->parent);
		}
	}

	// Lighting Basics
	fs << _T("AmbientColor 1 1 1\nAmbientIntensity 0.25\nDoubleSidedAreaLights 1\n\n");

	// Lights

	// Camera data goes here.
	if (m->hasCamera == true){
		ModelCamera *cam = &m->cam;
		uint32 anim = m->animManager->GetAnim();
		uint32 CameraTargetID = mcount;
		AnimationData CamData;
		if (cam->tTarget.data[anim].size() > 1){
			for (unsigned int x=0;x<cam->tTarget.data[anim].size();x++){
				Vec3D a = cam->target + cam->tTarget.data[anim][x];
				uint32 ctime = cam->tTarget.times[anim][x]/30;
				a.x = -a.x;
				a.z = -a.z;
				CamData.Push(a,ZeroRot,OneScale,ctime/30);
				if (ctime > RangeEnd)
					RangeEnd = ctime;
			}
		}else{
			CamData.Push(cam->target,ZeroRot,OneScale,0);
		}

		WriteLWSceneObject(fs,_T("Camera Target"),CamData,mcount,0,true,ModelID);

		fs << _T("AddCamera 30000000\nCameraName Camera\nShowCamera 1 -1 0.125490 0.878431 0.125490\nCameraMotion\nNumChannels 6\n");

		if (cam->tPos.data[anim].size() > 1){
			std::vector<float> time, PosX, PosY, PosZ, ZeroFloat;
			std::vector<uint8> splines;
			// Animations
			for (unsigned int x=0;x<cam->tPos.data[anim].size();x++){
				// Position Data
				Vec3D p_val = cam->pos + cam->tPos.data[anim][x];
				uint32 ctime = cam->tPos.times[anim][x]/30;
				float p_time = ctime/30;
				if (ctime > RangeEnd)
					RangeEnd = ctime;
				splines.push_back(0);
				ZeroFloat.push_back(0);
				time.push_back(p_time);
				PosX.push_back(-p_val.x);
				PosY.push_back(p_val.y);
				PosZ.push_back(p_val.z);
			}
			WriteLWSceneEnvArray(fs,0,PosX,time,splines);
			WriteLWSceneEnvArray(fs,0,PosY,time,splines);
			WriteLWSceneEnvArray(fs,0,PosZ,time,splines);
			WriteLWSceneEnvArray(fs,0,ZeroFloat,time,splines);
			WriteLWSceneEnvArray(fs,0,ZeroFloat,time,splines);
			WriteLWSceneEnvArray(fs,0,ZeroFloat,time,splines);
		}else{
			WriteLWSceneEnvChannel(fs,0,cam->pos.x,0);
			WriteLWSceneEnvChannel(fs,1,cam->pos.y,0);
			WriteLWSceneEnvChannel(fs,2,-(cam->pos.z),0);
			WriteLWSceneEnvChannel(fs,3,0,0);
			WriteLWSceneEnvChannel(fs,4,0,0);
			WriteLWSceneEnvChannel(fs,5,0,0);
		}

		fs << _T("ParentItem 1" << wxString::Format(_T("%07x"),ModelID) << "\n");
		fs << _T("IKInitCustomFrame 0\nGoalStrength 1\nIKFKBlending 0\nIKSoftMin 0.25\nIKSoftMax 0.75\nCtrlPosItemBlend 1\nCtrlRotItemBlend 1\nCtrlScaleItemBlend 1\n\n");
		fs << _T("HController 1\nPController 1\nPathAlignLookAhead 0.033\nPathAlignMaxLookSteps 10\nPathAlignReliableDist 0.001\n");
		fs << _T("TargetItem 1"<<wxString::Format(_T("%07x"),CameraTargetID)<<"\n");
		fs << _T("ZoomFactor "<<(cam->fov*3.6)<<"\nZoomType 2\n");
		WriteLWScenePlugin(fs,_T("CameraHandler"),1,_T("Perspective"));	// Make the camera a Perspective camera
	}

	// Scene File Basics
	fs << _T("RenderRangeType 0\nFirstFrame 1\nLastFrame "<<RangeEnd<<"\n");
	fs << _T("FrameStep 1\nRenderRangeObject 0\nRenderRangeArbitrary 1-"<<RangeEnd<<"\n");
	fs << _T("PreviewFirstFrame 0\nPreviewLastFrame "<<RangeEnd<<"\nPreviewFrameStep 1\nCurrentFrame 0\nFramesPerSecond 30\nChangeScene 0\n\n");

	// Rendering Options
	// Raytrace Shadows enabled.
	fs << _T("RenderMode 2\nRayTraceEffects 1\nDepthBufferAA 0\nRenderLines 1\nRayRecursionLimit 16\nRayPrecision 6\nRayCutoff 0.01\nDataOverlayLabel  \nSaveRGB 0\nSaveAlpha 0\n");

	fs.close();
}

/*
NOTE!!
I've done some research into the LWO2 format. I have a HUGE commented section about it down in the WMO function that details the layout, and some of the byte info too.
I'll update this function once I re-tune the WMO function.
		-Kjasi
*/
void ExportM2toLWO(Attachment *att, Model *m, const char *fn, bool init)
{
	wxString file = wxString(fn, wxConvUTF8);

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << file.BeforeLast(SLASH);
		Name << file.AfterLast(SLASH);

		MakeDirs(Path,_T("Objects"));

		file.Empty();
		file << Path << SLASH << _T("Objects") << SLASH << Name;
	}
	if (m->modelType != MT_CHAR){
		if (modelExport_PreserveDir == true){
			wxString Path1, Path2, Name;
			Path1 << file.BeforeLast(SLASH);
			Name << file.AfterLast(SLASH);
			Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);

			MakeDirs(Path1,Path2);

			file.Empty();
			file << Path1 << SLASH << Path2 << SLASH << Name;
		}
	}

	wxFFileOutputStream f(file, _T("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), file.c_str());
		return;
	}
	LogExportData(_T("LWO"),wxString(fn, wxConvUTF8).BeforeLast(SLASH),_T("M2"));
	int off_t;
	uint32 counter=0;
	uint32 TagCounter=0;
	uint16 PartCounter=0;
	uint16 SurfCounter=0;
	unsigned short numVerts = 0;
	unsigned short numGroups = 0;
	uint16 dimension = 2;

	// LightWave object files use the IFF syntax described in the EA-IFF85 document. Data is stored in a collection of chunks. 
	// Each chunk begins with a 4-byte chunk ID and the size of the chunk in bytes, and this is followed by the chunk contents.

	//InitCommon(att, init);

	unsigned int fileLen = 0;

	// ===================================================
	// FORM		// Format Declaration
	//
	// Always exempt from the length of the file!
	// ===================================================
	f.Write(_T("FORM"), 4);
	f.Write(reinterpret_cast<char *>(&fileLen), 4);


	// ===================================================
	// LWO2
	//
	// Declares this is the Lightwave Object 2 file format.
	// LWOB is the first format. It doesn't have a lot of the cool stuff LWO2 has...
	// ===================================================
	f.Write(_T("LWO2"), 4);
	fileLen += 4;


	// ===================================================
	// TAGS
	//
	// Used for various Strings. Known string types, in order:
	//		Sketch Color Names
	//		Part Names
	//		Surface Names
	// ===================================================
	f.Write(_T("TAGS"), 4);
	uint32 tagsSize = 0;
	wxString TAGS;
	u32 = 0;
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;

	#ifdef _DEBUG
	// Debug Texture List
	wxLogMessage(_T("M2 Texture List for %s:"),wxString(m->fullname.c_str()));
	for (unsigned short i=0; i<m->TextureList.size(); i++) {
		wxLogMessage(_T("Texture List[%i] = %s"),i,wxString(m->TextureList[i]));
	}
	wxLogMessage(_T("M2 Texture List Complete for %s"),wxString(m->fullname.c_str()));
	#endif

	// Mesh & Slot names
	wxString meshes[19] = {_T("Hairstyles"), _T("Facial1"), _T("Facial2"), _T("Facial3"), _T("Braces"),
		_T("Boots"), wxEmptyString, _T("Ears"), _T("Wristbands"),  _T("Kneepads"), _T("Pants"), _T("Pants"),
		_T("Tarbard"), _T("Trousers"), wxEmptyString, _T("Cape"), wxEmptyString, _T("Eyeglows"), _T("Belt") };
	wxString slots[15] = {_T("Helm"), wxEmptyString, _T("Shoulder"), wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString,
		wxEmptyString, wxEmptyString, _T("Right Hand Item"), _T("Left Hand Item"), wxEmptyString, wxEmptyString, _T("Quiver") };

	// Part Names
	for (unsigned short p=0; p<m->passes.size(); p++) {
		if (m->passes[p].init(m)){
			int g = m->passes[p].geoset;

			wxString partName;
			int mesh = m->geosets[g].id / 100;
			if (m->modelType == MT_CHAR && mesh < 19 && meshes[mesh] != wxEmptyString){
				partName = wxString::Format(_T("Geoset %03i - %s"),g,meshes[mesh].c_str());
			}else{
				partName = wxString::Format(_T("Geoset %03i"),g);
			}

			partName.Append(_T('\0'));
			if (fmod((float)partName.length(), 2.0f) > 0)
				partName.Append(_T('\0'));
			f.Write(partName.data(), partName.length());
			tagsSize += (uint32)partName.length();
		}
	}


	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {						
						wxString partName;
						if (att->slot < 15 && slots[att->slot]!=wxEmptyString){
							partName = wxString::Format(_T("%s"),slots[att->slot].c_str());
						}else{
							partName = wxString::Format(_T("Slot %02i"),att->slot);
						}

						partName.Append(_T('\0'));
						if (fmod((float)partName.length(), 2.0f) > 0)
							partName.Append(_T('\0'));
						f.Write(partName.data(), partName.length());
						tagsSize += (uint32)partName.length();
					}
				}
			}
		}

		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							int thisslot = att2->children[j]->slot;
							wxString partName;
							if (thisslot < 15 && slots[thisslot]!=wxEmptyString){
								partName = wxString::Format(_T("Child %02i - %s"),j,slots[thisslot].c_str());
							}else{
								partName = wxString::Format(_T("Child %02i - Slot %02i"),j,att2->children[j]->slot);
							}

							partName.Append(_T('\0'));
							if (fmod((float)partName.length(), 2.0f) > 0)
								partName.Append(_T('\0'));
							f.Write(partName.data(), partName.length());
							tagsSize += (uint32)partName.length();
						}
					}
				}
			}
		}
	}

	#ifdef _DEBUG
	wxLogMessage(_T("M2 Part Names Written for %s"),m->fullname.c_str());
	#endif

	// Surface Name
	//wxString *surfArray = new wxString[m->passes.size()];
	for (unsigned short i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		if (p.init(m)){
	//		if (!surfArray[m->passes[i].tex]){
				wxString matName;
				matName = wxString(m->TextureList[p.tex].c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));
				
				if (matName.Len() == 0)
					matName = wxString::Format(_T("Material_%03i"), p.tex);

				matName.Append(_T('\0'));
				if (fmod((float)matName.length(), 2.0f) > 0)
					matName.Append(_T('\0'));
				f.Write(matName.data(), matName.length());
				tagsSize += (uint32)matName.length();
	//			surfArray[m->passes[i].tex] = m->TextureList[m->passes[i].tex];
	//		}
		}
	}


	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {					
						wxString matName = wxString(attM->TextureList[p.tex].c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));

						if (matName.Len() == 0)
							matName = wxString::Format(_T("Attach Material %03i"), p.tex);

						matName.Append(_T('\0'));
						if (fmod((float)matName.length(), 2.0f) > 0)
							matName.Append(_T('\0'));
						f.Write(matName.data(), matName.length());
						tagsSize += (uint32)matName.length();
					}
				}
			}
		}

		for (uint32 a=0; a<att->children.size(); a++) {
			Attachment *att2 = att->children[a];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							wxString matName = wxString(mAttChild->TextureList[p.tex].c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));
							int thisslot = att2->children[j]->slot;
							if (thisslot < 15 && slots[thisslot]!=wxEmptyString){
								if (matName == _T("Cape")) {
									wxString tex = wxString(mAttChild->name.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));
									if (tex.Len() > 0){
										matName = wxString::Format(_T("%s - %s"),slots[thisslot].c_str(),tex.c_str());
									}else{
										matName = wxString::Format(_T("%s - Surface"),slots[thisslot].c_str());
									}
								}else if (matName != wxEmptyString){
									matName = wxString::Format(_T("%s - %s"),slots[thisslot].c_str(),matName.c_str());
								}else {
									matName = wxString::Format(_T("%s - Material %02i"),slots[thisslot].c_str(),p.tex);
								}
							}

							if (matName.Len() == 0)
								matName = wxString::Format(_T("Child %02i - Material %03i"), j, p.tex);

							matName.Append(_T('\0'));
							if (fmod((float)matName.length(), 2.0f) > 0)
								matName.Append(_T('\0'));
							f.Write(matName.data(), matName.length());
							tagsSize += (uint32)matName.length();
						}
					}
				}
			}
		}
	}

	off_t = -4-tagsSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(tagsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	fileLen += tagsSize;
	// ================
	#ifdef _DEBUG
	wxLogMessage(_T("M2 Surface Names Written for %s"),m->fullname.c_str());
	#endif

	

	// ===================================================
	// LAYR
	//
	// Specifies the start of a new layer. Each layer has it's own Point & Poly
	// chunk, which tells it what data is on what layer. It's probably best
	// to only have 1 layer for now.
	// ===================================================
	f.Write(_T("LAYR"), 4);
	u32 = MSB4<uint32>(18);
	fileLen += 8;
	f.Write(reinterpret_cast<char *>(&u32), 4);
	ub = 0;
	for(int i=0; i<18; i++) {
		f.Write(reinterpret_cast<char *>(&ub), 1);
	}
	fileLen += 18;
	// ================
	#ifdef _DEBUG
	wxLogMessage(_T("M2 Layer Defined for %s"),m->fullname.c_str());
	#endif

	// --
	// POINTS CHUNK, this is the vertice data
	// The PNTS chunk contains triples of floating-point numbers, the coordinates of a list of points. The numbers are written 
	// as IEEE 32-bit floats in network byte order. The IEEE float format is the standard bit pattern used by almost all CPUs 
	// and corresponds to the internal representation of the C language float type. In other words, this isn't some bizarre 
	// proprietary encoding. You can process these using simple fread and fwrite calls (but don't forget to correct the byte 
	// order if necessary).
	uint32 pointsSize = 0;
	f.Write(_T("PNTS"), 4);
	u32 = MSB4<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;

	// output all the model vertice data
	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (uint32 k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				Vec3D vert;
				if ((init == false)&&(m->vertices)) {
					vert.x = MSB4<float>(m->vertices[a].x);
					vert.y = MSB4<float>(m->vertices[a].y);
					vert.z = MSB4<float>(0-m->vertices[a].z);
				} else {
					vert.x = MSB4<float>(m->origVertices[a].pos.x);
					vert.y = MSB4<float>(m->origVertices[a].pos.y);
					vert.z = MSB4<float>(0-m->origVertices[a].pos.z);
				}
				f.Write(reinterpret_cast<char *>(&vert.x), 4);
				f.Write(reinterpret_cast<char *>(&vert.y), 4);
				f.Write(reinterpret_cast<char *>(&vert.z), 4);
				pointsSize += 12;

				numVerts++;
			}
			numGroups++;
		}
	}

	// Output the Attachment vertice data
	if (att!=NULL){
		wxLogMessage(_T("Attachment found! Attempting to save Point Data..."));
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);
			wxLogMessage(_T("Loaded Attached Model %s for export."),attM->modelname.c_str());

			if (attM){
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

				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						wxLogMessage(_T("Exporting Point data for Attachment..."));
						for (uint32 k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
							uint16 a = attM->indices[b];
							Vec3D vert;
							if ((init == false)&&(attM->vertices)) {
								vert.x = MSB4<float>((attM->vertices[a].x * scale.x) + pos.x);
								vert.y = MSB4<float>((attM->vertices[a].y * scale.y) + pos.y);
								vert.z = MSB4<float>(0-(attM->vertices[a].z * scale.z) - pos.z);
							} else {
								vert.x = MSB4<float>((attM->origVertices[a].pos.x * scale.x) + pos.x);
								vert.y = MSB4<float>((attM->origVertices[a].pos.y * scale.y) + pos.y);
								vert.z = MSB4<float>(0-(attM->origVertices[a].pos.z * scale.z) - pos.z);
							}
							f.Write(reinterpret_cast<char *>(&vert.x), 4);
							f.Write(reinterpret_cast<char *>(&vert.y), 4);
							f.Write(reinterpret_cast<char *>(&vert.z), 4);
							pointsSize += 12;
							numVerts++;
						}
						numGroups++;
					}
				}
			}
		}

		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);
				wxLogMessage(_T("Loaded Attached 2nd Child Model %s for export."),mAttChild->fullname.c_str());

				if (mAttChild){
					int boneID = -1;
					Model *mParent = NULL;

					if (att2->parent) {
						mParent = static_cast<Model*>(att2->children[j]->parent->model);
						if (mParent)
							boneID = mParent->attLookup[att2->children[j]->id];
					}
					Vec3D pos(0,0,0);
					Vec3D scale(1,1,1);
					if (boneID>-1) {
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
							scale.x = mat.m[0][0];
							scale.y = mat.m[1][1];
							scale.z = mat.m[2][2];

							mat.translation(cbone.transPivot);

							pos.x = mat.m[0][3];
							pos.y = mat.m[1][3];
							pos.z = mat.m[2][3];
						}
					}

					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							wxLogMessage(_T("Exporting Point data for Attached 2nd Child..."));
							for (uint32 k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
								uint16 a = mAttChild->indices[b];
								Vec3D vert;
								if ((init == false)&&(mAttChild->vertices)) {
									vert.x = MSB4<float>((mAttChild->vertices[a].x * scale.x) + pos.x);
									vert.y = MSB4<float>((mAttChild->vertices[a].y * scale.y) + pos.y);
									vert.z = MSB4<float>(0-(mAttChild->vertices[a].z * scale.z) - pos.z);
								} else {
									vert.x = MSB4<float>((mAttChild->origVertices[a].pos.x * scale.x) + pos.x);
									vert.y = MSB4<float>((mAttChild->origVertices[a].pos.y * scale.y) + pos.y);
									vert.z = MSB4<float>(0-(mAttChild->origVertices[a].pos.z * scale.z) - pos.z);
								}
								f.Write(reinterpret_cast<char *>(&vert.x), 4);
								f.Write(reinterpret_cast<char *>(&vert.y), 4);
								f.Write(reinterpret_cast<char *>(&vert.z), 4);
								pointsSize += 12;
								numVerts++;
							}
							numGroups++;
						}
					}
				}
			}
		}
		wxLogMessage(_T("Finished Attachment Point Data"));
	}

	fileLen += pointsSize;
	off_t = -4-pointsSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	// ================
#ifdef _DEBUG
	wxLogMessage(_T("M2 Point Data Written for %s"),m->fullname.c_str());
#endif

/*
	// --
	// The bounding box for the layer, just so that readers don't have to scan the PNTS chunk to find the extents.
	f.Write(_T("BBOX"), 4);
	u32 = MSB4<uint32>(24);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	Vec3D vert;
	vert.x = MSB4<float>(m->header.ps.BoundingBox[0].x);
	vert.y = MSB4<float>(m->header.ps.BoundingBox[0].y);
	vert.z = MSB4<float>(m->header.ps.BoundingBox[0].z);
	f.Write(reinterpret_cast<char *>(&vert.x), 4);
	f.Write(reinterpret_cast<char *>(&vert.y), 4);
	f.Write(reinterpret_cast<char *>(&vert.z), 4);
	vert.x = MSB4<float>(m->header.ps.BoundingBox[1].x);
	vert.y = MSB4<float>(m->header.ps.BoundingBox[1].y);
	vert.z = MSB4<float>(m->header.ps.BoundingBox[1].z);
	f.Write(reinterpret_cast<char *>(&vert.x), 4);
	f.Write(reinterpret_cast<char *>(&vert.y), 4);
	f.Write(reinterpret_cast<char *>(&vert.z), 4);
	// ================
*/

	// --
	uint32 vmapSize = 0;


	//Vertex Mapping
	f.Write(_T("VMAP"), 4);
	u32 = MSB4<uint32>(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	// UV Data
	f.Write(_T("TXUV"), 4);
	dimension = MSB2(2);
	f.Write(reinterpret_cast<char *>(&dimension), 2);
	f.Write(_T("Texture"), 7);
	ub = 0;
	f.Write(reinterpret_cast<char *>(&ub), 1);
	vmapSize += 14;

	counter = 0;

	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)){
			for(uint32 k=0, b=p.indexStart;k<p.indexCount;k++,b++) {
				uint16 a = m->indices[b];

				LW_WriteVX(f,counter,vmapSize);

				f32 = MSB4<float>(m->origVertices[a].texcoords.x);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				f32 = MSB4<float>(1 - m->origVertices[a].texcoords.y);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				vmapSize += 8;
				counter++;
			}
		}
	}

	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						for(uint32 k=0, b=p.indexStart;k<p.indexCount;k++,b++) {
							uint16 a = attM->indices[b];

							LW_WriteVX(f,counter,vmapSize);

							f32 = MSB4<float>(attM->origVertices[a].texcoords.x);
							f.Write(reinterpret_cast<char *>(&f32), 4);
							f32 = MSB4<float>(1 - attM->origVertices[a].texcoords.y);
							f.Write(reinterpret_cast<char *>(&f32), 4);
							vmapSize += 8;
							counter++;
						}
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							for(uint32 k=0, b=p.indexStart;k<p.indexCount;k++,b++) {
								uint16 a = mAttChild->indices[b];

								LW_WriteVX(f,counter,vmapSize);

								f32 = MSB4<float>(mAttChild->origVertices[a].texcoords.x);
								f.Write(reinterpret_cast<char *>(&f32), 4);
								f32 = MSB4<float>(1 - mAttChild->origVertices[a].texcoords.y);
								f.Write(reinterpret_cast<char *>(&f32), 4);
								vmapSize += 8;
								counter++;
							}
						}
					}
				}
			}
		}
	}

	fileLen += vmapSize;

	off_t = -4-vmapSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);


	// ================
#ifdef _DEBUG
	wxLogMessage(_T("M2 UV data Written for %s"),m->fullname.c_str());
#endif

	//m->header;
	/*
	//Vertex Mapping
	f.Write(_T("VMAP"), 4);
	u32 = MSB4<uint32>(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	// UV Data
	f.Write(_T("WGHT"), 4);
	dimension = MSB2(1);
	f.Write(reinterpret_cast<char *>(&dimension), 2);
	f.Write(_T("Texture"), 7);
	ub = 0;
	f.Write(reinterpret_cast<char *>(&ub), 1);
	vmapSize += 14;

	counter = 0;

	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)){
			for(uint32 k=0, b=p.indexStart;k<p.indexCount;k++,b++) {
				uint16 a = m->indices[b];

				LW_WriteVX(f,counter,vmapSize);

				f32 = MSB4<float>(m->origVertices[a].texcoords.x);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				f32 = MSB4<float>(1 - m->origVertices[a].texcoords.y);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				vmapSize += 8;
				counter++;
			}
		}
	}

	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						for(uint32 k=0, b=p.indexStart;k<p.indexCount;k++,b++) {
							uint16 a = attM->indices[b];

							LW_WriteVX(f,counter,vmapSize);

							f32 = MSB4<float>(attM->origVertices[a].texcoords.x);
							f.Write(reinterpret_cast<char *>(&f32), 4);
							f32 = MSB4<float>(1 - attM->origVertices[a].texcoords.y);
							f.Write(reinterpret_cast<char *>(&f32), 4);
							vmapSize += 8;
							counter++;
						}
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							for(uint32 k=0, b=p.indexStart;k<p.indexCount;k++,b++) {
								uint16 a = mAttChild->indices[b];

								LW_WriteVX(f,counter,vmapSize);

								f32 = MSB4<float>(mAttChild->origVertices[a].texcoords.x);
								f.Write(reinterpret_cast<char *>(&f32), 4);
								f32 = MSB4<float>(1 - mAttChild->origVertices[a].texcoords.y);
								f.Write(reinterpret_cast<char *>(&f32), 4);
								vmapSize += 8;
								counter++;
							}
						}
					}
				}
			}
		}
	}

	fileLen += vmapSize;

	off_t = -4-vmapSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

#ifdef _DEBUG
	wxLogMessage(_T("M2 Weight data Written for %s"),m->fullname.c_str());
#endif
*/

	// --
	// POLYGON CHUNK
	// The POLS chunk contains a list of polygons. A "polygon" in this context is anything that can be described using an 
	// ordered list of vertices. A POLS of type FACE contains ordinary polygons, but the POLS type can also be CURV, 
	// PTCH, MBAL or BONE, for example.
	//
	// The high 6 bits of the vertex count for each polygon are reserved for flags, which in effect limits the number of 
	// vertices per polygon to 1023. Don't forget to mask the high bits when reading the vertex count. The flags are 
	// currently only defined for CURVs.
	// 
	// The point indexes following the vertex count refer to the points defined in the most recent PNTS chunk. Each index 
	// can be a 2-byte or a 4-byte integer. If the high order (first) byte of the index is not 0xFF, the index is 2 bytes long. 
	// This allows values up to 65279 to be stored in 2 bytes. If the high order byte is 0xFF, the index is 4 bytes long and 
	// its value is in the low three bytes (index & 0x00FFFFFF). The maximum value for 4-byte indexes is 16,777,215 (224 - 1). 
	// Objects with more than 224 vertices can be stored using multiple pairs of PNTS and POLS chunks.
	// 
	// The cube has 6 square faces each defined by 4 vertices. LightWave polygons are single-sided by default 
	// (double-sidedness is a possible surface property). The vertices are listed in clockwise order as viewed from the 
	// visible side, starting with a convex vertex. (The normal is defined as the cross product of the first and last edges.)

	f.Write(_T("POLS"), 4);
	uint32 polySize = 4;
	u32 = MSB4<uint32>(polySize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8; // FACE is handled in the PolySize
	f.Write(_T("FACE"), 4);

	counter = 0;
	
	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		if (p.init(m)) {
			for (unsigned int k=0; k<p.indexCount; k+=3) {
				uint16 nverts;

				// Write the number of Verts
				nverts = MSB2(3);
				f.Write(reinterpret_cast<char *>(&nverts),2);
				polySize += 2;

				for (int x=0;x<3;x++,counter++){
					//wxLogMessage("Batch %i, index %i, x=%i",b,k,x);
					//uint16 indice16;
					//uint32 indice32;

					int mod = 0;
					if (x==1){
						mod = 1;
					}else if (x==2){
						mod = -1;
					}

					LW_WriteVX(f,counter+mod,polySize);
				}
			}
		}
	}
	
	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						for (unsigned int k=0; k<p.indexCount; k+=3) {
							uint16 nverts;

							// Write the number of Verts
							nverts = MSB2(3);
							f.Write(reinterpret_cast<char *>(&nverts),2);
							polySize += 2;

							for (int x=0;x<3;x++,counter++){
								//wxLogMessage("Batch %i, index %i, x=%i",b,k,x);
								//uint16 indice16;
								//uint32 indice32;

								int mod = 0;
								if (x==1){
									mod = 1;
								}else if (x==2){
									mod = -1;
								}

								LW_WriteVX(f,counter+mod,polySize);
							}
						}
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							for (unsigned int k=0; k<p.indexCount; k+=3) {
								uint16 nverts;

								// Write the number of Verts
								nverts = MSB2(3);
								f.Write(reinterpret_cast<char *>(&nverts),2);
								polySize += 2;

								for (int x=0;x<3;x++,counter++){
									//wxLogMessage("Batch %i, index %i, x=%i",b,k,x);
									//uint16 indice16;
									//uint32 indice32;

									int mod = 0;
									if (x==1){
										mod = 1;
									}else if (x==2){
										mod = -1;
									}

									LW_WriteVX(f,counter+mod,polySize);
								}
							}
						}
					}
				}
			}
		}
	}

	off_t = -4-polySize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(polySize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	fileLen += polySize;
	// ========
#ifdef _DEBUG
	wxLogMessage(_T("M2 Polygons Written for %s"),m->fullname.c_str());
#endif

	// The PTAG chunk associates tags with polygons. In this case, it identifies which surface is assigned to each polygon. 
	// The first number in each pair is a 0-based index into the most recent POLS chunk, and the second is a 0-based 
	// index into the TAGS chunk.

	// NOTE: Every PTAG type needs a seperate PTAG call!

	TagCounter = 0;
	PartCounter = 0;
	SurfCounter = 0;
	counter = 0;
	uint32 ptagSize;

	// Parts PolyTag
	f.Write(_T("PTAG"), 4);
	ptagSize = 4;
	counter=0;
	u32 = MSB4<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write(_T("PART"), 4);
	for (unsigned int i=0;i<m->passes.size();i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)){
			for (unsigned int k=0; k<p.indexCount; k+=3) {
				LW_WriteVX(f,counter,ptagSize);

				u16 = MSB2(TagCounter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				ptagSize += 2;
				counter++;
			}
			TagCounter++;
			PartCounter++;
		}
	}
	
	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						for (unsigned int k=0; k<p.indexCount; k+=3) {
							LW_WriteVX(f,counter,ptagSize);

							u16 = MSB2(TagCounter);
							f.Write(reinterpret_cast<char *>(&u16), 2);
							ptagSize += 2;
							counter++;
						}
						TagCounter++;
						PartCounter++;
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							for (unsigned int k=0; k<p.indexCount; k+=3) {
								LW_WriteVX(f,counter,ptagSize);

								u16 = MSB2(TagCounter);
								f.Write(reinterpret_cast<char *>(&u16), 2);
								ptagSize += 2;
								counter++;
							}
							TagCounter++;
							PartCounter++;
						}
					}
				}
			}
		}
	}


	fileLen += ptagSize;

	off_t = -4-ptagSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);


	// Surface PolyTag
	counter=0;
	ptagSize = 4;
	f.Write(_T("PTAG"), 4);
	u32 = MSB4<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write(_T("SURF"), 4);
	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (unsigned int k=0; k<p.indexCount; k+=3) {
				LW_WriteVX(f,counter,ptagSize);

				u16 = MSB2(TagCounter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				ptagSize += 2;
				counter++;
			}
			TagCounter++;
			SurfCounter++;
		}
	}

	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						for (unsigned int k=0; k<p.indexCount; k+=3) {
							LW_WriteVX(f,counter,ptagSize);

							u16 = MSB2(TagCounter);
							f.Write(reinterpret_cast<char *>(&u16), 2);
							ptagSize += 2;
							counter++;
						}
						TagCounter++;
						SurfCounter++;
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							for (unsigned int k=0; k<p.indexCount; k+=3) {
								LW_WriteVX(f,counter,ptagSize);

								u16 = MSB2(TagCounter);
								f.Write(reinterpret_cast<char *>(&u16), 2);
								ptagSize += 2;
								counter++;
							}
							TagCounter++;
							SurfCounter++;
						}
					}
				}
			}
		}
	}
	
	fileLen += ptagSize;

	off_t = -4-ptagSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);


	// ================
#ifdef _DEBUG
	wxLogMessage(_T("M2 PTag Surface data Written for %s"),m->fullname.c_str());
#endif

	// --
	uint32 vmadSize = 0;
	f.Write(_T("VMAD"), 4);
	u32 = MSB4<uint32>(vmadSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write(_T("TXUV"), 4);
	dimension = 2;
	dimension = MSB2(dimension);
	f.Write(reinterpret_cast<char *>(&dimension), 2);
	f.Write(_T("Texture"), 7);
	ub = 0;
	f.Write(reinterpret_cast<char *>(&ub), 1);
	vmadSize += 14;

	counter = 0;
	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (uint32 k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				int a = m->indices[b];

				LW_WriteVX(f,counter,vmadSize);
				LW_WriteVX(f,(counter/3),vmadSize);

				f32 = MSB4<float>(m->origVertices[a].texcoords.x);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				f32 = MSB4<float>(1 - m->origVertices[a].texcoords.y);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				counter++;
				vmadSize += 8;
			}
		}
	}
	
	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						for (uint32 k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
							uint16 a = attM->indices[b];

							LW_WriteVX(f,counter,vmadSize);
							LW_WriteVX(f,(counter/3),vmadSize);

							f32 = MSB4<float>(attM->origVertices[a].texcoords.x);
							f.Write(reinterpret_cast<char *>(&f32), 4);
							f32 = MSB4<float>(1 - attM->origVertices[a].texcoords.y);
							f.Write(reinterpret_cast<char *>(&f32), 4);
							counter++;
							vmadSize += 8;
						}
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							for (uint32 k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
								uint16 a = mAttChild->indices[b];

								LW_WriteVX(f,counter,vmadSize);
								LW_WriteVX(f,(counter/3),vmadSize);

								f32 = MSB4<float>(mAttChild->origVertices[a].texcoords.x);
								f.Write(reinterpret_cast<char *>(&f32), 4);
								f32 = MSB4<float>(1 - mAttChild->origVertices[a].texcoords.y);
								f.Write(reinterpret_cast<char *>(&f32), 4);
								counter++;
								vmadSize += 8;
							}
						}
					}
				}
			}
		}
	}

	fileLen += vmadSize;
	off_t = -4-vmadSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(vmadSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	// ================
#ifdef _DEBUG
	wxLogMessage(_T("M2 VMAD data Written for %s"),m->fullname.c_str());
#endif

	

	// --
	uint32 surfaceCounter = PartCounter;
	
	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			int clipSize = 0;
			f.Write(_T("CLIP"), 4);
			u32 = MSB4<uint32>(0);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			fileLen += 8;

			u32 = MSB4<uint32>(++surfaceCounter);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.Write(_T("STIL"), 4);
			clipSize += 8;

#ifdef _DEBUG
			if (p.useTex2 != false){
				wxLogMessage(_T("Pass %i uses Texture 2!"),i);
			}
#endif
			
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

			wxString sTexName = wxEmptyString;
			if (modelExport_LW_PreserveDir == true){
				sTexName << _T("Images") << SLASH;
			}
			if (m->modelType != MT_CHAR){
				if (modelExport_PreserveDir == true){
					sTexName += texPath;
					sTexName << SLASH;
					sTexName.Replace(_T("\\"),_T("/"));
				}
			}
			sTexName += texName;

			sTexName << _T(".tga") << _T('\0');

			if (fmod((float)sTexName.length(), 2.0f) > 0)
				sTexName.Append(_T('\0'));

			u16 = MSB2((unsigned short)sTexName.length());
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.Write(sTexName.data(), sTexName.length());
			clipSize += (2+(int)sTexName.length());

			// update the chunks length
			off_t = -4-clipSize;
			f.SeekO(off_t, wxFromCurrent);
			u32 = MSB4<uint32>(clipSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.SeekO(0, wxFromEnd);

			// save texture to file
			wxString texFilename(fn, wxConvUTF8);
			texFilename = texFilename.BeforeLast(SLASH);
			texFilename += SLASH;
			texFilename += texName;

			if (modelExport_LW_PreserveDir == true){
				wxString Path, Name;

				Path << wxString(fn, wxConvUTF8).BeforeLast(SLASH);
				Name << texFilename.AfterLast(SLASH);

				MakeDirs(Path,_T("Images"));

				texFilename.Empty();
				texFilename << Path << SLASH<<_T("Images")<<SLASH << Name;
			}
			if (m->modelType != MT_CHAR){
				if (modelExport_PreserveDir == true){
					wxString Path1, Path2, Name;
					Path1 << texFilename.BeforeLast(SLASH);
					Name << texName.AfterLast(SLASH);
					Path2 << texPath;

					MakeDirs(Path1,Path2);

					texFilename.Empty();
					texFilename << Path1 << SLASH << Path2 << SLASH << Name;
				}
			}
			if (texFilename.Length() == 0){
				texFilename << wxString(m->modelname.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) << wxString::Format(_T("_Image_%03i"),i);
			}
			texFilename << _T(".tga");
			wxLogMessage(_T("Exporting Image: %s"),texFilename.c_str());
			SaveTexture(texFilename);

			fileLen += clipSize;
		}
	}

	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						int clipSize = 0;
						f.Write(_T("CLIP"), 4);
						u32 = MSB4<uint32>(0);
						f.Write(reinterpret_cast<char *>(&u32), 4);
						fileLen += 8;

						u32 = MSB4<uint32>(++surfaceCounter);
						f.Write(reinterpret_cast<char *>(&u32), 4);
						f.Write(_T("STIL"), 4);
						clipSize += 8;

						wxString FilePath = wxString(fn, wxConvUTF8).BeforeLast(SLASH);
						wxString texName = wxString(attM->TextureList[p.tex].c_str(), wxConvUTF8).BeforeLast(_T('.'));
						wxString texPath = texName.BeforeLast(SLASH);
						if (texName.AfterLast(SLASH) == _T("Cape")){
							texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + wxString(attM->name.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_Replacable");
							texPath = wxString(fn, wxConvUTF8).BeforeLast(SLASH);
						}else{
							texName = texName.AfterLast(SLASH);
						}

						if (texName.Length() == 0){
							texName << wxString(attM->modelname.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast('.');
							texPath = wxString(attM->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
						}

/*
						//texName = attM->TextureList[p.tex];

						if ((texName.Find(SLASH) <= 0)&&(texName == _T("Cape"))){
							texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_Replacable");
							texPath = wxString(attM->name).BeforeLast(SLASH);
						}else{
							texName = texName.AfterLast(SLASH);
						}

						if (texName.Length() == 0)
							texName << wxString(attM->modelname).AfterLast(SLASH).BeforeLast(_T('.')) << wxString::Format(_T("_Image_%03i"),i);
*/
						wxString sTexName = wxEmptyString;
						if (modelExport_LW_PreserveDir == true){
							sTexName << _T("Images") << SLASH;
						}
						if (modelExport_PreserveDir == true){
							sTexName += texPath;
							sTexName << SLASH;
							sTexName.Replace(_T("\\"),_T("/"));
						}
						sTexName += texName;

						sTexName << _T(".tga") << _T('\0');
						if (fmod((float)sTexName.length(), 2.0f) > 0)
							sTexName.Append(_T('\0'));

						u16 = MSB2((unsigned short)sTexName.length());
						f.Write(reinterpret_cast<char *>(&u16), 2);
						f.Write(sTexName.data(), sTexName.length());
						clipSize += (2+(int)sTexName.length());

						// update the chunks length
						off_t = -4-clipSize;
						f.SeekO(off_t, wxFromCurrent);
						u32 = MSB4<uint32>(clipSize);
						f.Write(reinterpret_cast<char *>(&u32), 4);
						f.SeekO(0, wxFromEnd);

						// save texture to file
						wxString texFilename(fn, wxConvUTF8);
						texFilename = texFilename.BeforeLast(SLASH);
						texFilename += SLASH;
						texFilename += texName;

						if (modelExport_LW_PreserveDir == true){
							wxString Path, Name;

							Path << wxString(fn, wxConvUTF8).BeforeLast(SLASH);
							Name << texFilename.AfterLast(SLASH);

							MakeDirs(Path,_T("Images"));

							texFilename.Empty();
							texFilename << Path << SLASH<<_T("Images")<<SLASH << Name;
						}
						if (modelExport_PreserveDir == true){
							wxString Path1, Path2, Name;
							Path1 << texFilename.BeforeLast(SLASH);
							Name << texName.AfterLast(SLASH);
							Path2 << texPath;

							MakeDirs(Path1,Path2);

							texFilename.Empty();
							texFilename << Path1 << SLASH << Path2 << SLASH << Name;
						}
						
						if (texFilename.Length() == 0){
							texFilename << wxString(attM->modelname.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast('.') << wxString::Format(_T("_Image_%03i"),i);
						}
						texFilename << _T(".tga");
						wxLogMessage(_T("Exporting Image: %s"),texFilename.c_str());
						SaveTexture(texFilename);

						fileLen += clipSize;
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 k=0; k<mAttChild->passes.size(); k++) {
						ModelRenderPass &p = mAttChild->passes[k];

						if (p.init(mAttChild)) {
							int clipSize = 0;
							f.Write(_T("CLIP"), 4);
							u32 = MSB4<uint32>(0);
							f.Write(reinterpret_cast<char *>(&u32), 4);
							fileLen += 8;

							u32 = MSB4<uint32>(++surfaceCounter);
							f.Write(reinterpret_cast<char *>(&u32), 4);
							f.Write(_T("STIL"), 4);
							clipSize += 8;

							wxString FilePath = wxString(fn, wxConvUTF8).BeforeLast(SLASH);
							wxString texName = wxString(mAttChild->TextureList[p.tex].c_str(), wxConvUTF8).BeforeLast(_T('.'));
							wxString texPath = texName.BeforeLast(SLASH);
							if (texName.AfterLast(SLASH) == _T("Cape")){
								//texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + wxString(mAttChild->name).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_Replacable");
								texName = wxString(mAttChild->name.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));
								texPath = wxString(mAttChild->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
							}else{
								texName = texName.AfterLast(SLASH);
							}

							if (texName.Length() == 0){
								texName << wxString(mAttChild->modelname.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));
								texPath = wxString(mAttChild->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
							}

							wxString sTexName = wxEmptyString;
							if (modelExport_LW_PreserveDir == true){
								sTexName << _T("Images") << SLASH;
							}
							if (modelExport_PreserveDir == true){
								sTexName += texPath;
								sTexName << SLASH;
								sTexName.Replace(_T("\\"),_T("/"));
							}
							sTexName += texName;

							sTexName << _T(".tga") << _T('\0');
							if (fmod((float)sTexName.length(), 2.0f) > 0)
								sTexName.Append(_T('\0'));

							u16 = MSB2((unsigned short)sTexName.length());
							f.Write(reinterpret_cast<char *>(&u16), 2);
							f.Write(sTexName.data(), sTexName.length());
							clipSize += (2+(int)sTexName.length());

							// update the chunks length
							off_t = -4-clipSize;
							f.SeekO(off_t, wxFromCurrent);
							u32 = MSB4<uint32>(clipSize);
							f.Write(reinterpret_cast<char *>(&u32), 4);
							f.SeekO(0, wxFromEnd);

							// save texture to file
							wxString texFilename(fn, wxConvUTF8);
							texFilename = texFilename.BeforeLast(SLASH);
							texFilename += SLASH;
							texFilename += texName;

							if (modelExport_LW_PreserveDir == true){
								wxString Path, Name;

								Path << wxString(fn, wxConvUTF8).BeforeLast(SLASH);
								Name << texFilename.AfterLast(SLASH);

								MakeDirs(Path,_T("Images"));

								texFilename.Empty();
								texFilename << Path << SLASH<<_T("Images")<<SLASH << Name;
							}
							if (modelExport_PreserveDir == true){
								wxString Path1, Path2, Name;
								Path1 << texFilename.BeforeLast(SLASH);
								Name << texName.AfterLast(SLASH);
								Path2 << texPath;

								MakeDirs(Path1,Path2);

								texFilename.Empty();
								texFilename << Path1 << SLASH << Path2 << SLASH << Name;
							}
							
							if (texFilename.Length() == 0){
								texFilename << wxString(attM->modelname.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast('.') << wxString::Format(_T("_Image_%03i"),i);
							}
							texFilename << _T(".tga");
							wxLogMessage(_T("Exporting Image: %s"),texFilename.c_str());
							SaveTexture(texFilename);

							fileLen += clipSize;
						}
					}
				}
			}
		}
	}

	// ================
#ifdef _DEBUG
	wxLogMessage(_T("M2 Images & Image Data Written for %s"),m->fullname.c_str());
#endif

	// --
	wxString surfName;
	surfaceCounter = PartCounter;
	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			surfaceCounter++;

			// Surface name
			surfName = wxString(m->TextureList[p.tex].c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));
			if (surfName.Len() == 0)
				surfName = wxString::Format(_T("Material_%03i"),p.tex);

			Vec4D color = p.ocol;
			float reflect = 0.0f;
			if (p.useEnvMap)
				reflect = 0.5f;

			wxString cmnt;
			if (m->modelType == MT_CHAR){
				cmnt = wxString::Format(_T("Character Material %03i"),p.tex);
			}else{
				cmnt = wxString(m->TextureList[p.tex].c_str(), wxConvUTF8).BeforeLast(_T('.')).Append(_T(".TGA"));
			}

			// If double-sided...
			bool cull = true;
			if (p.cull == false)
				cull = false;
			LW_WriteSurface(f,surfName,color,reflect,cull,surfaceCounter,cmnt,fileLen);

		}
	}

	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						surfaceCounter++;

						// Surface name
						surfName = wxString(attM->TextureList[p.tex].c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));
						if (surfName.Len() == 0)
							surfName = wxString::Format(_T("Material_%03i"),p.tex);
						Vec4D color = p.ocol;
						float reflect = 0.0f;
						if (p.useEnvMap)
							reflect = 0.5f;

						wxString cmnt = wxString(attM->TextureList[p.tex].c_str(), wxConvUTF8).BeforeLast(_T('.')).Append(_T(".TGA"));

						// If double-sided...
						bool cull = true;
						if (p.cull == false)
							cull = false;
						LW_WriteSurface(f,surfName,color,reflect,cull,surfaceCounter,cmnt,fileLen);
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							surfaceCounter++;

							// Comment
							wxString cmnt = wxString(mAttChild->TextureList[p.tex].c_str(), wxConvUTF8).BeforeLast(_T('.')).Append(_T(".TGA"));

							// Surface name
							surfName = wxString(mAttChild->TextureList[p.tex].c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));
							int thisslot = att2->children[j]->slot;
							if (thisslot < 15 && slots[thisslot]!=wxEmptyString){
								if (surfName == _T("Cape")) {
									wxString tex = wxString(mAttChild->name.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));
									if (tex.Len() > 0){
										surfName = wxString::Format(_T("%s - %s"),slots[thisslot].c_str(),tex.c_str());
										cmnt = wxString(mAttChild->name.c_str(), wxConvUTF8).BeforeLast(_T('.')).Append(_T(".tga"));
									}else{
										surfName = wxString::Format(_T("%s - Surface"),slots[thisslot].c_str());
										cmnt = _T("Surface");
									}
								}else if (surfName != wxEmptyString){
									surfName = wxString::Format(_T("%s - %s"),slots[thisslot].c_str(),surfName.c_str());
								}else{
									surfName = wxString::Format(_T("%s - Material %02i"),slots[thisslot].c_str(),p.tex);
									cmnt = surfName;
								}
							}
							if (surfName.Len() == 0)
								surfName = wxString::Format(_T("Child %02i - Material %03i"), j, p.tex);

							Vec4D color = p.ocol;
							float reflect = 0.0f;
							if (p.useEnvMap)
								reflect = 0.5f;

							// If double-sided...
							bool cull = true;
							if (p.cull == false)
								cull = false;

							LW_WriteSurface(f,surfName,color,reflect,cull,surfaceCounter,cmnt,fileLen);
						}
					}
				}
			}
		}
	}
	// ================
#ifdef _DEBUG
	wxLogMessage(_T("M2 Surface Data Written for %s"),m->fullname.c_str());
#endif

	f.SeekO(4, wxFromStart);
	u32 = MSB4<uint32>(fileLen);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	f.Close();
	wxLogMessage(_T("M2 %s Successfully written!"),m->fullname.c_str());

	ExportM2toScene(m,fn,init);
}

//---------------------------------------------
// --==WMOs==--
//---------------------------------------------

/*
	This will export Lights & Doodads (as nulls) into a Lightwave Scene file.
*/
void ExportWMOObjectstoLWO(WMO *m, const char *fn){
	// Should we generate a scene file?
	// Wll only generate if there are doodads or lights.
	bool doreturn = false;
	if (((modelExport_LW_ExportLights == false) || (m->nLights == 0)) && ((modelExport_LW_ExportDoodads == false) || (m->nDoodads == 0))){
		doreturn = true;
	}
	if (doreturn == true)
		return;

	// Open file
	wxString SceneName = wxString(fn, wxConvUTF8).BeforeLast(_T('.'));
	SceneName << _T(".lws");

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << SceneName.BeforeLast(SLASH);
		Name << SceneName.AfterLast(SLASH);

		MakeDirs(Path,_T("Scenes"));

		SceneName.Empty();
		SceneName << Path << SLASH << _T("Scenes") << SLASH << Name;
	}
	if (modelExport_PreserveDir == true){
		wxString Path1, Path2, Name;
		Path1 << SceneName.BeforeLast(SLASH);
		Name << SceneName.AfterLast(SLASH);
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);

		MakeDirs(Path1,Path2);

		SceneName.Empty();
		SceneName << Path1 << SLASH << Path2 << SLASH << Name;
	}

	ofstream fs(SceneName.mb_str(), ios_base::out | ios_base::trunc);

	if (!fs.is_open()) {
		wxMessageBox(_T("Unable to open the scene file for exporting."),_T("Scene Export Failure"));
		wxLogMessage(_T("Error: Unable to open file \"%s\". Could not export the scene."), SceneName.c_str());
		return;
	}
	SceneName = SceneName.AfterLast(SLASH);

	/*
		Lightwave Scene files are simple text files. New Lines (\n) are need to add a new variable for the scene to understand.
		Lightwave files are pretty sturdy. Variables not already in a scene or model file, will usually be generated when opened.
		As such, we don't need to declare EVERY variable for the scene file, but I'm gonna add what I think is pertinent.
	*/

	// File Top
	fs << _T("LWSC\n");
	fs << _T("5\n\n"); // I think this is a version-compatibility number...

	// Scene File Basics
	fs << _T("RenderRangeType 0\nFirstFrame 1\nLastFrame 60\n");
	fs << _T("FrameStep 1\nRenderRangeObject 0\nRenderRangeArbitrary 1-60\n");
	fs << _T("PreviewFirstFrame 0\nPreviewLastFrame 60\nPreviewFrameStep 1\nCurrentFrame 0\nFramesPerSecond 30\nChangeScene 0\n\n");

	uint32 mcount = 0; // Model Count
	uint32 lcount = 0; // Light Count
	Vec3D ZeroPos(0,0,0);
	Vec3D ZeroRot(0,0,0);
	Vec3D OneScale(1,1,1);

	uint32 DoodadLightArrayID[3000];
	uint32 DoodadLightArrayDDID[3000];
	uint32 DDLArrCount = 0;

	// Objects/Doodads go here

	// Exported Object
	int ModelID = mcount;
	wxString Obj = wxString(fn, wxConvUTF8).AfterLast(SLASH);
	wxString objFilename = wxEmptyString;
	if (modelExport_LW_PreserveDir == true){
		objFilename << _T("Objects") << SLASH;
	}
	if (modelExport_PreserveDir == true){
		objFilename += wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
		objFilename << SLASH;
		objFilename.Replace(_T("\\"),_T("/"));
	}
	objFilename += Obj;

	AnimationData ObjData;
	ObjData.Push(ZeroPos,ZeroRot,OneScale,0);

	WriteLWSceneObject(fs,objFilename,ObjData,mcount);

	if (modelExport_LW_ExportDoodads ==  true){
		// Doodads
		for (int ds=0;ds<m->nDoodadSets;ds++){			
			m->showDoodadSet(ds);
			WMODoodadSet *DDSet = &m->doodadsets[ds];
			wxString DDSetName;
			DDSetName << wxString(m->name.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) << wxString(_T(" DoodadSet ")) << wxString(DDSet->name, wxConvUTF8);
			int DDSID = mcount;

			AnimationData DDData;
			DDData.Push(ZeroPos,ZeroRot,OneScale,0);
			WriteLWSceneObject(fs,DDSetName,DDData,mcount,7,true,ModelID);

			for (int dd=DDSet->start;dd<(DDSet->start+DDSet->size);dd++){
				WMOModelInstance *doodad = &m->modelis[dd];
				wxString name = wxString(doodad->filename.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));
				// Position
				Vec3D Pos = doodad->pos;
				// Heading, Pitch & Bank.
				Vec3D Rot = QuaternionToXYZ(doodad->dir,doodad->w);

				int DDID = mcount;
				bool isNull = true;
				if (modelExport_LW_DoodadsAs > 0)
					isNull = false;
				
				if (!doodad->model){
					isNull = true;
				}

				if (isNull == false){
					wxString pathdir = wxEmptyString;
					if (modelExport_LW_PreserveDir == true){
						pathdir << _T("Objects") << SLASH;
					}
					name = pathdir << wxString(doodad->filename.c_str(), wxConvUTF8).BeforeLast(_T('.')) << _T(".lwo");
					name.Replace(_T("\\"),_T("/"));
				}

				AnimationData DoodadData;
				DoodadData.Push(Pos,Rot,Vec3D(doodad->sc,doodad->sc,doodad->sc),0);

				WriteLWSceneObject(fs,name,DoodadData,mcount,7,isNull,DDSID,wxString(doodad->filename.c_str(), wxConvUTF8));
				wxLogMessage(_T("Export: Finished writing the Doodad to the Scene File."));

				// Doodad Lights
				// Apparently, Doodad Lights must be parented to the Doodad for proper placement.
				if ((doodad->model) && (doodad->model->header.nLights > 0)){
					wxLogMessage(_T("Export: Doodad Lights found for %s, Number of lights: %i"), doodad->filename.c_str(), doodad->model->header.nLights);
					DoodadLightArrayDDID[DDLArrCount] = DDID;
					DoodadLightArrayID[DDLArrCount] = dd;
					DDLArrCount++;
				}
			}
		}
	}

	// Lighting Basics
	fs << _T("AmbientColor 1 1 1\nAmbientIntensity 0.25\nDoubleSidedAreaLights 1\n\n");

	// Lights
	if (modelExport_LW_ExportLights == true){
		// WMO Lights
		for (int i=0;i<m->nLights;i++){
			WMOLight *light = &m->lights[i];

			Vec3D color;
			color.x=light->fcolor.x;
			color.y=light->fcolor.y;
			color.z=light->fcolor.z;
			float intense = light->intensity;
			bool useAtten = false;
			float AttenEnd = light->attenEnd;

			if (light->useatten > 0){
				useAtten = true;
			}

			WriteLWSceneLight(fs,lcount,light->pos,light->type,color,intense,useAtten,AttenEnd,2.5);
		}

		// Doodad Lights
		for (unsigned int i=0;i<DDLArrCount;i++){
			
			WMOModelInstance *doodad = &m->modelis[DoodadLightArrayID[i]];
			ModelLight *light = &doodad->model->lights[i];

			if (light->type < 0){
				continue;
			}
			
			Vec3D color = light->diffColor.getValue(0,0);
			float intense = light->diffIntensity.getValue(0,0);
			bool useAtten = false;
			float AttenEnd = light->AttenEnd.getValue(0,0);
			wxString name = wxString(doodad->filename.c_str(), wxConvUTF8).AfterLast(SLASH);

			if (light->UseAttenuation.getValue(0,0) > 0){
				useAtten = true;
			}
			
			WriteLWSceneLight(fs,lcount,light->pos,light->type,color,intense,useAtten,AttenEnd,5,name,DoodadLightArrayDDID[i]);
		}
	}

	// Camera data (if we want it) goes here.
	// Yes, we can export flying cameras to Lightwave!
	// Just gotta add them back into the listing...
	fs << _T("AddCamera 30000000\nCameraName Camera\nShowCamera 1 -1 0.125490 0.878431 0.125490\nZoomFactor 2.316845\nZoomType 2\n\n");
	WriteLWScenePlugin(fs,_T("CameraHandler"),1,_T("Perspective"));	// Make the camera a Perspective camera

	// Backdrop Settings
	// Add this if viewing a skybox, or using one as a background?

	// Rendering Options
	// Raytrace Shadows enabled.
	fs << _T("RenderMode 2\nRayTraceEffects 1\nDepthBufferAA 0\nRenderLines 1\nRayRecursionLimit 16\nRayPrecision 6\nRayCutoff 0.01\nDataOverlayLabel  \nSaveRGB 0\nSaveAlpha 0\n");

	fs.close();

	// Export Doodad Files
	wxString cWMOName(m->name.c_str(),wxConvUTF8);
	if (modelExport_LW_ExportDoodads ==  true){
		if (modelExport_LW_DoodadsAs == 1){
			// Copy Model-list into an array
			std::vector<std::string> modelarray = m->models;

			// Remove the WMO
			wxDELETE(g_modelViewer->canvas->wmo);
			g_modelViewer->canvas->wmo = NULL;
			g_modelViewer->isWMO = false;
			g_modelViewer->isChar = false;
			
			// Export Individual Doodad Models
			for (unsigned int x=0;x<modelarray.size();x++){
				g_modelViewer->isModel = true;
				wxString cModelName(modelarray[x].c_str(),wxConvUTF8);

				wxLogMessage(_T("Export: Attempting to export doodad model: %s"),cModelName.c_str());
				wxString dfile = wxString(fn,wxConvUTF8).BeforeLast(SLASH) << SLASH << cModelName.AfterLast(SLASH);
				dfile = dfile.BeforeLast(_T('.')) << _T(".lwo");

				g_modelViewer->canvas->LoadModel((char *)cModelName.c_str());
				ExportM2toLWO(NULL, g_modelViewer->canvas->model, dfile.fn_str(), true);

				wxLogMessage(_T("Export: Finished exporting doodad model: %s\n\n"),cModelName.c_str());

				// Delete the loaded model
				g_modelViewer->canvas->clearAttachments();
				g_modelViewer->canvas->model = NULL;
				g_modelViewer->isModel = false;
			}
		}
		//texturemanager.clear();

		// Reload our original WMO file.
		//wxLogMessage("Reloading original WMO file: %s",cWMOName);

		// Load the WMO


	}
}


void ExportWMOtoLWO(WMO *m, const char *fn){
	wxString file = wxString(fn, wxConvUTF8);

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << file.BeforeLast(SLASH);
		Name << file.AfterLast(SLASH);

		MakeDirs(Path,_T("Objects"));

		file.Empty();
		file << Path << SLASH<<_T("Objects")<<SLASH << Name;
	}
	if (modelExport_PreserveDir == true){
		wxString Path1, Path2, Name;
		Path1 << file.BeforeLast(SLASH);
		Name << file.AfterLast(SLASH);
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);

		MakeDirs(Path1,Path2);

		file.Empty();
		file << Path1 << SLASH << Path2 << SLASH << Name;
	}
#ifndef _WINDOWS
	file.Replace(wxT("\\"),wxT("/"));
#endif
	wxFFileOutputStream f(file, _T("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), file.c_str());
		wxMessageDialog(g_modelViewer,_T("Could not open file for exporting."),_T("Exporting Error..."));
		return;
	}
	LogExportData(_T("LWO"),wxString(fn, wxConvUTF8).BeforeLast(SLASH),_T("WMO"));

	int off_t;
	uint16 dimension;

	// LightWave object files use the IFF syntax described in the EA-IFF85 document. Data is stored in a collection of chunks. 
	// Each chunk begins with a 4-byte chunk ID and the size of the chunk in bytes, and this is followed by the chunk contents.


	/* LWO Model Format, as layed out in offical LWO2 files.( I Hex Edited to find most of this information from files I made/saved in Lightwave. -Kjasi)
	FORM	// Format Declaration
	LWO2	// Declares this is the Lightwave Object 2 file format. LWOB is the first format. Doesn't have a lot of the cool stuff LWO2 has...

	TAGS	// Used for various Strings
		Sketch Color Names
		Part Names
		Surface Names
	LAYR		// Specifies the start of a new layer. Probably best to only be on one...
		PNTS		// Points listing & Block Section. If more than sizeof(uint32) points are in a single layer, a second PNTS block (and thus a second POLS block) is added after the first POLS block.
			BBOX		// Bounding Box. It's optional, but will probably help.
			VMPA		// Vertex Map Parameters, Always Preceeds a VMAP & VMAD. 4bytes: Size (2 * 4 bytes).
						// UV Sub Type: 0-Linear, 1-Subpatched, 2-Linear Corners, 3-Linear Edges, 4-Across Discontinuous Edges.
						// Sketch Color: 0-12; 6-Default Gray
				VMAP		// Vector Map Section. Always Preceeds the following:
					SPOT	// Aboslute Morph Maps. Used only while modeling. Ignore.
					TXUV	// Defines UV Vector Map. Best not to use these unless the data has no Discontinuous UVs.
					PICK	// Point Selection Sets (2 bytes, then Set Name, then data. (Don't know what kind)
					MORF	// Relative Morph Maps. These are used for non-boned mesh animation.
					RGB		// Point Color Map, no Alpha. Note the space at end of the group!
					RGBA	// Same as above, but with an alpha channel.
					WGHT	// Weight Map. Used to give bones limited areas to effect, or can be used for point-by-point maps for various surfacing tricks.

		POLS		// Declares Polygon section. Next 4 bytes = Number of Polys
			FACE		// The actual Polygons. The maximum number of vertices per poly is 1023!
						// The following SubChunks are available to all POLS chunks. (PTCH, SUBD, MBAL & BONE)
				PTAG		// The Poly Tags for this Poly. These usually reference items in the TAGS group.
					COLR	// The Sketch Color Name
					PART	// The Part Name
					SURF	// The Surface Name
				VMPA		// Discontinuous Vertex Map Parameters (See the one in the Points section for details)
					VMAD		// Discontinuous Vector Map Section. Best if used only for UV Maps. Difference between VMAP & VMAD: VMAPs are connected to points, while VMADs are connected to Polys.
						APSL	// Adaptive Pixel Subdivision Level. Only needed for sub-patched models, so just ignore it for our outputs.
						TXUV	// Defines UV Vector Map
			PTCH	// Cat-mull Clarke Patches. Don't need this.
			SUBD	// Subdivision Patches. Same as above, but might help the water look good...
			MBAL	// Metaballs. Don't bother...
			BONE	// Line segments representing the object's skeleton. These are converted to bones for deformation during the rigging process.

	CLIP (for each Image)
		STIL	// 2 bytes, size of string, followed by image_name.extention
		FLAG	// Flags. 2 bytes, size of chunk. Not sure what the flag values are.
		AMOD	// 2 bytes: What's changed, 2 bytes: value. 2-Alphas: 0-Enabled, 1-Disabled, 2-Alpha Only. AMOD is omitted if value is 0.
	XREF	// Calls an instance of a CLIP. We'll avoid this for now.

	SURF	// Starts the surface's data. Not sure about the 4 bytes after it...
		// Until BLOK, this just sets the default values
		COLR	// Color
		LUMI	// Luminosity
		DIFF	// Diffusion
		SPEC	// Specularity
		REFL	// Reflections
		TRAN	// Transparancy
		TRNL	// Translucency
		RIND	// Refractive Index
		BUMP	// Bump Amount
		GLOS	// Glossiness
		GVAL	// Glow
		SHRP	// Diffuse Sharpness

		SMAN	// Smoothing Amount

		RFOP	// Reflection Options: 0-Backdrop Only (default), 1-Raytracing + Backdrop, 2 - Spherical Map, 3 - Raytracing + Spherical Map
		TROP	// Same as RFOP, but for Refraction.
		SIDE	// Declares if the surface is Double-Sided.
		NVSK	// Exclude from VStack

		CMNT // Surface Comment. 2bytes: Size. Simple Text line for this surface. Make sure it doesn't end on an odd byte!
		VERS // Version Compatibility mode, including what it's compatible with. 2 bytes (int16, value 4), 4 bytes (int32, value is 850 for LW8.5 Compatability, 931 for LW9.3.1, and 950 for Default as of LW9.6)

		BLOK	// First Blok. Bloks hold Surface texture information!
			IMAP	// Declares that this surface texture is an image map. (There are other types, but this is the only one we'll need.)
				CHAN COLR	// Declares that the image map will be applied to the color channel. (Color has a Texture!)
					OPAC	// Opacity of Layer
					ENAB	// Is the layer enabled?
					NEGA	// Is it inverted?
					TMAP	// Texture Map details
						CNTR	// Position
						SIZE	// Scale
						ROTA	// Rotation
						FALL	// Falloff
						OREF	// Object Reference
						CSYS	// Coordinate System: 0-Object's Coordinates, 1-World's Coordinates

						// Image Maps
						PROJ	// Image Projection Mode: 0-Planar (references AXIS), 1-Cylindrical, 2-Spherical, 3-Cubic, 4-Front Projection, 5-UV (IDed in VMAP chunk)
						AXIS	// The axis the image uses: 0-X, 1-Y, or 2-Z;
						IMAG	// The image to use: Use CLIP Index
						WRAP	// Wrapping Mode: 0-Reset, 1-Repeat, 2-Mirror, 3-Edge
						WRPW	// Wrap Count Width (Used for Cylindrical & Spherical projections)
						WRPH	// Wrap Count Height
						VMAP	// Name of the UV Map to use, should PROJ be set to 5!
						AAST	// Antialiasing Strength
						PIXB	// Pixel Blending

		// Node Information
		// We can probably skip this for now. Later, it would be cool to mess with it, but for now, it'll be automatically generated once the file is opened in LW.

		NODS	// Node Block & Size
			NROT
			NLOC
			NZOM
			NSTA	// Activate Nodes
			NVER
			NNDS
			NSRV
				Surface
			NTAG
			NRNM
				Surface
			NNME
				Surface
			NCRD
			NMOD
			NDTA
			NPRW
			NCOM
			NCON

	*/



	wxLogMessage(_T("Starting Lightwave WMO Model Export Function..."));
	unsigned int fileLen = 0;


	// ===================================================
	// FORM		// Format Declaration
	//
	// Always exempt from the length of the file!
	// ===================================================
	f.Write("FORM", 4);
	f.Write(reinterpret_cast<char *>(&fileLen), 4);


	// ===================================================
	// LWO2
	//
	// Declares this is the Lightwave Object 2 file format.
	// LWOB is the first format. It doesn't have a lot of the cool stuff LWO2 has...
	// ===================================================
	f.Write("LWO2", 4);
	fileLen += 4;


	// ===================================================
	// TAGS
	//
	// Used for various Strings. Known string types, in order:
	//		Sketch Color Names
	//		Part Names
	//		Surface Names
	// ===================================================
	f.Write("TAGS", 4);
	uint32 tagsSize = 0;
	u32 = 0;
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;

	uint32 counter=0;
	uint32 TagCounter=0;
	uint16 PartCounter=0;
	uint16 SurfCounter=0;
	unsigned int numVerts = 0;
	unsigned int numGroups = 0;

	// Build Surface Name Database
	std::vector<std::string> surfarray;
	std::vector<std::string> sfix;
	std::vector<uint16> sfix2;
	std::vector<uint16> check;
	for (uint16 t=0;t<m->nTextures;t++){
		wxString tex = wxString(m->textures[t].c_str(), wxConvUTF8).BeforeLast(_T('.'));
		surfarray.push_back((char *)tex.c_str());
		//sfix.push_back(tex.c_str());
		sfix2.push_back(t);
		check.push_back(0);
	}
	// Rename duplicate names
	for (uint16 t=0;t<surfarray.size();t++){
		uint16 mod = 0;
		for (uint16 k=0;k<surfarray.size();k++){
			if ((t!=k)&&(surfarray[sfix2[t]] == surfarray[sfix2[k]])&&(check[k]==0)){
				//sfix[t] = wxString(surfarray[t] + wxString::Format(_T("_v%02i"),mod));
				//sfix[k] = wxString(surfarray[k] + wxString::Format(_T("_v%02i"),mod+1));
				sfix2[k] = sfix2[t];
				//sfix2[t] = t-1;
				mod++;
				check[k] = 1;
				
				// This code can erase the extra surface names.
				// Not used because we don't have a (sucessful) way to convert the deleted name's IDs to the other ID.
				/*for (int g=t;g>0;g--){
					if ((sfix2[g] - 1)>-1){
						sfix2[g] -= 1;
					}
				}*/
				//surfarray.erase(surfarray.begin() + k);//, surfarray.begin() + k+1);
				//wxLogMessage("Deleting duplicate.");
				//k++;
			}
		}
	}


#ifdef _DEBUG
	wxLogMessage(_T("Texture List"));
	for (uint16 x=0;x<m->nTextures;x++){
		wxLogMessage(_T("[ID:%02i] = %s"),x,m->textures[x].data());
	}

	wxLogMessage(_T("Surface List"));
	for (uint16 x=0;x<surfarray.size();x++){
		wxLogMessage(_T("[ID:%02i] = %s"),x,surfarray[x].c_str());
	}
#endif

	// --== Part Names ==--
	for (uint16 g=0;g<m->nGroups;g++) {
		wxString partName = wxString(m->groups[g].name.c_str(), wxConvUTF8);

		partName.Append(_T('\0'));
		if (fmod((float)partName.length(), 2.0f) > 0)
			partName.Append(_T('\0'));
		f.Write(partName.data(), partName.length());
		tagsSize += (uint32)partName.length();
	}

	// --== Surface Names ==--	
	// Write the Name Database to the File
	for (uint16 t=0;t<surfarray.size();t++){
		wxString matName = wxString(surfarray[t].c_str(), wxConvUTF8).AfterLast(SLASH);
		wxLogMessage(_T("MatNames T=%i, Writing MatName: %s"),t,matName.c_str());

		if (matName.Len() == 0)
			matName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast('.') + wxString::Format(_T("_Material_%03i"), t);

		matName.Append(_T('\0'));
		if (fmod((float)matName.length(), 2.0f) > 0)
			matName.Append(_T('\0'));
		f.Write(matName.data(), matName.length());
		tagsSize += (uint32)matName.length();
		//surfarray[t] = wxString(m->textures[t].c_str()).BeforeLast('.');
	}

	off_t = -4-tagsSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(tagsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	fileLen += tagsSize;



	// ===================================================
	// LAYR
	//
	// Specifies the start of a new layer. Each layer has it's own Point & Poly
	// chunk, which tells it what data is on what layer. It's probably best
	// to only have 1 layer for now.
	// ===================================================
	f.Write("LAYR", 4);
	u32 = MSB4<uint32>(18);
	fileLen += 8;
	f.Write(reinterpret_cast<char *>(&u32), 4);
	ub = 0;
	for(int i=0; i<18; i++) {
		f.Write(reinterpret_cast<char *>(&ub), 1);
	}
	fileLen += 18;
	// ================


	// ===================================================
	// POINTS CHUNK, this is the vertice data
	// The PNTS chunk contains triples of floating-point numbers, the coordinates of a list of points. The numbers are written 
	// as IEEE 32-bit floats in network byte order. The IEEE float format is the standard bit pattern used by almost all CPUs 
	// and corresponds to the internal representation of the C language float type. In other words, this isn't some bizarre 
	// proprietary encoding. You can process these using simple fread and fwrite calls (but don't forget to correct the byte 
	// order if necessary).
	// ===================================================
	uint32 pointsSize = 0;
	f.Write("PNTS", 4);
	u32 = MSB4<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;

	unsigned int bindice = 0;
	unsigned int gverts = 0;

	// output all the vertice data
	for (int g=0; g<m->nGroups; g++) {
		gverts += m->groups[g].nVertices;
		for (int b=0; b<m->groups[g].nBatches; b++)	{
			WMOBatch *batch = &m->groups[g].batches[b];

			bindice += batch->indexCount;
			for(int ii=0;ii<batch->indexCount;ii++){
				int a = m->groups[g].indices[batch->indexStart + ii];
				Vec3D vert;
				vert.x = MSB4<float>(m->groups[g].vertices[a].x);
				vert.y = MSB4<float>(m->groups[g].vertices[a].z);
				vert.z = MSB4<float>(m->groups[g].vertices[a].y);
				f.Write(reinterpret_cast<char *>(&vert.x), 4);
				f.Write(reinterpret_cast<char *>(&vert.y), 4);
				f.Write(reinterpret_cast<char *>(&vert.z), 4);
				pointsSize += 12;

				numVerts++;
			}
			numGroups++;
		}
	}
#ifdef _DEBUG
	wxLogMessage(_T("WMO Point Count: %i, Stored Indices: %i, Stored Verticies: %i"),numVerts, bindice, gverts);
#endif


	fileLen += pointsSize;
	off_t = -4-pointsSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	// ================


	// --
	// The bounding box for the layer, just so that readers don't have to scan the PNTS chunk to find the extents.
	f.Write("BBOX", 4);
	u32 = MSB4<uint32>(24);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	Vec3D vert;
	vert.x = MSB4<float>(m->v1.x);
	vert.z = MSB4<float>(m->v1.y);
	vert.y = MSB4<float>(m->v1.z);
	f.Write(reinterpret_cast<char *>(&vert.x), 4);
	f.Write(reinterpret_cast<char *>(&vert.y), 4);
	f.Write(reinterpret_cast<char *>(&vert.z), 4);
	vert.x = MSB4<float>(m->v2.x);
	vert.z = MSB4<float>(m->v2.y);
	vert.y = MSB4<float>(m->v2.z);
	f.Write(reinterpret_cast<char *>(&vert.x), 4);
	f.Write(reinterpret_cast<char *>(&vert.y), 4);
	f.Write(reinterpret_cast<char *>(&vert.z), 4);
	fileLen += 32;

	// --== Vertex Mapping ==--
	bool hasVertColors = false;

	// ===================================================
	//VMPA		// Vertex Map Parameters, Always Preceeds a VMAP & VMAD. 4bytes: Size, then Num Vars (2) * 4 bytes.
				// UV Sub Type: 0-Linear, 1-Subpatched, 2-Linear Corners, 3-Linear Edges, 4-Across Discontinuous Edges.
				// Sketch Color: 0-12; 6-Default Gray
	// ===================================================
	f.Write("VMPA", 4);
	u32 = MSB4<uint32>(8);	// We got 2 Paramaters, * 4 Bytes.
	f.Write(reinterpret_cast<char *>(&u32), 4);
	u32 = 4;				// Across Discontinuous Edges UV Sub Type
	f.Write(reinterpret_cast<char *>(&u32), 4);
	u32 = MSB4<uint32>(6);	// Default Gray
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 16;

	// UV Data
	uint32 vmapSize = 0;
	counter = 0;

	f.Write("VMAP", 4);
	u32 = MSB4<uint32>(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write("TXUV", 4);
	dimension = MSB2(2);
	f.Write(reinterpret_cast<char *>(&dimension), 2);
	f.Write(_T("Texture"), 7);
	ub = 0;
	f.Write(reinterpret_cast<char *>(&ub), 1);
	vmapSize += 14;

	for (int32 g=0; g<m->nGroups; g++) {
		WMOGroup *group = &m->groups[g];

		if (group->hascv){
			hasVertColors = true;
		}
		for (int32 b=0; b<group->nBatches; b++) {
			WMOBatch *batch = &group->batches[b];
			for(uint32 ii=0;ii<batch->indexCount;ii++) {
				int a = group->indices[batch->indexStart + ii];

				LW_WriteVX(f,counter,vmapSize);

				f32 = MSB4<float>(m->groups[g].texcoords[a].x);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				f32 = MSB4<float>(1 - m->groups[g].texcoords[a].y);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				vmapSize += 8;
				counter++;
			}
		}
	}
	fileLen += vmapSize;
	off_t = -4-vmapSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

/*
	// ===================================================
	// Vertex Colors
	//
	// Doesn't quite work yet, but we're close! Getting an error in the first 0xFF00 points...
	// WMV's LW output doesn't output Verts, but the indices. The Vert Color data is for verts. (Obviously...)
	// ===================================================
	wxLogMessage(_T("Building VertColor Data..."));
	if (hasVertColors == true){
		counter = 0;
		vmapSize = 0;

		// Vertex Color Data
		f.Write("VMAP", 4);
		u32 = MSB4<uint32>(vmapSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		fileLen += 8;
		f.Write("RGBA", 4);
		dimension = MSB2(4);
		f.Write(reinterpret_cast<char *>(&dimension), 2);
		f.Write("Colors", 6);
		vmapSize += 12;

		for (uint32 g=0; g<m->nGroups; g++) {
			WMOGroup *group = &m->groups[g];
			for (uint32 b=0; b<group->nBatches; b++) {
				WMOBatch *batch = &group->batches[b];
				for(uint32 ii=0;ii<batch->indexCount;ii++) {
					uint32 a = batch->indexStart + ii;

					LW_WriteVX(f,counter,vmapSize);

					uint8 r, g, b, alpha;
					float rf,gf,bf,af;
					r = 255;
					g = 255;
					b = 255;
					alpha = 255;
						
					// The if hascv is here because if one of the groups DO have Vertex Colors,
					// we should set the colors for all the vertexs.
					if (group->hascv) {
						b = group->VertexColors[group->IndiceToVerts[a]].b;
						g = group->VertexColors[group->IndiceToVerts[a]].g;
						r = group->VertexColors[group->IndiceToVerts[a]].r;
						alpha = group->VertexColors[group->IndiceToVerts[a]].a;
					}

					rf = (float)(r/255.0f);
					gf = (float)(g/255.0f);
					bf = (float)(b/255.0f);
					af = (float)((255.0f-alpha)/255.0f);

					f32 = MSB4<float>(rf);
					f.Write(reinterpret_cast<char *>(&f32), 4);
					f32 = MSB4<float>(gf);
					f.Write(reinterpret_cast<char *>(&f32), 4);
					f32 = MSB4<float>(bf);
					f.Write(reinterpret_cast<char *>(&f32), 4);
					f32 = MSB4<float>(af);
					f.Write(reinterpret_cast<char *>(&f32), 4);
					vmapSize += 16;
					counter++;
				}
			}
		}
		fileLen += vmapSize;

		off_t = -4-vmapSize;
		f.SeekO(off_t, wxFromCurrent);
		u32 = MSB4<uint32>(vmapSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		f.SeekO(0, wxFromEnd);
	}
*/

	// --
	// POLYGON CHUNK
	// The POLS chunk contains a list of polygons. A "polygon" in this context is anything that can be described using an 
	// ordered list of vertices. A POLS of type FACE contains ordinary polygons, but the POLS type can also be CURV, 
	// PTCH, MBAL or BONE, for example.
	//
	// The high 6 bits of the vertex count for each polygon are reserved for flags, which in effect limits the number of 
	// vertices per polygon to 1023. Don't forget to mask the high bits when reading the vertex count. The flags are 
	// currently only defined for CURVs.
	// 
	// The point indexes following the vertex count refer to the points defined in the most recent PNTS chunk. Each index 
	// can be a 2-byte or a 4-byte integer. If the high order (first) byte of the index is not 0xFF, the index is 2 bytes long. 
	// This allows values up to 65279 to be stored in 2 bytes. If the high order byte is 0xFF, the index is 4 bytes long and 
	// its value is in the low three bytes (index & 0x00FFFFFF). The maximum value for 4-byte indexes is 16,777,215 (224 - 1). 
	// Objects with more than 224 vertices can be stored using multiple pairs of PNTS and POLS chunks.
	// 
	// The cube has 6 square faces each defined by 4 vertices. LightWave polygons are single-sided by default 
	// (double-sidedness is a possible surface property). The vertices are listed in clockwise order as viewed from the 
	// visible side, starting with a convex vertex. (The normal is defined as the cross product of the first and last edges.)

	f.Write("POLS", 4);
	uint32 polySize = 4;
	u32 = MSB4<uint32>(polySize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8; // FACE is handled in the PolySize
	f.Write("FACE", 4);

	counter = 0;
	unsigned long polycount = 0;
	uint32 gpolys = 0;
	unsigned int tnBatches = 0;
	
	wxLogMessage(_T("-= Outputting Polys =-\nGroupCount: %i"),m->nGroups);

	for (int g=0;g<m->nGroups;g++) {
		gpolys += m->groups[g].nTriangles;
		for (int b=0; b<m->groups[g].nBatches; b++){
			WMOBatch *batch = &m->groups[g].batches[b];
			for (unsigned int k=0; k<batch->indexCount; k+=3) {
				uint16 nverts;

				// Write the number of Verts
				nverts = MSB2(3);
				f.Write(reinterpret_cast<char *>(&nverts),2);
				polySize += 2;

				for (int x=0;x<3;x++,counter++){
					//wxLogMessage("Batch %i, index %i, x=%i",b,k,x);

					// mod is needed, cause otherwise the polys will be generated facing the wrong way. 
					// Proper order: 0, 2, 1
					int mod = 0;
					if (x==1){
						mod = 1;
					}else if (x==2){
						mod = -1;
					}

					uint32 a = counter + mod;

					LW_WriteVX(f,a,polySize);
				}
				polycount++;
			}
			tnBatches++;
		}
	}

	off_t = -4-polySize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(polySize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

#ifdef _DEBUG
	wxLogMessage(_T("WMO Poly Count: %i, Stored Polys: %i, Polysize: %i"),polycount, gpolys, polySize);
	wxLogMessage(_T("WMO nGroups: %i, nBatchs: %i"),m->nGroups, tnBatches);
#endif

	fileLen += polySize;
	// ========


	// --
	// The PTAG chunk associates tags with polygons. In this case, it identifies which surface is assigned to each polygon. 
	// The first number in each pair is a 0-based index into the most recent POLS chunk, and the second is a 0-based 
	// index into the TAGS chunk.

	// NOTE: Every PTAG type needs a seperate PTAG call!

	TagCounter = 0;
	PartCounter = 0;
	counter = 0;
	uint32 ptagSize;

	// Parts PolyTag
	f.Write("PTAG", 4);
	ptagSize = 4;
	counter=0;
	u32 = MSB4<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write("PART", 4);
	for (int g=0;g<m->nGroups;g++) {
		for (int b=0; b<m->groups[g].nBatches; b++) {
			WMOBatch *batch = &m->groups[g].batches[b];

			for (unsigned int k=0; k<batch->indexCount; k+=3) {
				LW_WriteVX(f,counter,ptagSize);

				u16 = MSB2(TagCounter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				ptagSize += 2;
				counter++;
			}
		}
		TagCounter++;
		PartCounter++;
	}
	fileLen += ptagSize;

	off_t = -4-ptagSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	// Surface PolyTag
	counter = 0;
	f.Write("PTAG", 4);
	ptagSize = 4;
	u32 = MSB4<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write("SURF", 4);
	for (int g=0;g<m->nGroups;g++) {
		for (int b=0; b<m->groups[g].nBatches; b++) {
			WMOBatch *batch = &m->groups[g].batches[b];

			for (uint32 k=0; k<batch->indexCount; k+=3) {
				LW_WriteVX(f,counter,ptagSize);

				int texid = m->mat[batch->texture].tex-1;
				int surfID = PartCounter + texid;
				if (check[texid] == 1){
					surfID = PartCounter + sfix2[texid];
				}

				u16 = MSB2(surfID);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				ptagSize += 2;
				counter++;
			}
			SurfCounter++;
		}
	}
	fileLen += ptagSize;

	off_t = -4-ptagSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	// ===================================================
	//VMPA		// Vertex Map Parameters, Always Preceeds a VMAP & VMAD. 4bytes: Size, then Num Vars (2) * 4 bytes.
				// UV Sub Type: 0-Linear, 1-Subpatched, 2-Linear Corners, 3-Linear Edges, 4-Across Discontinuous Edges.
				// Sketch Color: 0-12; 6-Default Gray
	// ===================================================
	f.Write("VMPA", 4);
	u32 = MSB4<uint32>(8);	// We got 2 Paramaters, * 4 Bytes.
	f.Write(reinterpret_cast<char *>(&u32), 4);
	u32 = 4;				// Across Discontinuous Edges UV Sub Type
	f.Write(reinterpret_cast<char *>(&u32), 4);
	u32 = MSB4<uint32>(6);	// Default Gray
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 16;


	// ===================================================
	// Discontinuous Vertex Mapping
	// ===================================================

	uint32 vmadSize = 0;
	f.Write("VMAD", 4);
	u32 = MSB4<uint32>(vmadSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	// UV Data
	f.Write("TXUV", 4);
	dimension = 2;
	dimension = MSB2(dimension);
	f.Write(reinterpret_cast<char *>(&dimension), 2);
	f.Write(_T("Texture"), 7);
	ub = 0;
	f.Write(reinterpret_cast<char *>(&ub), 1);
	vmadSize += 14;

	counter = 0;
	for (int g=0;g<m->nGroups;g++) {
		WMOGroup *group = &m->groups[g];
		for (int b=0; b<group->nBatches; b++){
			WMOBatch *batch = &group->batches[b];

			for (uint32 k=0, b=0; k<batch->indexCount; k++,b++) {
				int a = m->groups[g].indices[batch->indexStart + k];

				LW_WriteVX(f,counter,vmadSize);
				LW_WriteVX(f,(counter/3),vmadSize);

				f32 = MSB4<float>(group->texcoords[a].x);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				f32 = MSB4<float>(1 - group->texcoords[a].y);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				vmadSize += 8;
				counter++;
			}
		}
	}
	fileLen += vmadSize;
	off_t = -4-vmadSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(vmadSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	

	// ===================================================
	// Texture File List
	// ===================================================

	// Export Texture Files
	// There is currently a bug where the improper textures are being assigned the wrong filenames.
	// Once the proper textures can be assigned the proper filenames, then this will be re-installed.
	
	for (uint32 x=0;x<m->textures.size();x++){
		SaveTexture2(wxString(m->textures[x].c_str(), wxConvUTF8), wxString(fn, wxConvUTF8).BeforeLast(SLASH),wxString(_T("LWO")),wxString(_T("tga")));
	}

	// Texture Data
	for (uint32 t=0;t<surfarray.size();t++){
		int clipSize = 0;
		f.Write("CLIP", 4);
		u32 = MSB4<uint32>(clipSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		fileLen += 8;

		u32 = MSB4<uint32>(PartCounter + t);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		f.Write("STIL", 4);
		clipSize += 8;

		wxString sTexName = wxEmptyString;
		if (modelExport_LW_PreserveDir == true){
			sTexName += _T("Images/");
		}
		sTexName += wxString(surfarray[t].c_str(), wxConvUTF8) + wxString(_T(".tga"));
		sTexName.Replace(_T("\\"),_T("/"));
		sTexName += _T('\0');
#ifndef _WINDOWS
		sTexName.Replace(_T("\\"),_T("/"));
#endif
		if (fmod((float)sTexName.length(), 2.0f) > 0)
				sTexName.Append(_T('\0'));

		u16 = MSB2((unsigned short)sTexName.length());
		f.Write(reinterpret_cast<char *>(&u16), 2);
		f.Write(sTexName.data(), sTexName.length());
		clipSize += (2+(int)sTexName.length());

		// update the chunks length
		off_t = -4-clipSize;
		f.SeekO(off_t, wxFromCurrent);
		u32 = MSB4<uint32>(clipSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		f.SeekO(0, wxFromEnd);

		fileLen += clipSize;
	}


	// ===================================================
	// Surface Data
	// ===================================================

	wxString surfName;
	for (uint32 surf=0;surf<surfarray.size();surf++){
		WMOMaterial *mat = &m->mat[surf];

		// Surface name
		surfName = wxString(surfarray[surf].c_str(), wxConvUTF8).AfterLast(SLASH);
		Vec4D Color(Vec4D(0.5f,0.5f,0.5f,1.0f));
		bool cull = true;
		if (mat->flags & WMO_MATERIAL_CULL)
			cull = true;
		wxString cmnt = wxString(surfarray[surf].c_str(), wxConvUTF8);
		float reflect = 0.0f;

		LW_WriteSurface(f,surfName, Color, reflect, cull, PartCounter + surf, cmnt, fileLen);
	}
	// ================


	f.SeekO(4, wxFromStart);
	u32 = MSB4<uint32>(fileLen);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	f.Close();

	// Cleanup, Isle 3!
	surfarray.erase(surfarray.begin(),surfarray.end());
	surfarray.~vector();
	sfix.erase(sfix.begin(),sfix.end());
	sfix.~vector();
	sfix2.erase(sfix2.begin(),sfix2.end());
	sfix2.~vector();
	check.erase(check.begin(),check.end());
	check.~vector();	

	// Export Lights & Doodads
	if ((modelExport_LW_ExportDoodads == true)||(modelExport_LW_ExportLights == true)){
		ExportWMOObjectstoLWO(m,fn);
	}
}



// -----------------------------------------
// New Lightwave Stuff
//
// Under construction, only visible/usable while in Debug Mode.
// -----------------------------------------


// Seperated out the Writing function, so we don't have to write it all out every time we want to export something.
// Should probably do something similar with the other exporting functions as well...
bool WriteLWObject(wxString filename, wxString Tags[], int LayerNum, std::vector<LWLayer> Layers){

	// LightWave object files use the IFF syntax described in the EA-IFF85 document. Data is stored in a collection of chunks. 
	// Each chunk begins with a 4-byte chunk ID and the size of the chunk in bytes, and this is followed by the chunk contents.


	/* LWO Model Format, as layed out in offical LWO2 files.( I Hex Edited to find most of this information from files I made/saved in Lightwave. -Kjasi)
	FORM	// Format Declaration
	LWO2	// Declares this is the Lightwave Object 2 file format. LWOB is the first format. Doesn't have a lot of the cool stuff LWO2 has...

	TAGS	// Used for various Strings
		Sketch Color Names
		Part Names
		Surface Names
	LAYR		// Specifies the start of a new layer. Probably best to only be on one...
		PNTS		// Points listing & Block Section
			BBOX		// Bounding Box. It's optional, but will probably help.
			VMPA		// Vertex Map Parameters, Always Preceeds a VMAP & VMAD. 4bytes: Size (2 * 4 bytes).
						// UV Sub Type: 0-Linear, 1-Subpatched, 2-Linear Corners, 3-Linear Edges, 4-Across Discontinuous Edges.
						// Sketch Color: 0-12; 6-Default Gray
				VMAP		// Vector Map Section. Always Preceeds the following:
					SPOT	// Aboslute Morph Maps. Used only while modeling. Ignore.
					TXUV	// Defines UV Vector Map. Best not to use these unless the data has no Discontinuous UVs.
					PICK	// Point Selection Sets (2 bytes, then Set Name, then data. (Don't know what kind)
					MORF	// Relative Morph Maps. These are used for non-boned mesh animation.
					RGB		// Point Color Map, no Alpha. Note the space at end of the group!
					RGBA	// Same as above, but with an alpha channel.
					WGHT	// Weight Map. Used to give bones limited areas to effect, or can be used for point-by-point maps for various surfacing tricks.

		POLS		// Declares Polygon section. Next 4 bytes = Number of Polys
			FACE		// The actual Polygons. The maximum number of vertices is 1023 per poly!
				PTAG		// The Poly Tags for this Poly. These usually reference items in the TAGS group.
					COLR	// The Sketch Color Name
					PART	// The Part Name
					SURF	// The Surface Name
				VMPA		// Discontinuous Vertex Map Parameters (See the one in the Points section for details)
					VMAD		// Discontinuous Vector Map Section. Best if used only for UV Maps. Difference between VMAP & VMAD: VMAPs are connected to points, while VMADs are connected to Polys.
						APSL	// Adaptive Pixel Subdivision Level. Only needed for sub-patched models, so just ignore it for our outputs.
						TXUV	// Defines UV Vector Map
			PTCH	// Cat-mull Clarke Patches. Don't need this, but it mirror's FACE's sub-chunks.
			SUBD	// Subdivision Patches. Same as above.
			MBAL	// Metaballs. Don't bother...
			BONE	// Line segments representing the object's skeleton. These are converted to bones for deformation during rendering.

	CLIP (for each Image)
		STIL	// 2 bytes, size of string, followed by image name.extention
		FLAG	// Flags. 2 bytes, size of chunk. Not sure what the flag values are.
		AMOD	// 2 bytes: What's changed, 2 bytes: value. 2-Alphas: 0-Enabled, 1-Disabled, 2-Alpha Only. AMOD is omitted if value is 0.
	XREF	// Calls an instance of a CLIP. We'll avoid this for now.

	SURF	// Starts the surface's data. Not sure about the 4 bytes after it...
		// Until BLOK, this just sets the default values
		COLR	// Color
		LUMI	// Luminosity
		DIFF	// Diffusion
		SPEC	// Specularity
		REFL	// Reflections
		TRAN	// Transparancy
		TRNL	// Translucency
		RIND	// Refractive Index
		BUMP	// Bump Amount
		GLOS	// Glossiness
		GVAL	// Glow
		SHRP	// Diffuse Sharpness

		SMAN	// Smoothing Amount

		RFOP	// Reflection Options: 0-Backdrop Only (default), 1-Raytracing + Backdrop, 2 - Spherical Map, 3 - Raytracing + Spherical Map
		TROP	// Same as RFOP, but for Refraction.
		SIDE	// Is it Double-Sided?
		NVSK	// Exclude from VStack

		CMNT // Surface Comment, but I don't seem to be able to get it to show up in LW... 2bytes: Size. Simple Text line for this surface. Make sure it doesn't end on an odd byte! VERS must be 931 or 950!
		VERS // Version Compatibility mode, including what it's compatible with. 2 bytes (int16, value 4), 4 bytes (int32, value is 850 for LW8.5 Compatability, 931 for LW9.3.1, and 950 for Default)

		BLOK	// First Blok. Bloks hold Surface texture information!
			IMAP	// Declares that this surface texture is an image map.
				CHAN COLR	// Declares that the image map will be applied to the color channel. (Color has a Texture!)
					OPAC	// Opacity of Layer
					ENAB	// Is the layer enabled?
					NEGA	// Is it inverted?
					TMAP	// Texture Map details
						CNTR	// Position
						SIZE	// Scale
						ROTA	// Rotation
						FALL	// Falloff
						OREF	// Object Reference
						CSYS	// Coordinate System: 0-Object's Coordinates, 1-World's Coordinates

						// Image Maps
						PROJ	// Image Projection Mode: 0-Planar (references AXIS), 1-Cylindrical, 2-Spherical, 3-Cubic, 4-Front Projection, 5-UV (IDed in VMAP chunk)
						AXIS	// The axis the image uses: 0-X, 1-Y, or 2-Z;
						IMAG	// The image to use: Use CLIP Index
						WRAP	// Wrapping Mode: 0-Reset, 1-Repeat, 2-Mirror, 3-Edge
						WRPW	// Wrap Count Width (Used for Cylindrical & Spherical projections)
						WRPH	// Wrap Count Height
						VMAP	// Name of the UV Map to use, should PROJ be set to 5!
						AAST	// Antialiasing Strength
						PIXB	// Pixel Blending

		// Node Information
		// We can probably skip this for now. Later, it would be cool to mess with it, but for now, it'll be automatically generated once the file is opened in LW.

		NODS	// Node Block & Size
			NROT
			NLOC
			NZOM
			NSTA	// Activate Nodes
			NVER
			NNDS
			NSRV
				Surface
			NTAG
			NRNM
				Surface
			NNME
				Surface
			NCRD
			NMOD
			NDTA
			NPRW
			NCOM
			NCON

	*/

	// Check to see if we have any data to generate this file.
	if (Layers.size() < 1){
		wxMessageBox(_T("No Layer Data found.\nUnable to write object file."),_T("Error"));
		wxLogMessage(_T("Error: No Layer Data. Unable to write object file."));
		return false;
	}
/*

   	// -----------------------------------------
	// Initial Variables
	// -----------------------------------------

	// File Length
	unsigned int fileLen = 0;

	// Other Declares
	int off_t;
	uint16 dimension;

	// Needed Numbers
	int TagCount = Tags->size();

	// Open Model File
	wxString file = wxString(filename, wxConvUTF8);

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << file.BeforeLast(SLASH);
		Name << file.AfterLast(SLASH);

		MakeDirs(Path,"Objects");

		file.Empty();
		file << Path << SLASH<<_T("Objects")<<SLASH << Name;
	}
	if (modelExport_PreserveDir == true){
		wxString Path1, Path2, Name;
		Path1 << file.BeforeLast(SLASH);
		Name << file.AfterLast(SLASH);
		Path2 << wxString(filename).BeforeLast(SLASH);

		MakeDirs(Path1,Path2);

		file.Empty();
		file << Path1 << SLASH << Path2 << SLASH << Name;
	}

	wxFFileOutputStream f(file, wxT("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), file);
		return false;
	}

	// ===================================================
	// FORM		// Format Declaration
	//
	// Always exempt from the length of the file!
	// ===================================================
	f.Write("FORM", 4);
	f.Write(reinterpret_cast<char *>(&fileLen), 4);

	// ===================================================
	// LWO2
	//
	// Declares this is the Lightwave Object 2 file format.
	// LWOB is the first format. It doesn't have a lot of the cool stuff LWO2 has...
	// ===================================================
	f.Write("LWO2", 4);
	fileLen += 4;

	// ===================================================
	// TAGS
	//
	// Used for various Strings. Known string types, in order:
	//		Sketch Color Names
	//		Part Names
	//		Surface Names
	// ===================================================
	f.Write("TAGS", 4);
	uint32 tagsSize = 0;
	f.Write(reinterpret_cast<char *>(&tagsSize), 4);
	fileLen += 8;

	// For each Tag...
	for (int i=0; i<TagCount; i++){
		wxString tagName = Tags[i];

		tagName.Append(_T('\0'));
		if (fmod((float)tagName.length(), 2.0f) > 0)
			tagName.Append(_T('\0'));
		f.Write(tagName.data(), tagName.length());
		tagsSize += tagName.length();
	}
	// Correct TAGS Length
	off_t = -4-tagsSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4<uint32>(tagsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	fileLen += tagsSize;


	// -------------------------------------------------
	// Generate our Layers
	//
	// Point, Poly & Vertex Map data will be nested in
	// our layers.
	// -------------------------------------------------
	for (int l=0;l<LayerNum;l++){
		// Define a Layer & It's data
		uint16 LayerNameSize = Layers[l].Name.Len();
		uint16 LayerSize = 18+LayerNameSize;
		f.Write("LAYR", 4);
		u32 = MSB4<uint32>(LayerSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		fileLen += 8;

		// Layer Number
		u16 = MSB2(l);
		f.Write(reinterpret_cast<char *>(&u16), 2);
		// Flags
		f.Write(reinterpret_cast<char *>(0), 2);
		// Pivot
		f.Write(reinterpret_cast<char *>(0), 4);
		f.Write(reinterpret_cast<char *>(0), 4);
		f.Write(reinterpret_cast<char *>(0), 4);
		// Name
		if (LayerNameSize>0){
			f.Write(Layers[l].Name, LayerNameSize);
		}
		// Parent
		f.Write(reinterpret_cast<char *>(0), 2);
		fileLen += LayerSize;


		// -------------------------------------------------
		// Points Chunk
		//
		// There will be new Point Chunk for every Layer, so if we go
		// beyond 1 Layer, this should be nested.
		// -----------------------------------------

		uint32 pointsSize = 0;
		f.Write("PNTS", 4);
		u32 = MSB4<uint32>(pointsSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		fileLen += 8;

		// Writes the point data
		for (int i=0; i<Layers[l].PointCount; i++) {
			Vec3D vert;
			vert.x = MSB4<float>(Layers[l].Points[i].x);
			vert.y = MSB4<float>(Layers[l].Points[i].z);
			vert.z = MSB4<float>(Layers[l].Points[i].y);

			f.Write(reinterpret_cast<char *>(&vert.x), 4);
			f.Write(reinterpret_cast<char *>(&vert.y), 4);
			f.Write(reinterpret_cast<char *>(&vert.z), 4);

			pointsSize += 12;
			free (vert);
		}
		// Corrects the filesize...
		fileLen += pointsSize;
		off_t = -4-pointsSize;
		f.SeekO(off_t, wxFromCurrent);
		u32 = MSB4<uint32>(pointsSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		f.SeekO(0, wxFromEnd);


		// -----------------------------------------
		// Point Vertex Maps
		// UV, Weights, Vertex Color Maps, etc.
		// -----------------------------------------
		f.Write("VMAP", 4);
		uint32 vmapSize = 0;
		u32 = MSB4<uint32>(vmapSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		fileLen += 8;
*/
	/*
		// UV Data
		f.Write("TXUV", 4);
		dimension = 2;
		dimension = MSB2(dimension);
		f.Write(reinterpret_cast<char *>(&dimension), 2);
		vmapSize += 6;

		wxString UVMapName;
		UVMapName << filename << '\0';
		if (fmod((float)UVMapName.length(), 2.0f) > 0)
			UVMapName.Append(_T('\0'));
		f.Write(UVMapName.data(), UVMapName.length());
		vmapSize += UVMapName.length();
		*/
		/*
		for (int g=0; g<m->nGroups; g++) {
			for (int b=0; b<m->groups[g].nBatches; b++)
			{
				WMOBatch *batch = &m->groups[g].batches[b];
				for(int ii=0;ii<batch->indexCount;ii++)
				{
					int a = m->groups[g].indices[batch->indexStart + ii];
					u16 = MSB2(counter);
					f.Write(reinterpret_cast<char *>(&u16), 2);
					f32 = MSB4<float>(m->groups[g].texcoords[a].x);
					f.Write(reinterpret_cast<char *>(&f32), 4);
					f32 = MSB4<float>(1 - m->groups[g].texcoords[a].y);
					f.Write(reinterpret_cast<char *>(&f32), 4);
					counter++;
					if (counter == 256)
						counter = 0;
					vmapSize += 10;
				}
			}
		}
		*/
	/*
		fileLen += vmapSize;

		off_t = -4-vmapSize;
		f.SeekO(off_t, wxFromCurrent);
		u32 = MSB4<uint32>(vmapSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		f.SeekO(0, wxFromEnd);

		// -----------------------------------------
		// Polygon Chunk
		// -----------------------------------------
		if (PolyCount > 0){
			f.Write("POLS", 4);
			uint32 polySize = 4;
			u32 = MSB4(polySize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			fileLen += 8; // FACE is handled in the PolySize
			f.Write("FACE", 4);

			PolyChunk32 swapped;
			int size = 2;
			for (int p=0;p<PolyCount;p++){
				swapped.numVerts = MSB2(Polys[p].numVerts);
				if (Polys[p].indice[0] < 0xFF){
					swapped.indice[0] =  MSB2((Polys[p].indice[0] & 0x0000FFFF));
					size += 2;
				}else{
					swapped.indice[0] =  MSB4((Polys[p].indice[0] + 0xFF000000));
					size += 4;
				}
				if (Polys[p].indice[2] < 0xFF){
					swapped.indice[1] =  MSB2((Polys[p].indice[2] & 0x0000FFFF));
					size += 2;
				}else{
					swapped.indice[1] =  MSB4((Polys[p].indice[2] + 0xFF000000));
					size += 4;
				}
				if (Polys[p].indice[1] < 0xFF){
					swapped.indice[2] =  MSB2((Polys[p].indice[1] & 0x0000FFFF));
					size += 2;
				}else{
					swapped.indice[2] =  MSB4((Polys[p].indice[1] + 0xFF000000));
					size += 4;
				}

				polySize += size;
				f.Write(reinterpret_cast<char *>(&swapped), size);
				wxLogMessage(_T("Writing polygon %i..."), p);
			}
			off_t = -4-polySize;
			f.SeekO(off_t, wxFromCurrent);
			u32 = MSB4(polySize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.SeekO(0, wxFromEnd);
			
			fileLen += polySize;
		}
	*/
/*
	}

	// Correct File Length
	f.SeekO(4, wxFromStart);
	u32 = MSB4<uint32>(fileLen);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	f.Close();
*/
	// If we've gotten this far, then the file is good!
	return true;
}



// No longer writes data to a LWO file. Instead, it collects the data, and send it to a seperate function that writes the actual file.
void ExportWMOtoLWO2(WMO *m, const char *fn)
{
	wxString filename(fn, wxConvUTF8);

	wxString Tags[1];
	int TagCount = 0;
	int LayerNum = 1;
	std::vector<LWLayer> Layers;






	WriteLWObject(filename, Tags, LayerNum, Layers);


/*
	uint32 counter=0;
	uint32 TagCounter=0;
	uint16 PartCounter=0;
	uint16 SurfCounter=0;
	unsigned int numVerts = 0;
	unsigned int numGroups = 0;


	// Texture Name Array
	// Find a Match for mat->tex and place it into the Texture Name Array.
	wxString *texarray = new wxString[m->nTextures+1];
	for (int g=0;g<m->nGroups;g++) {
		for (int b=0; b<m->groups[g].nBatches; b++)
		{
			WMOBatch *batch = &m->groups[g].batches[b];
			WMOMaterial *mat = &m->mat[batch->texture];
			wxString outname(fn, wxConvUTF8);

			bool nomatch = true;
			for (int t=0;t<=m->nTextures; t++) {
				if (t == mat->tex) {
					texarray[mat->tex] = m->textures[t-1];
					texarray[mat->tex] = texarray[mat->tex].BeforeLast('.');
					nomatch = false;
					break;
				}
			}
			if (nomatch == true){
				texarray[mat->tex] = outname << wxString::Format(_T("_Material_%03i"), mat->tex);
			}
		}
	}

	// Part Names
	for (int g=0;g<m->nGroups;g++) {
		wxString partName = m->groups[g].name;

		partName.Append(_T('\0'));
		if (fmod((float)partName.length(), 2.0f) > 0)
			partName.Append(_T('\0'));
		f.Write(partName.data(), partName.length());
		tagsSize += partName.length();
	}

	// Surface Names
	wxString *surfarray = new wxString[m->nTextures+1];
	for (unsigned int t=0;t<m->nTextures;t++){
		wxString matName = wxString(m->textures[t]).BeforeLast('.');

		matName.Append(_T('\0'));
		if (fmod((float)matName.length(), 2.0f) > 0)
			matName.Append(_T('\0'));
		f.Write(matName.data(), matName.length());
		tagsSize += matName.length();
		surfarray[t] = wxString(m->textures[t]).BeforeLast('.');
	}

	off_t = -4-tagsSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4(tagsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	fileLen += tagsSize;



	// ===================================================
	// LAYR
	//
	// Specifies the start of a new layer. Each layer has it's own Point & Poly
	// chunk, which tells it what data is on what layer. It's probably best
	// to only have 1 layer for now.
	// ===================================================
	f.Write("LAYR", 4);
	u32 = MSB4(18);
	fileLen += 8;
	f.Write(reinterpret_cast<char *>(&u32), 4);
	ub = 0;
	for(int i=0; i<18; i++) {
		f.Write(reinterpret_cast<char *>(&ub), 1);
	}
	fileLen += 18;
	// ================


	// ===================================================
	// POINTS CHUNK, this is the vertice data
	// The PNTS chunk contains triples of floating-point numbers, the coordinates of a list of points. The numbers are written 
	// as IEEE 32-bit floats in network byte order. The IEEE float format is the standard bit pattern used by almost all CPUs 
	// and corresponds to the internal representation of the C language float type. In other words, this isn't some bizarre 
	// proprietary encoding. You can process these using simple fread and fwrite calls (but don't forget to correct the byte 
	// order if necessary).
	// ===================================================
	uint32 pointsSize = 0;
	f.Write("PNTS", 4);
	u32 = MSB4(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;

	unsigned int bindice = 0;
	unsigned int gverts = 0;

	// output all the vertice data
	for (int g=0; g<m->nGroups; g++) {
		gverts += m->groups[g].nVertices;
		for (int b=0; b<m->groups[g].nBatches; b++)	{
			WMOBatch *batch = &m->groups[g].batches[b];

			bindice += batch->indexCount;
			for(int ii=0;ii<batch->indexCount;ii++){
				int a = m->groups[g].indices[batch->indexStart + ii];
				Vec3D vert;
				vert.x = MSB4<float>(m->groups[g].vertices[a].x);
				vert.y = MSB4<float>(m->groups[g].vertices[a].z);
				vert.z = MSB4<float>(m->groups[g].vertices[a].y);
				f.Write(reinterpret_cast<char *>(&vert.x), 4);
				f.Write(reinterpret_cast<char *>(&vert.y), 4);
				f.Write(reinterpret_cast<char *>(&vert.z), 4);
				pointsSize += 12;

				numVerts++;
			}
			numGroups++;
		}
	}
#ifdef _DEBUG
	wxLogMessage("WMO Point Count: %i, Stored Indices: %i, Stored Verticies: %i",numVerts, bindice, gverts);
#endif


	fileLen += pointsSize;
	off_t = -4-pointsSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	// ================


	// --
	// The bounding box for the layer, just so that readers don't have to scan the PNTS chunk to find the extents.
	f.Write("BBOX", 4);
	u32 = MSB4(24);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	Vec3D vert;
	vert.x = MSB4(m->v1.x);
	vert.z = MSB4(m->v1.y);
	vert.y = MSB4(m->v1.z);
	f.Write(reinterpret_cast<char *>(&vert.x), 4);
	f.Write(reinterpret_cast<char *>(&vert.y), 4);
	f.Write(reinterpret_cast<char *>(&vert.z), 4);
	vert.x = MSB4(m->v2.x);
	vert.z = MSB4(m->v2.y);
	vert.y = MSB4(m->v2.z);
	f.Write(reinterpret_cast<char *>(&vert.x), 4);
	f.Write(reinterpret_cast<char *>(&vert.y), 4);
	f.Write(reinterpret_cast<char *>(&vert.z), 4);
	fileLen += 32;



	// Removed Point Vertex Mapping for WMOs.
	// UV Map now generated by Discontinuous Vertex Mapping, down in the Poly section.

	uint32 vmapSize = 0;

	//Vertex Mapping
	f.Write("VMAP", 4);
	u32 = MSB4(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	// UV Data
	f.Write("TXUV", 4);
	dimension = MSB2(2);
	f.Write(reinterpret_cast<char *>(&dimension), 2);
	f.Write("Texture", 7);
	ub = 0;
	f.Write(reinterpret_cast<char *>(&ub), 1);
	vmapSize += 14;

	// u16, f32, f32
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			for(int ii=0;ii<batch->indexCount;ii++)
			{
				int a = m->groups[i].indices[batch->indexStart + ii];


				uint16 indice16;
				uint32 indice32;

				uint16 counter16 = (counter & 0x0000FFFF);
				uint32 counter32 = counter + 0xFF000000;

				if (counter < 0xFF00){
					indice16 = MSB2(counter16);
					f.Write(reinterpret_cast<char *>(&indice16),2);
					vmapSize += 2;
				}else{
					indice32 = MSB4(counter32);
					f.Write(reinterpret_cast<char *>(&indice32), 4);
					vmapSize += 4;
				}

				f32 = MSB4(m->groups[i].texcoords[a].x);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				f32 = MSB4(1 - m->groups[i].texcoords[a].y);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				vmapSize += 8;
				counter++;
			}
		}
	}
	fileLen += vmapSize;

	off_t = -4-vmapSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);




	// --
	// POLYGON CHUNK
	// The POLS chunk contains a list of polygons. A "polygon" in this context is anything that can be described using an 
	// ordered list of vertices. A POLS of type FACE contains ordinary polygons, but the POLS type can also be CURV, 
	// PTCH, MBAL or BONE, for example.
	//
	// The high 6 bits of the vertex count for each polygon are reserved for flags, which in effect limits the number of 
	// vertices per polygon to 1023. Don't forget to mask the high bits when reading the vertex count. The flags are 
	// currently only defined for CURVs.
	// 
	// The point indexes following the vertex count refer to the points defined in the most recent PNTS chunk. Each index 
	// can be a 2-byte or a 4-byte integer. If the high order (first) byte of the index is not 0xFF, the index is 2 bytes long. 
	// This allows values up to 65279 to be stored in 2 bytes. If the high order byte is 0xFF, the index is 4 bytes long and 
	// its value is in the low three bytes (index & 0x00FFFFFF). The maximum value for 4-byte indexes is 16,777,215 (224 - 1). 
	// Objects with more than 224 vertices can be stored using multiple pairs of PNTS and POLS chunks.
	// 
	// The cube has 6 square faces each defined by 4 vertices. LightWave polygons are single-sided by default 
	// (double-sidedness is a possible surface property). The vertices are listed in clockwise order as viewed from the 
	// visible side, starting with a convex vertex. (The normal is defined as the cross product of the first and last edges.)

	f.Write("POLS", 4);
	uint32 polySize = 4;
	u32 = MSB4(polySize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8; // FACE is handled in the PolySize
	f.Write("FACE", 4);

	counter = 0;
	unsigned long polycount = 0;
	uint32 gpolys = 0;
	unsigned int tnBatches = 0;
	
	for (int g=0;g<m->nGroups;g++) {
		gpolys += m->groups[g].nTriangles;
		for (int b=0; b<m->groups[g].nBatches; b++){
			WMOBatch *batch = &m->groups[g].batches[b];
			for (unsigned int k=0; k<batch->indexCount; k+=3) {
				uint16 nverts;

				// Write the number of Verts
				nverts = MSB2(3);
				f.Write(reinterpret_cast<char *>(&nverts),2);
				polySize += 2;

				for (int x=0;x<3;x++,counter++){
					//wxLogMessage("Batch %i, index %i, x=%i",b,k,x);
					uint16 indice16;
					uint32 indice32;

					int mod = 0;
					if (x==1){
						mod = 1;
					}else if (x==2){
						mod = -1;
					}

					uint16 counter16 = ((counter+mod) & 0x0000FFFF);
					uint32 counter32 = (counter+mod) + 0xFF000000;

					if ((counter+mod) < 0xFF00){
						indice16 = MSB2(counter16);
						f.Write(reinterpret_cast<char *>(&indice16), 2);
						polySize += 2;
					}else{
						//wxLogMessage("Counter above limit: %i", counter);
						indice32 = MSB4(counter32);
						f.Write(reinterpret_cast<char *>(&indice32), 4);
						polySize += 4;
					}
				}
				polycount++;
			}
			tnBatches++;
		}
	}

	off_t = -4-polySize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4(polySize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

#ifdef _DEBUG
	wxLogMessage("WMO Poly Count: %i, Stored Polys: %i, Polysize: %i, Stored Polysize: %i",polycount, gpolys, polySize,gpolys * sizeof(PolyChunk16) );
	wxLogMessage("WMO nGroups: %i, nBatchs: %i",m->nGroups, tnBatches);
#endif

	fileLen += polySize;
	// ========


	// --
	// The PTAG chunk associates tags with polygons. In this case, it identifies which surface is assigned to each polygon. 
	// The first number in each pair is a 0-based index into the most recent POLS chunk, and the second is a 0-based 
	// index into the TAGS chunk.

	// NOTE: Every PTAG type needs a seperate PTAG call!

	TagCounter = 0;
	PartCounter = 0;
	counter=0;
	int32 ptagSize;

	// Parts PolyTag
	f.Write("PTAG", 4);
	ptagSize = 4;
	counter=0;
	u32 = MSB4(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write("PART", 4);
	for (int g=0;g<m->nGroups;g++) {
		for (int b=0; b<m->groups[g].nBatches; b++) {
			WMOBatch *batch = &m->groups[g].batches[b];

			for (unsigned int k=0; k<batch->indexCount; k+=3) {
				uint16 indice16;
				uint32 indice32;

				uint16 counter16 = (counter & 0x0000FFFF);
				uint32 counter32 = counter + 0xFF000000;

				if (counter < 0xFF00){
					indice16 = MSB2(counter16);
					f.Write(reinterpret_cast<char *>(&indice16),2);
					ptagSize += 2;
				}else{
					indice32 = MSB4(counter32);
					f.Write(reinterpret_cast<char *>(&indice32), 4);
					ptagSize += 4;
				}

				u16 = MSB2(TagCounter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				ptagSize += 2;
				counter++;
			}
		}
		TagCounter++;
		PartCounter++;
	}
	fileLen += ptagSize;

	off_t = -4-ptagSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);


	// Surface PolyTag
	counter = 0;
	SurfCounter = 0;
	f.Write("PTAG", 4);
	ptagSize = 4;
	u32 = MSB4(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write("SURF", 4);
	for (int g=0;g<m->nGroups;g++) {
		for (int b=0; b<m->groups[g].nBatches; b++) {
			WMOBatch *batch = &m->groups[g].batches[b];

			for (unsigned int k=0; k<batch->indexCount; k+=3) {
				uint16 indice16;
				uint32 indice32;

				uint16 counter16 = (counter & 0x0000FFFF);
				uint32 counter32 = counter + 0xFF000000;

				if (counter < 0xFF00){
					indice16 = MSB2(counter16);
					f.Write(reinterpret_cast<char *>(&indice16),2);
					ptagSize += 2;
				}else{
					indice32 = MSB4(counter32);
					f.Write(reinterpret_cast<char *>(&indice32), 4);
					ptagSize += 4;
				}

				int surfID = TagCounter + 0;
				for (int x=0;x<m->nTextures;x++){
					wxString target = texarray[m->mat[batch->texture].tex];
					if (surfarray[x] == target){
						surfID = TagCounter + x;
						break;
					}
				}

				u16 = MSB2(surfID);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				ptagSize += 2;
				counter++;
			}
			SurfCounter++;
		}
	}
	fileLen += ptagSize;

	off_t = -4-ptagSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);


	// ===================================================
	//VMPA		// Vertex Map Parameters, Always Preceeds a VMAP & VMAD. 4bytes: Size, then Num Vars (2) * 4 bytes.
				// UV Sub Type: 0-Linear, 1-Subpatched, 2-Linear Corners, 3-Linear Edges, 4-Across Discontinuous Edges.
				// Sketch Color: 0-12; 6-Default Gray
	// ===================================================
	f.Write("VMPA", 4);
	u32 = MSB4(8);	// We got 2 Paramaters, * 4 Bytes.
	f.Write(reinterpret_cast<char *>(&u32), 4);
	u32 = 0;							// Linear UV Sub Type
	f.Write(reinterpret_cast<char *>(&u32), 4);
	u32 = MSB4(6);	// Default Gray
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 16;


	// ===================================================
	// Discontinuous Vertex Mapping
	// ===================================================

	int32 vmadSize = 0;
	f.Write("VMAD", 4);
	u32 = MSB4(vmadSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	// UV Data
	f.Write("TXUV", 4);
	dimension = 2;
	dimension = MSB2(dimension);
	f.Write(reinterpret_cast<char *>(&dimension), 2);
	f.Write("Texture", 7);
	ub = 0;
	f.Write(reinterpret_cast<char *>(&ub), 1);
	vmadSize += 14;

	counter = 0;
	// u16, u16, f32, f32
	for (int g=0;g<m->nGroups;g++) {
		for (int b=0; b<m->groups[g].nBatches; b++)
		{
			WMOBatch *batch = &m->groups[g].batches[b];

			for (uint32 k=0, b=0; k<batch->indexCount; k++,b++) {
				int a = m->groups[g].indices[batch->indexStart + k];
				uint16 indice16;
				uint32 indice32;

				uint16 counter16 = (counter & 0x0000FFFF);
				uint32 counter32 = counter + 0xFF000000;

				if (counter < 0xFF00){
					indice16 = MSB2(counter16);
					f.Write(reinterpret_cast<char *>(&indice16),2);
					vmadSize += 2;
				}else{
					indice32 = MSB4(counter32);
					f.Write(reinterpret_cast<char *>(&indice32), 4);
					vmadSize += 4;
				}

				if (((counter/3) & 0x0000FFFF) < 0xFF00){
					indice16 = MSB2(((counter/3) & 0x0000FFFF));
					f.Write(reinterpret_cast<char *>(&indice16),2);
					vmadSize += 2;
				}else{
					indice32 = MSB4((counter/3) + 0xFF000000);
					f.Write(reinterpret_cast<char *>(&indice32), 4);
					vmadSize += 4;
				}
			//	u16 = MSB2((uint16)(counter/3));
			//	f.Write(reinterpret_cast<char *>(&u16), 2);
				f32 = MSB4(m->groups[g].texcoords[a].x);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				f32 = MSB4(1 - m->groups[g].texcoords[a].y);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				counter++;
				vmadSize += 8;
			}
		}
	}
	fileLen += vmadSize;
	off_t = -4-vmadSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = MSB4(vmadSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);



	// ===================================================
	// Texture File List
	// ===================================================
	uint32 surfaceCounter = PartCounter;
	uint32 ClipCount = 0;
	for (int g=0;g<m->nGroups;g++) {
		for (int b=0; b<m->groups[g].nBatches; b++)
		{
			WMOBatch *batch = &m->groups[g].batches[b];
			WMOMaterial *mat = &m->mat[batch->texture];

			int clipSize = 0;
			f.Write("CLIP", 4);
			u32 = MSB4(0);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			fileLen += 8;

			u32 = MSB4(++surfaceCounter);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.Write("STIL", 4);
			clipSize += 8;

			glBindTexture(GL_TEXTURE_2D, mat->tex);
			//wxLogMessage("Opening %s so we can save it...",texarray[mat->tex]);
			wxString FilePath = wxString(fn, wxConvUTF8).BeforeLast(SLASH);
			wxString texName = texarray[mat->tex];
			wxString texPath = texName.BeforeLast(SLASH);
			texName = texName.AfterLast(SLASH);

			wxString sTexName = "";
			if (modelExport_LW_PreserveDir == true){
				sTexName += "Images/";
			}
			if (modelExport_PreserveDir == true){
				sTexName += texPath;
				sTexName << "\\";
				sTexName.Replace(_T("\\"),_T("/"));
			}
			sTexName += texName;

			sTexName << _T(".tga") << '\0';

			if (fmod((float)sTexName.length(), 2.0f) > 0)
				sTexName.Append(_T('\0'));

			u16 = MSB2(sTexName.length());
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.Write(sTexName.data(), sTexName.length());
			clipSize += (2+sTexName.length());

			// update the chunks length
			off_t = -4-clipSize;
			f.SeekO(off_t, wxFromCurrent);
			u32 = MSB4(clipSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.SeekO(0, wxFromEnd);

			// save texture to file
			wxString texFilename;
			texFilename = FilePath;
			texFilename += SLASH;
			texFilename += texName;

			if (modelExport_LW_PreserveDir == true){
				MakeDirs(FilePath,"Images");

				texFilename.Empty();
				texFilename = FilePath << "\\" << "Images" << "\\" << texName;
			}

			if (modelExport_PreserveDir == true){
				wxString Path;
				Path << texFilename.BeforeLast(SLASH);

				MakeDirs(Path,texPath);

				texFilename.Empty();
				texFilename = Path << SLASH << texPath << SLASH << texName;
			}
			texFilename << (".tga");
			wxLogMessage("Saving Image: %s",texFilename);

			// setup texture
			SaveTexture(texFilename);

			fileLen += clipSize;
		}
	}
	// ================


	// --
	wxString surfName;
	surfaceCounter = PartCounter;
	for (int g=0;g<m->nGroups;g++) {
		for (int b=0; b<m->groups[g].nBatches; b++)
		{
			WMOBatch *batch = &m->groups[g].batches[b];
			WMOMaterial *mat = &m->mat[batch->texture];

			// Surface name
			//surfName = wxString::Format(_T("Material_%03i"),mat->tex);
			++surfaceCounter;

			surfName = texarray[mat->tex];			
		}
	}
	// ================


	f.SeekO(4, wxFromStart);
	u32 = MSB4(fileLen);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	f.Close();

	// Cleanup, Isle 3!
	wxDELETEA(texarray);

	// Export Lights & Doodads
	//if ((modelExport_LW_ExportDoodads == true)||(modelExport_LW_ExportLights == true)){
	//	ExportWMOObjectstoLWO(m,fn);
	//}
	*/

}

void ExportADTtoLWO(MapTile *m, const char *fn){
	wxString file = wxString(fn, wxConvUTF8);

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << file.BeforeLast(SLASH);
		Name << file.AfterLast(SLASH);

		MakeDirs(Path,_T("Objects"));

		file.Empty();
		file << Path << SLASH<<_T("Objects")<<SLASH << Name;
	}
	if (modelExport_PreserveDir == true){
		wxString Path1, Path2, Name;
		Path1 << file.BeforeLast(SLASH);
		Name << file.AfterLast(SLASH);
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);

		MakeDirs(Path1,Path2);

		file.Empty();
		file << Path1 << SLASH << Path2 << SLASH << Name;
	}
#ifndef _WINDOWS
	file.Replace(wxT("\\"),wxT("/"));
#endif
	wxFFileOutputStream f(file, _T("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), file.c_str());
		wxMessageDialog(g_modelViewer,_T("Could not open file for exporting."),_T("Exporting Error..."));
		return;
	}
	LogExportData(_T("LWO"),wxString(fn, wxConvUTF8).BeforeLast(SLASH),_T("ADT"));

	

	wxLogMessage(_T("ADT Export completed."));
}