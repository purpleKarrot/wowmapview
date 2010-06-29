#ifndef DATABASE_H
#define DATABASE_H

// Combined the previous 5 various "db" files into one.
// trying to cut down on excess files.
// Also instead of declaring the db objects inside various classes
// may aswell declare them as globals since pretty much most the
// different objects need to access them at one point or another.

#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// WX
#include <wx/string.h>

// STL
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <set>


// OUR HEADERS
#include "dbcfile.h"
#include "enums.h"
#include "util.h"

#include "CharStartOutfit.hpp"
#include "CreatureModelData.hpp"
#include "ItemDisplayInfo.hpp"

// 
class AnimDB;
class CharClassesDB;
class CharFacialHairDB;
class CharHairGeosetsDB;
class CharRacesDB;
class CharSectionsDB;
class CreatureModelDB;
class CreatureSkinDB;
class HelmGeosetDB;
class ItemDatabase;
class ItemDisplayDB;
class ItemSetDB;
class ItemSubClassDB;
class ItemVisualDB;
class ItemVisualEffectDB;
class NPCDatabase;
class NPCDB;
class StartOutfitDB;
class CreatureTypeDB;
class LightSkyBoxDB;
class SpellItemEnchantmentDB;
class ItemVisualsDB;
class ItemDB;

// dbs
extern ItemDatabase	items;
extern AnimDB animdb;
extern HelmGeosetDB	helmetdb;
extern ItemVisualEffectDB effectdb;
extern ItemDisplayDB itemdisplaydb;
extern CreatureModelDB modeldb;
extern NPCDatabase npcs;
extern ItemSetDB setsdb;
extern CreatureSkinDB skindb;
extern CharStartOutfit startdb;
extern ItemSubClassDB subclassdb;
extern ItemVisualDB visualdb;
extern CharHairGeosetsDB hairdb;
extern CharSectionsDB chardb;
extern CharClassesDB classdb;
extern CharFacialHairDB	facialhairdb;
extern CharRacesDB racedb;
extern NPCDB npcdb;
extern CreatureTypeDB npctypedb;
extern LightSkyBoxDB skyboxdb;
extern SpellItemEnchantmentDB spellitemenchantmentdb;
extern ItemVisualsDB itemvisualsdb;
extern ItemDB itemdb;

class ItemDB: public DBCFile
{
	std::map<int, int> itemLookup, itemDisplayLookup;
	bool inited;
public:
	ItemDB(): DBCFile("DBFilesClient\\Item.dbc"), inited(false) {}
	~ItemDB() {}

	static const size_t MaxItem = 100000;

	// Fields
	static const size_t ID = 0;	// unit
	static const size_t Itemclass = 1;	// unit
	static const size_t Subclass = 2;	// unit
	// static const size_t materialid = 4;	// uint
	static const size_t ItemDisplayInfo = 5;	// unit
	static const size_t InventorySlot = 6;	// unit
	static const size_t Sheath = 7;	// unit

	Record getByID(unsigned int id);
	Record getByDisplayId(unsigned int id);
};

class SpellItemEnchantmentDB: public DBCFile
{
public:
	SpellItemEnchantmentDB(): DBCFile("DBFilesClient\\SpellItemEnchantment.dbc") {}
	~SpellItemEnchantmentDB() {}

	// Fields
	static const size_t Name = 14;		// string, localization
	static const size_t VisualID = 31;	// unit

	static const size_t VisualIDV400 = 15;
};

class ItemVisualsDB: public DBCFile
{
public:
	ItemVisualsDB(): DBCFile("DBFilesClient\\ItemVisuals.dbc") {}
	~ItemVisualsDB() {}

	// Fields
	static const size_t VisualID = 0;	// unit
};

class LightSkyBoxDB: public DBCFile
{
public:
	LightSkyBoxDB(): DBCFile("DBFilesClient\\LightSkybox.dbc") {}
	~LightSkyBoxDB() {}

	// Fields
	// static const size_t ID;
	static const size_t Name = 1;		// string
	// static const size_t Flags;
};

class AnimDB: public DBCFile
{
public:
	AnimDB(): DBCFile("DBFilesClient\\AnimationData.dbc") {}
	~AnimDB() {}

