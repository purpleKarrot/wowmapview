#ifndef MODELHEADERS_H
#define MODELHEADERS_H

#include <stdint.h>
#include "vec3d.h"

#pragma pack(push,1)

//float floats[14];
struct PhysicsSettings {
	Vec3D VertexBox[2];
	float VertexRadius;
	Vec3D BoundingBox[2];
	float BoundingRadius;
};

struct ModelHeader {
	char id[4];
	uint8_t version[4];
	uint32_t nameLength;
	uint32_t nameOfs;
	uint32_t GlobalModelFlags; // 1: tilt x, 2: tilt y, 4:, 8: add another field in header, 16: ; (no other flags as of 3.1.1);

	uint32_t nGlobalSequences; // AnimationRelated
	uint32_t ofsGlobalSequences; // A list of timestamps.
	uint32_t nAnimations; // AnimationRelated
	uint32_t ofsAnimations; // Information about the animations in the model.
	uint32_t nAnimationLookup; // AnimationRelated
	uint32_t ofsAnimationLookup; // Mapping of global IDs to the entries in the Animation sequences block.
	//uint32 nD;
	//uint32 ofsD;
	uint32_t nBones; // BonesAndLookups
	uint32_t ofsBones; // Information about the bones in this model.
	uint32_t nKeyBoneLookup; // BonesAndLookups
	uint32_t ofsKeyBoneLookup; // Lookup table for key skeletal bones.

	uint32_t nVertices; // GeometryAndRendering
	uint32_t ofsVertices; // Vertices of the model.
	uint32_t nViews; // GeometryAndRendering
	//uint32 ofsViews; // Views (LOD) are now in .skins.

	uint32_t nColors; // ColorsAndTransparency
	uint32_t ofsColors; // Color definitions.

	uint32_t nTextures; // TextureAndTheifAnimation
	uint32_t ofsTextures; // Textures of this model.

	uint32_t nTransparency; // H,  ColorsAndTransparency
	uint32_t ofsTransparency; // Transparency of textures.
	//uint32 nI;   // always unused ?
	//uint32 ofsI;
	uint32_t nTexAnims;	// J, TextureAndTheifAnimation
	uint32_t ofsTexAnims;
	uint32_t nTexReplace; // TextureAndTheifAnimation
	uint32_t ofsTexReplace; // Replaceable Textures.

	uint32_t nTexFlags; // Render Flags
	uint32_t ofsTexFlags; // Blending modes / render flags.
	uint32_t nBoneLookup; // BonesAndLookups
	uint32_t ofsBoneLookup; // A bone lookup table.

	uint32_t nTexLookup; // TextureAndTheifAnimation
	uint32_t ofsTexLookup; // The same for textures.

	uint32_t nTexUnitLookup;		// L, TextureAndTheifAnimation
	uint32_t ofsTexUnitLookup; // And texture units. Somewhere they have to be too.
	uint32_t nTransparencyLookup; // M, ColorsAndTransparency
	uint32_t ofsTransparencyLookup; // Everything needs its lookup. Here are the transparencies.
	uint32_t nTexAnimLookup; // TextureAndTheifAnimation
	uint32_t ofsTexAnimLookup; // Wait. Do we have animated Textures? Wasn't ofsTexAnims deleted? oO

	struct PhysicsSettings ps;

	uint32_t nBoundingTriangles; // Miscellaneous
	uint32_t ofsBoundingTriangles;
	uint32_t nBoundingVertices; // Miscellaneous
	uint32_t ofsBoundingVertices;
	uint32_t nBoundingNormals; // Miscellaneous
	uint32_t ofsBoundingNormals;

	uint32_t nAttachments; // O, Miscellaneous
	uint32_t ofsAttachments; // Attachments are for weapons etc.
	uint32_t nAttachLookup; // P, Miscellaneous
	uint32_t ofsAttachLookup; // Of course with a lookup.
	uint32_t nEvents; //
	uint32_t ofsEvents; // Used for playing sounds when dying and a lot else.
	uint32_t nLights; // R
	uint32_t ofsLights; // Lights are mainly used in loginscreens but in wands and some doodads too.
	uint32_t nCameras; // S, Miscellaneous
	uint32_t ofsCameras; // The cameras are present in most models for having a model in the Character-Tab.
	uint32_t nCameraLookup; // Miscellaneous
	uint32_t ofsCameraLookup; // And lookup-time again.
	uint32_t nRibbonEmitters; // U, Effects
	uint32_t ofsRibbonEmitters; // Things swirling around. See the CoT-entrance for light-trails.
	uint32_t nParticleEmitters; // V, Effects
	uint32_t ofsParticleEmitters; // Spells and weapons, doodads and loginscreens use them. Blood dripping of a blade? Particles.
};