	/// Fields
	static const size_t AnimID = 0;		// uint
	static const size_t Name = 1;		// string
	// static const size_t WeaponState = 2;	// int, 32 = pull weapons out during animation. 16 and 4 weapons are put back.
	// static const size_t Flags = 3;
	// static const size_t Unkonwn = 4;
	// static const size_t Preceeding; // The animation, preceeding this one.
	// static const size_t RealId; // Same as ID for normal animations. (WotLK)
	// static const size_t Group; // 0 for normal, 3 for fly. (WotLK)

	Record getByAnimID(unsigned int id);
};

// ============

class CharHairGeosetsDB: public DBCFile
{
public:
	CharHairGeosetsDB(): DBCFile("DBFilesClient\\CharHairGeosets.dbc") {}
	~CharHairGeosetsDB() {}

	/// Fields
	static const size_t CharHairGeosetID = 0;	// uint
	static const size_t Race = 1;				// uint
	static const size_t Gender = 2;				// uint
	static const size_t Section = 3;			// uint, ID unique between race, and gender.
	static const size_t Geoset = 4;				// uint, Defines hairstyle, each number should be unique for that race / gender combo.
	//static const size_t Bald = 5;				// uint, If this hairstyle bald or not .

	Record getByParams(unsigned int race, unsigned int gender, unsigned int section);
	int getGeosetsFor(unsigned int race, unsigned int gender);
};

class CharSectionsDB: public DBCFile
{
public:
	CharSectionsDB(): DBCFile("DBFilesClient\\CharSections.dbc") {}
	~CharSectionsDB() {}

	/// Fields
	static const size_t SectonID = 0;	// uint
	static const size_t Race = 1;		// uint
	static const size_t Gender = 2;		// uint
	static const size_t Type = 3;		// uint
	#ifndef WotLK
	static const size_t Section = 4;	// uint
	static const size_t Color = 5;		// uint
	static const size_t Tex1 = 6;		// string
	static const size_t Tex2 = 7;		// string
	static const size_t Tex3 = 8;		// string
	static const size_t IsNPC = 9;		// uint | 1 for npc
	#else
	static const size_t Tex1 = 4;		// string
	static const size_t Tex2 = 5;		// string
	static const size_t Tex3 = 6;		// string
	//static const size_t Flags = 7;		// uint		(As specified in http://www.madx.dk/wowdev/wiki/index.php?title=CharSections.dbc)
	static const size_t Section = 8;	// uint
	static const size_t Color = 9;		// uint
	#endif

	/// Types
	static const size_t SkinType = 0;
	static const size_t FaceType = 1;
	static const size_t FacialHairType = 2;
	static const size_t HairType = 3;
	static const size_t UnderwearType = 4;

	Record getByParams(unsigned int race, unsigned int gender, unsigned int type, unsigned int section, unsigned int color, unsigned int npc);
	int getColorsFor(unsigned int race, unsigned int gender, unsigned int type, unsigned int section, unsigned int npc);
	int getSectionsFor(unsigned int race, unsigned int gender, unsigned int type, unsigned int color, unsigned int npc);
};


class CharRacesDB: public DBCFile
{
public:
	CharRacesDB(): DBCFile("DBFilesClient\\ChrRaces.dbc") 	{}

	~CharRacesDB() {}

	// Correct for WotLK v3.3.x
	static const size_t RaceID = 0;			// uint
	static const size_t maleModeID = 4;		// unit
	static const size_t femaleModeID = 5;	// unit
	static const size_t ShortName = 6;		// string, Name, represented by only 2 chars
	static const size_t Name = 11;			// string, Model name, 10048 to 11
	static const size_t FullName = 14;		// string, localization, Name with spaces & such.
	static const size_t GeoType1 = 65;		// string, Facial Feature Type for Men
	//static const size_t GeoType2 = 66;		// string, Facial Feature Type for Women
	//static const size_t GeoType3 = 67;		// string, Changes Hair to this value. (IE: Hair for everyone, but Horns for Tauren)

	// -= GeoType Values =-
	// Normal: Default Geotype. Facial Hair for Men, no known value for Women.
	// Peircings: Women Only. Earrings, noserings, eyebrow peircings, etc.
	// Earrings: Gnome Women Only. Just Earrings.
	// Features: Forsaken(Scourge) Only. Indicates a model change for the face.
	// Horns: Taurens & Draenei Females Only. Horns.
	// Tusks: Trolls Only. Tusks.
	// Markings: Night Elf Women Only. Facial Markings.
	// None: No Changable Geotypes. Currently only found on Goblin Women.