// block B - animations
struct ModelAnimation {
	uint16_t animID;
	uint16_t subAnimID;
	uint32_t length;

	float moveSpeed;

	uint32_t flags;
	uint16_t probability; // This is used to determine how often the animation is played. For all animations of the same type, this adds up to 0x7FFF (32767).
	uint16_t unused;
	uint32_t d1;
	uint32_t d2;
	uint32_t playSpeed;  // note: this can't be play speed because it's 0 for some models

	Vec3D boxA, boxB; // Minimum Extent, Maximum Extend
	float rad; // Bounds Radius

	int16_t NextAnimation;
	int16_t Index;
};

struct AnimationBlockHeader
{
	uint32_t nEntrys;
	uint32_t ofsEntrys;
};

// sub-block in block E - animation data
struct AnimationBlock {
	int16_t type;		// interpolation type (0=none, 1=linear, 2=hermite)
	int16_t seq;		// global sequence id or -1
	uint32_t nTimes;
	uint32_t ofsTimes;
	uint32_t nKeys;
	uint32_t ofsKeys;
};

struct FakeAnimationBlock {
	uint32_t nTimes;
	uint32_t ofsTimes;
	uint32_t nKeys;
	uint32_t ofsKeys;
};

#define	MODELBONE_BILLBOARD	8
#define	MODELBONE_TRANSFORM	512
// block E - bones
struct ModelBoneDef {
	int32_t keyboneid; // Back-reference to the key bone lookup table. -1 if this is no key bone.
	int32_t flags; // Only known flags: 8 - billboarded and 512 - transformed
	int16_t parent; // parent bone index
	int16_t geoid; // A geoset for this bone.
	int32_t unknown; // new int added to the bone definitions.  Added in WoW 2.0
	AnimationBlock translation; // (short, vec3f)
	AnimationBlock rotation; // (short, vec4s)
	AnimationBlock scaling; // (short, vec3f)
	Vec3D pivot;
};

struct ModelTexAnimDef {
	AnimationBlock trans, rot, scale;
};

struct ModelVertex {
	Vec3D pos;
	uint8_t weights[4];
	uint8_t bones[4];
	Vec3D normal;
	Vec2D texcoords;
	int unk1, unk2; // always 0,0 so this is probably unused
};

struct ModelView {
	char id[4];				 // Signature
	uint32_t nIndex, ofsIndex; // Vertices in this model (index into vertices[])
	uint32_t nTris, ofsTris;	 // indices
	uint32_t nProps, ofsProps; // additional vtx properties
	uint32_t nSub, ofsSub;	 // materials/renderops/submeshes
	uint32_t nTex, ofsTex;	 // material properties/textures
	int32_t lod;				 // LOD bias?
};


/// One material + render operation
struct ModelGeoset {
	uint32_t id;		// mesh part id?
	uint16_t vstart;	// first vertex
	uint16_t vcount;	// num vertices
	uint16_t istart;	// first index
	uint16_t icount;	// num indices
	uint16_t nBones;		// number of bone indices, Number of elements in the bone lookup table.
	uint16_t StartBones;		// ? always 1 to 4, Starting index in the bone lookup table.
	uint16_t d5;		// ?
	uint16_t rootBone;		// root bone?
	Vec3D BoundingBox[2];
	float radius;
};

#define	TEXTUREUNIT_STATIC	16
/// A texture unit (sub of material)
struct ModelTexUnit{
	// probably the texture units
	// size always >=number of materials it seems
	uint16_t flags;		// Usually 16 for static textures, and 0 for animated textures.
	uint16_t shading;		// If set to 0x8000: shaders. Used in skyboxes to ditch the need for depth buffering. See below.
	uint16_t op;			// Material this texture is part of (index into mat)
	uint16_t op2;			// Always same as above?
	int16_t colorIndex;	// A Color out of the Colors-Block or -1 if none.
	uint16_t flagsIndex;	// RenderFlags (index into render flags, TexFlags)
	uint16_t texunit;		// Index into the texture unit lookup table.
	uint16_t mode;		// See below.
	uint16_t textureid;	// Index into Texture lookup table
	uint16_t texunit2;	// copy of texture unit value?
	uint16_t transid;		// Index into transparency lookup table.
	uint16_t texanimid;	// Index into uvanimation lookup table.
};