	static const size_t NameV310 = 12;		// string, model name, 10048 to 11
	
	static const size_t GeoType1V400 = 17;
	//static const size_t GeoType2V400 = 18;
	//static const size_t GeoType3V400 = 19;

	Record getByName(wxString name);
//	Record getById(unsigned int id);
};


class CharFacialHairDB: public DBCFile
{
public:
	CharFacialHairDB(): DBCFile("DBFilesClient\\CharacterFacialHairStyles.dbc") {}
	~CharFacialHairDB() {}

	/// Fields
	static const size_t Race = 0;				// uint
	static const size_t Gender = 1;				// uint
	static const size_t Style = 2;				// uint
	#ifndef WotLK
	static const size_t Geoset100 = 6;			// uint
	static const size_t Geoset300 = 7;			// uint
	static const size_t Geoset200 = 8;			// uint
	#else
	static const size_t Geoset100 = 3;			// uint
	static const size_t Geoset300 = 4;			// uint
	static const size_t Geoset200 = 5;			// uint
	#endif
	// uint
	// uint
	
	Record getByParams(unsigned int race, unsigned int gender, unsigned int style);
	int getStylesFor(unsigned int race, unsigned int gender);

	static const size_t RaceV400 = 1;				// uint
	static const size_t GenderV400 = 2;				// uint
	static const size_t StyleV400 = 3;				// uint
	static const size_t Geoset100V400 = 4;			// uint
	static const size_t Geoset300V400 = 5;			// uint
	static const size_t Geoset200V400 = 6;			// uint
};


class CharClassesDB: public DBCFile
{
public:
	CharClassesDB(): DBCFile("DBFilesClient\\ChrClasses.dbc") {}
	~CharClassesDB() {}

	/// Fields
	static const size_t ClassID = 0;	// uint
	static const size_t Name = 4;		// string, localization - english name
	//static const size_t RawName = 14;	// string

//	Record getById(unsigned int id);
	
	static const size_t NameV400 = 3;	// string, localization - english name
};


class HelmGeosetDB: public DBCFile
{
public:
	HelmGeosetDB(): DBCFile("DBFilesClient\\HelmetGeosetVisData.dbc") {}
	~HelmGeosetDB() {}

	/// Fields
	static const size_t TypeID = 0;		// uint
	static const size_t Hair = 1;		// int Hair, 0 = show, anything else = don't show? eg: a value of 1020 won't hide night elf ears, but 999999 or -1 will.
	static const size_t Facial1Flags = 2;		// int Beard or Tusks
	static const size_t Facial2Flags = 3;		// int Earring
	static const size_t Facial3Flags = 4;		// int, See ChrRaces, column 24 to 26 for information on what is what.
	static const size_t EarsFlags = 5;		// int Ears
	//static const size_t Field6 = 6;		// int
	//static const size_t Field7 = 7;		// int

	Record getById(unsigned int id);
};

// ==============================================

// -----------------------------------
// Item Stuff
// -----------------------------------

class ItemDatabase;

extern const char* ItemTypeNames[NUM_ITEM_TYPES];

struct ItemDisplayDB:   ItemDisplayInfo
{
	static const size_t ItemDisplayID = 0;	// uint
	static const size_t Model = 1;			// string
	static const size_t Model2 = 2;			// string
	static const size_t Skin = 3;			// string
	static const size_t Skin2 = 4;			// string
	static const size_t Icon = 5;			// string
	static const size_t Texture = 6;			// string
	static const size_t GloveGeosetFlags = 7;		// uint
	static const size_t BracerGeosetFlags = 8;		// uint
	static const size_t RobeGeosetFlags = 9;		// uint
	static const size_t BootsGeosetFlags = 10;		// uint
	static const size_t Unknown = 11;		// uint
	static const size_t ItemGroupSounds = 12;			// uint
	static const size_t GeosetVisID1 = 13;	// uint
	static const size_t GeosetVisID2 = 14;	// uint
	static const size_t TexArmUpper = 15;	// string
	static const size_t TexArmLower = 16;	// string
	static const size_t TexHands = 17;		// string
	static const size_t TexChestUpper = 18;	// string
	static const size_t TexChestLower = 19;	// string
	static const size_t TexLegUpper = 20;	// string
	static const size_t TexLegLower = 21;	// string
	static const size_t TexFeet = 22;		// string
	static const size_t Visuals = 23;		// uint