enum TextureFlags {
	TEXTURE_WRAPX=1,
	TEXTURE_WRAPY
};

/*
Shader thingey
Its actually two uint8s defining the shader used. Everything below this is in binary. X represents a variable digit.
Depending on "Mode", its either "Diffuse_%s_%s" and "Combiners_%s_%s" (Mode=0) or "Diffuse_%s" and "Combiners_%s" (Mode>0).


Diffuse
Mode   Shading     String
0      0XXX 0XXX   Diffuse_T1_T2
0      0XXX 1XXX   Diffuse_T1_Env
0      1XXX 0XXX   Diffuse_Env_T2
0      1XXX 1XXX   Diffuse_Env_Env

1      0XXX XXXX   Diffuse_T1
1      1XXX XXXX   Diffuse_Env

Combiners
Mode   Shading     String
0      X000 XXXX   Combiners_Opaque_%s
0      X001 XXXX   Combiners_Mod_%s
0      X011 XXXX   Combiners_Add_%s
0      X100 XXXX   Combiners_Mod2x_%s

0      XXXX X000   Combiners_%s_Opaque
0      XXXX X001   Combiners_%s_Mod
0      XXXX X011   Combiners_%s_Add
0      XXXX X100   Combiners_%s_Mod2x
0      XXXX X110   Combiners_%s_Mod2xNA
0      XXXX X111   Combiners_%s_AddNA

1      X000 XXXX   Combiners_Opaque
1      X001 XXXX   Combiners_Mod
1      X010 XXXX   Combiners_Decal
1      X011 XXXX   Combiners_Add
1      X100 XXXX   Combiners_Mod2x
1      X101 XXXX   Combiners_Fade

*/

#define	RENDERFLAGS_UNLIT	1
#define	RENDERFLAGS_UNFOGGED	2
#define	RENDERFLAGS_TWOSIDED	4
#define	RENDERFLAGS_BILLBOARD	8
#define	RENDERFLAGS_ZBUFFERED	16

// block X - render flags
struct ModelRenderFlags {
	uint16_t flags;
	uint16_t blend;
};

// block G - color defs
// For some swirling portals and volumetric lights, these define vertex colors. 
// Referenced from the Texture Unit blocks in the LOD part. Contains a separate timeline for transparency values. 
// If no animation is used, the given value is constant.
struct ModelColorDef {
	AnimationBlock color; // (short, vec3f) Three floats. One for each color.
	AnimationBlock opacity; // (short, short) 0 - transparent, 0x7FFF - opaque.
};

// block H - transparency defs
struct ModelTransDef {
	AnimationBlock trans;
};

#define	TEXTURE_MAX	32
struct ModelTextureDef {
	uint32_t type;
	uint32_t flags;
	uint32_t nameLen;
	uint32_t nameOfs;
};

enum ModelLightTypes {
	MODELLIGHT_DIRECTIONAL=0,
	MODELLIGHT_POINT
};

struct ModelLightDef {
	int16_t type; // 0: Directional, 1: Point light
	int16_t bone; // If its attached to a bone, this is the bone. Else here is a nice -1.
	Vec3D pos; // Position, Where is this light?
	AnimationBlock ambientColor; // The ambient color. Three floats for RGB.
	AnimationBlock ambientIntensity; // A float for the intensity.
	AnimationBlock diffuseColor; // The diffuse color. Three floats for RGB.
	AnimationBlock diffuseIntensity; // A float for the intensity again.
	AnimationBlock attenuationStart; // This defines, where the light starts to be.
	AnimationBlock attenuationEnd; // And where it stops.
	AnimationBlock useAttenuation; // Its an integer and usually 1.
};

struct ModelCameraDef {
	int32_t id; // 0 is potrait camera, 1 characterinfo camera; -1 if none; referenced in CamLookup_Table
	float fov; // No radians, no degrees. Multiply by 35 to get degrees.
	float farclip; // Where it stops to be drawn.
	float nearclip; // Far and near. Both of them.
	AnimationBlock transPos; // How the cameras position moves. Should be 3*3 floats. (? WoW parses 36 bytes = 3*3*sizeof(float))
	Vec3D pos; // float, Where the camera is located.
	AnimationBlock transTarget; // How the target moves. Should be 3*3 floats. (?)
	Vec3D target; // float, Where the camera points to.
	AnimationBlock rot; // The camera can have some roll-effect. Its 0 to 2*Pi.
};

struct ModelParticleParams {
	FakeAnimationBlock colors; 	// (short, vec3f)	This one points to 3 floats defining red, green and blue.
	FakeAnimationBlock opacity;      // (short, short)		Looks like opacity (short), Most likely they all have 3 timestamps for {start, middle, end}.
	FakeAnimationBlock sizes; 		// (short, vec2f)	It carries two floats per key. (x and y scale)
	int32_t d[2];
	FakeAnimationBlock Intensity; 	// Some kind of intensity values seen: 0,16,17,32(if set to different it will have high intensity) (short, short)
	FakeAnimationBlock unk2; 		// (short, short)
	float unk[3];
	float scales[3];
	float slowdown;
	float unknown1[2];
	float rotation;				//Sprite Rotation
	float unknown2[2];
	float Rot1[3];					//Model Rotation 1
	float Rot2[3];					//Model Rotation 2
	float Trans[3];				//Model Translation
	float f2[4];
	int32_t nUnknownReference;
	int32_t ofsUnknownReferenc;
};

#define	MODELPARTICLE_DONOTTRAIL			0x10
#define	MODELPARTICLE_DONOTBILLBOARD	0x1000
struct ModelParticleEmitterDef {
    int32_t id;
	int32_t flags;
	Vec3D pos; // The position. Relative to the following bone.
	int16_t bone; // The bone its attached to.
	int16_t texture; // And the texture that is used.
	int32_t nModelFileName;
	int32_t ofsModelFileName;
	int32_t nParticleFileName;
	int32_t ofsParticleFileName; // TODO
	int8_t blend;
	int8_t EmitterType; // EmitterType	 1 - Plane (rectangle), 2 - Sphere, 3 - Spline? (can't be bothered to find one)
	int16_t ParticleColor; // This one is used so you can assign a color to specific particles. They loop over all
						 // particles and compare +0x2A to 11, 12 and 13. If that matches, the colors from the dbc get applied.
	int8_t ParticleType; // 0 "normal" particle,
					   // 1 large quad from the particle's origin to its position (used in Moonwell water effects)
					   // 2 seems to be the same as 0 (found some in the Deeprun Tram blinky-lights-sign thing)
	int8_t HeadorTail; // 0 - Head, 1 - Tail, 2 - Both
	int16_t TextureTileRotation; // TODO, Rotation for the texture tile. (Values: -1,0,1)
	int16_t cols; // How many different frames are on that texture? People should learn what rows and cols are.
	int16_t rows; // Its different everywhere. I just took it random.
	AnimationBlock EmissionSpeed; // All of the following blocks should be floats.
	AnimationBlock SpeedVariation; // Variation in the flying-speed. (range: 0 to 1)
	AnimationBlock VerticalRange; // Drifting away vertically. (range: 0 to pi)
	AnimationBlock HorizontalRange; // They can do it horizontally too! (range: 0 to 2*pi)
	AnimationBlock Gravity; // Fall down, apple!
	AnimationBlock Lifespan; // Everyone has to die.
	int32_t unknown;
	AnimationBlock EmissionRate; // Stread your particles, emitter.
	int32_t unknown2;
	AnimationBlock EmissionAreaLength; // Well, you can do that in this area.
	AnimationBlock EmissionAreaWidth;
	AnimationBlock Gravity2; // A second gravity? Its strong.
	ModelParticleParams p;
	AnimationBlock en;
};

struct ModelRibbonEmitterDef {
	int32_t id;
	int32_t bone;
	Vec3D pos;
	int32_t nTextures;
	int32_t ofsTextures;
	int32_t nUnknown;
	int32_t ofsUnknown;
	AnimationBlock color;
	AnimationBlock opacity; // And an alpha value in a short, where: 0 - transparent, 0x7FFF - opaque.
	AnimationBlock above; // The height above.
	AnimationBlock below; // The height below. Do not set these to the same!
	float res; // This defines how smooth the ribbon is. A low value may produce a lot of edges.
	float length; // The length aka Lifespan.
	float Emissionangle; // use arcsin(val) to get the angle in degree
	int16_t s1, s2;
	AnimationBlock unk1; // (short)
	AnimationBlock unk2; // (boolean)
	int32_t unknown; // This looks much like just some Padding to the fill up the 0x10 Bytes, always 0
};