	bool hasId(unsigned int id);
};


class ItemVisualDB: public DBCFile
{
public:
	ItemVisualDB(): DBCFile("DBFilesClient\\ItemVisuals.dbc") {}
	~ItemVisualDB() {}

	/// Fields
	static const size_t VisualID = 0;	// uint
	static const size_t Effect1 = 1;	// uint
	//static const size_t Effect2 = 2;	// uint
	//static const size_t Effect3 = 3;	// uint
	//static const size_t Effect4 = 4;	// uint
	//static const size_t Effect5 = 5;	// uint

//	Record getById(unsigned int id);
};

class ItemVisualEffectDB: public DBCFile
{
public:
	ItemVisualEffectDB(): DBCFile("DBFilesClient\\ItemVisualEffects.dbc") {}
	~ItemVisualEffectDB() {}

	/// Fields
	static const size_t EffectID = 0;	// uint
	static const size_t Model = 1;		// string

//	Record getById(unsigned int id);
};


class ItemSetDB: public DBCFile
{
	std::set<int> avail;

public:
	ItemSetDB(): DBCFile("DBFilesClient\\ItemSet.dbc") {}
	~ItemSetDB() {}

	static const size_t NumItems = 10;

	/// Fields
	static const size_t SetID = 0;	// uint
	static const size_t Name = 1;	// string, Localization
	static const size_t ItemIDBase = 18; // 10 * uint

//	Record getById(unsigned int id);
	void cleanup(ItemDatabase &l_itemdb);
	bool available(unsigned int id);
	
	static const size_t ItemIDBaseV400 = 2; // 10 * uint
};

struct ItemRecord {
	wxString name;
	int id, itemclass, subclass, type, model, sheath, quality;
	bool discovery;

	ItemRecord(const char* line);
	ItemRecord():id(0), type(0), itemclass(-1), subclass(-1), model(0), sheath(0), quality(0), discovery(false)
	{}
	ItemRecord(wxString name, int type): id(0), name(name), type(type), itemclass(-1), subclass(-1), model(0), sheath(0), quality(0), discovery(false)
	{}
	/*
	ItemRecord(const ItemRecord &r): id(r.id), name(r.name), itemclass(r.itemclass), subclass(r.subclass), type(r.type), model(r.model), sheath(r.sheath), quality(r.quality)
	{}
	*/

	void getLine(const char* line);

	const bool operator< (const ItemRecord &r) const
	{
		if (type == r.type) 
			return name < r.name;
		else 
			return type < r.type;
	}
};

class ItemDatabase {
public:
//	ItemDatabase(const char* filename);
	ItemDatabase();

	std::vector<ItemRecord> items;
	std::map<int, int> itemLookup;

	void cleanup(ItemDisplayDB &l_itemdb);	
	void open(wxString filename);

	const ItemRecord& getByID(int id);
	const ItemRecord& getByPos(int id);
	int getItemIDByModel(int id);
	bool avaiable(int id);
	int getItemNum(int displayid);
	wxString addDiscoveryId(int id, wxString name);
	wxString addDiscoveryDisplayId(int id, wxString name, int type);
	void cleanupDiscovery();
};

/*
class ItemClassDB: public DBCFile
{
public:
	ItemClassDB(): DBCFile("DBFilesClient\\ItemClass.dbc") {}
	~ItemClassDB() {}

	/// Fields
	static const size_t ClassID = 0;	// uint
	static const size_t Name = 3;		// string

	//Record getById(unsigned int id);
};
*/

class ItemSubClassDB: public DBCFile
{
public:
	ItemSubClassDB(): DBCFile("DBFilesClient\\ItemSubClass.dbc") {}
	~ItemSubClassDB() {}

	/// Fields
	static const size_t ClassID = 0;	// int
	static const size_t SubClassID = 1;	// int
	//static const size_t Flags = 4;		// uint
	// ...
	static const size_t Hands = 9;		// int
	static const size_t Name = 10;		// string

//	Record getById(int id, int subid);

	static const size_t HandsV400 = 10;		// int
	static const size_t NameV400 = 11;		// string
};

// ============/////////////////=================/////////////////