/* 
These events are used for timing sounds for example. You can find the $DTH (death) event on nearly every model. It will play the death sound for the unit.
The events you can use depend on the way, the model is used. Dynamic objects can shake the camera, doodads shouldn't. Units can do a lot more than other objects.
Somehow there are some entries, that don't use the $... names but identifiers like "DEST" (destination), "POIN" (point) or "WHEE" (wheel). How they are used? Idk.
*/
struct ModelEventDef {
	char id[4]; // This is a (actually 3 character) name for the event with a $ in front.
	int32_t dbid; // This data is passed when the event is fired.
	int32_t bone; // Somewhere it has to be attached.
	Vec3D pos; // Relative to that bone of course.
	int16_t type; // This is some fake-AnimationBlock.
	int16_t seq; // Built up like a real one but without timestamps(?). What the fuck?
	uint32_t nTimes; // See the documentation on AnimationBlocks at this topic.
	uint32_t ofsTimes; // This points to a list of timestamps for each animation given.
};
/*
There are a lot more of them. I did not list all up to now.
ID	 Data	 Description
DEST		 exploding ballista, that one has a really fucked up block. Oo
POIN	 unk	 something alliance gunship related (flying in icecrown)
WHEE	 601+	 Used on wheels at vehicles.
$tsp		 p is {0 to 3} (position); t is {W, S, B, F (feet) or R} (type); s is {R or L} (right or left); this is used when running through snow for example.
$AHx		 UnitCombat_C, x is {0 to 3}
$BRT		 Plays some sound.
$BTH		 Used for bubbles or breath. ("In front of head")
$BWP		 UnitCombat_C
$BWR		 Something with bow and rifle. Used in AttackRifle, AttackBow etc. "shoot now"?
$CAH		 UnitCombat_C
$Cxx		 UnitCombat_C, x is {P or S}
$CSD	SoundEntries.dbc	 Emote sounds?
$CVS	SoundEntriesAdvanced.dbc	 Sound
$DSE		
$DSL	SoundEntries.dbc	 Sound with something special. Use another one if you always want to have it playing..
$DSO	SoundEntries.dbc	 Sound
$DTH		 UnitCombat_C, death, this plays death sounds and more.
$EMV		 MapLoad.cpp
$ESD		 Plays some emote sound.
$EWT		 MapLoad.cpp
$FDx		 x is {1 to 5}. Calls some function in the Object VMT. Also plays some sound.
$FDx		 x is {6 to 9}. Calls some function in the Object VMT.
$FDX		 Should do nothing. But is existant.
$FSD		 Plays some sound.
$GCx		 Play gameobject custom sound referenced in GameObjectDisplayInfo.dbc. x can be from {0 to 3}: {Custom0, Custom1, Custom2, Custom3}
$GOx		 Play gameobject sound referenced in GameObjectDisplayInfo.dbc. x can be from {0 to 5}: {Stand, Open, Loop, Close, Destroy, Opened}
$HIT		 Get hit?
$KVS		 MapLoad.cpp
$SCD		 Plays some sound.
$SHK	SpellEffectCameraShakes.dbc	 Add a camera shake
$SHx		 x is {L or R}, fired on Sheath and SheathHip. "Left/right shoulder" was in the old list.
$SMD		 Plays some sound.
$SMG		 Plays some sound.
$SND	SoundEntries.dbc	 Sound
$TRD		 Does something with a spell, a sound and a spellvisual.
$VGx		 UnitVehicle_C, x is {0 to 8}
$VTx		 UnitVehicle_C, x is {0 to 8}
$WxG		 x is {W or N}. Calls some function in the Object VMT.
-------	 ----------------------------------	 - Old documentation (?) ----------------------------------------------
$CSx		 x is {L or R} ("Left/right hand") (?)
$CFM		
$CHD		 ("Head") (?)
$CCH		 ("Bust") (?)
$TRD		 ("Crotch") (?)
$CCH		 ("Bust") (?)
$BWR		 ("Right hand") (?)
$CAH		
$CST
*/



/*
 * This block specifies a bunch of locations on the body - hands, shoulders, head, back, 
 * knees etc. It is used to put items on a character. This seems very likely as this block 
 * also contains positions for sheathed weapons, a shield, etc.
 */
struct ModelAttachmentDef {
	int32_t id; // Just an id. Is referenced in the enum POSITION_SLOTS.
	int32_t bone; // Somewhere it has to be attached.
	Vec3D pos; // Relative to that bone of course.
	AnimationBlock unk; // Its an integer in the data. It has been 1 on all models I saw. Whatever.
		
};

#pragma pack(pop)
#endif