// ------------------------------
// NPC Stuff
// -------------------------------
struct NPCRecord 
{
	wxString name;
	int id, model, type;
	bool discovery;

	NPCRecord(const char* line);
	NPCRecord(): id(0), model(0) {}
	NPCRecord(const NPCRecord &r): id(r.id), name(r.name), model(r.model), type(r.type) {}

	const bool operator< (const NPCRecord &r) const
	{ 
		return name < r.name;
	}
};

class NPCDatabase 
{
public:
	NPCDatabase(const char* filename);
	NPCDatabase() { }

	std::vector<NPCRecord> npcs;
	std::map<int, int> npcLookup;

	void open(wxString filename);

	const NPCRecord& get(int id);
	const NPCRecord& getByID(int id);
	bool avaiable(int id);
	wxString addDiscoveryId(int id, wxString name);
};

// =========================================

class SpellEffectsDB: public DBCFile
{
public:
	SpellEffectsDB(): DBCFile("DBFilesClient\\SpellVisualEffectName.dbc") {}
	~SpellEffectsDB() {}

	/// Fields
	static const size_t ID = 0;				// uint
	static const size_t EffectName = 1;		// string
	static const size_t ModelName = 2;		// string
	static const size_t SpellType = 3;		// uint
	//static const size_t UnknownValue2 = 4;	// uint

	Record getById(unsigned int id);
	Record getByName(wxString name);
};


// ===============================================

struct CreatureModelDB: CreatureModelData
{
	Record getByFilename(wxString fn);
	Record getByID(unsigned int id);
};

class CreatureSkinDB: public DBCFile
{
public:
	CreatureSkinDB(): DBCFile("DBFilesClient\\CreatureDisplayInfo.dbc") {}
	~CreatureSkinDB() {}

	/// Fields
	static const size_t SkinID = 0;			// uint
	static const size_t ModelID = 1;		// uint
											// uint SoundID
	static const size_t NPCID = 3;			// uint CreatureDisplayInfoExtraID
	//static const size_t Scale = 4;			// float
	//static const size_t Opacity = 5;			// uint, 0-255, 255 is solid
	static const size_t Skin = 6;			// string
	//static const size_t Skin2 = 7;			// string
	//static const size_t Skin3 = 8;			// string
	// uint IconID
	// unit sizeClass
	// unit bloodID
	// unit NPCSoundID
	// unit particleColorID
	// unit creatureGeosetData
	// unit objectEffectPackageID

	Record getByModelID(unsigned int id);
	Record getBySkinID(unsigned int id);
};

class CreatureTypeDB: public DBCFile
{
public:
	CreatureTypeDB(): DBCFile("DBFilesClient\\CreatureType.dbc") {}
	~CreatureTypeDB() {}
	
	// Fields
	static const size_t ID = 0;			// uint
	static const size_t Name = 1;		// string

	Record getByID(unsigned int id);
};

class NPCDB: public DBCFile
{
public:
	NPCDB(): DBCFile("DBFilesClient\\CreatureDisplayInfoExtra.dbc") {}
	~NPCDB() {}

	/// Fields
	static const size_t NPCID = 0;			// uint
	static const size_t RaceID = 1;			// uint
	static const size_t Gender = 2;			// bool
	static const size_t SkinColor = 3;		// uint
	static const size_t Face = 4;			// uint
	static const size_t HairStyle = 5;		// uint
	static const size_t HairColor = 6;		// uint
	static const size_t FacialHair = 7;		// uint
	static const size_t HelmID = 8;			// uint, Slot1
	static const size_t ShoulderID = 9;		// uint, Slot3
	static const size_t ShirtID = 10;		// uint, Slot4
	static const size_t ChestID = 11;		// uint, Slot5
	static const size_t BeltID = 12;		// uint, Slot6
	static const size_t PantsID = 13;		// uint, Slot7
	static const size_t BootsID = 14;		// uint, Slot8
	static const size_t BracersID = 15;		// uint, Slot9
	static const size_t GlovesID = 16;		// uint, Slot10
	static const size_t TabardID = 17;		// uint, Slot19
	static const size_t CapeID = 18;		// uint, Slot16
	//static const size_t CanEquip = 19;		// bool
	static const size_t Filename = 20;		// string. an index offset to the filename.

	Record getByFilename(wxString fn);
	Record getByNPCID(unsigned int id);

};


#endif

