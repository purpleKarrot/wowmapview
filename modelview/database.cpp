
#include "database.h"
#include "mpq.h"

ItemDatabase		items;
// dbs
NPCDatabase			npcs;

// --
HelmGeosetDB		helmetdb;
ItemVisualEffectDB	effectdb;
ItemDisplayDB		itemdisplaydb;
StartOutfitDB		startdb;
ItemSubClassDB		subclassdb;
ItemVisualDB		visualdb;
ItemSetDB			setsdb;
ItemDB				itemdb;
// --
AnimDB				animdb;
CharHairGeosetsDB	hairdb;
CharSectionsDB		chardb;
CharClassesDB		classdb;
CharFacialHairDB	facialhairdb;
CharRacesDB			racedb;
//--
CreatureModelDB		modeldb;
CreatureSkinDB		skindb;
CreatureTypeDB		npctypedb;
NPCDB				npcdb;
LightSkyBoxDB			skyboxdb;
SpellItemEnchantmentDB	spellitemenchantmentdb;
ItemVisualsDB			itemvisualsdb;

// ANIMDB.H
AnimDB::Record AnimDB::getByAnimID(unsigned int id)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->getUInt(AnimID) == id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}
// --


// --
// CHARDB.H
// HairGeosets

CharHairGeosetsDB::Record CharHairGeosetsDB::getByParams(unsigned int race, unsigned int gender, unsigned int section)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender && i->getUInt(Section)==section)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}

int CharHairGeosetsDB::getGeosetsFor(unsigned int race, unsigned int gender)
{
	int n = 0;
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender) {
			n++;
		}
	}
    return n;
}

// Sections

int CharSectionsDB::getColorsFor(unsigned int race, unsigned int gender, unsigned int type, unsigned int section, unsigned int npc)
{
	int n = 0;
#if 1 // for worgen female
	if (gameVersion >= 40000 && race == 22 && gender == 1) { // worgen female
		wxString fn;
		switch(type) { // 0: base, 1: face, 2: facial, 3: hair, 4: underwear
			case 0: // Character\Worgen\Female\WorgenFemaleSkin00_12.blp
				for(int i=0; i<20; i++) {
					fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleSkin%02d_%02d.blp"), section, i);
					if (MPQFile::getSize(fn.fn_str()) > 0)
						n++;
					else
						break;
				}
				break;
			case 1: // Character\Worgen\Female\WorgenFemaleFaceUpper27_09.blp
				for(int i=0; i<30; i++) {
					fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleFaceUpper%02d_%02d.blp"), section, i);
					if (MPQFile::getSize(fn.fn_str()) > 0)
						n++;
					else
						break;
				}
				break;
			case 3: // Character\Worgen\Hair00_00.blp
				for(int i=0; i<20; i++) {
					fn.Printf(_T("Character\\Worgen\\Hair00_%02d.blp"), i);
					if (MPQFile::getSize(fn.fn_str()) > 0)
						n++;
					else
						break;
				}
				break;
			case 4: // Character\Worgen\Male\WorgenMaleNakedPelvisSkin00_00.blp
				for(int i=0; i<20; i++) {
					fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleNakedPelvisSkin%02d_%02d.blp"), section, i);
					if (MPQFile::getSize(fn.fn_str()) > 0)
						n++;
					else
						break;
				}
				break;
		}
	}
	// if already in dbc
	if (n > 0)
		return n;
#endif // for worgen female
	for(Iterator i=begin(); i!=end(); ++i)
	{
		// don't allow NPC skins ;(
		#ifndef WotLK
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender && i->getUInt(Type)==type && i->getUInt(Section)==section && i->getUInt(IsNPC)==npc) {
			n++;
		}
		#else
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender && i->getUInt(Type)==type && i->getUInt(Section)==section) {
			n++;
		}
		#endif
	}

    return n;
}

int CharSectionsDB::getSectionsFor(unsigned int race, unsigned int gender, unsigned int type, unsigned int color, unsigned int npc)
{
	int n = 0;
#if 1 // for worgen female
	if (gameVersion >= 40000 && race == 22 && gender == 1) { // worgen female
		wxString fn;
		switch(type) { // 0: base, 1: face, 2: facial, 3: hair, 4: underwear
			case 0: // Character\Worgen\Female\WorgenFemaleSkin00_12.blp
				for(int i=0; i<20; i++) {
					fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleSkin%02d_%02d.blp"), i, color);
					if (MPQFile::getSize(fn.fn_str()) > 0)
						n++;
					else
						break;
				}
				break;
			case 1: // Character\Worgen\Female\WorgenFemaleFaceUpper27_09.blp
				for(int i=0; i<30; i++) {
					fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleFaceUpper%02d_%02d.blp"), i, color);
					if (MPQFile::getSize(fn.fn_str()) > 0)
						n++;
					else
						break;
				}
				break;
			case 3: // Character\Worgen\Hair00_00.blp
				for(int i=0; i<20; i++) {
					fn.Printf(_T("Character\\Worgen\\Hair%02d_%02d.blp"), i, color);
					if (MPQFile::getSize(fn.fn_str()) > 0)
						n++;
					else
						break;
				}
				break;
			case 4: // Character\Worgen\Male\WorgenMaleNakedPelvisSkin00_00.blp
				for(int i=0; i<20; i++) {
					fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleNakedPelvisSkin%02d_%02d.blp"), i, color);
					if (MPQFile::getSize(fn.fn_str()) > 0)
						n++;
					else
						break;
				}
				break;
		}
	}
	// if already in dbc
	if (n > 0)
		return n;
#endif // for worgen female
	for(Iterator i=begin(); i!=end(); ++i)
	{
		#ifndef WotLK
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender && i->getUInt(Type)==type && i->getUInt(Color)==color && i->getUInt(IsNPC)==npc) {
			n++;
		}
		#else
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender && i->getUInt(Type)==type && i->getUInt(Color)==color) {
			n++;
		}
		#endif
	}
    return n;
}

CharSectionsDB::Record CharSectionsDB::getByParams(unsigned int race, unsigned int gender, unsigned int type, unsigned int section, unsigned int color, unsigned int npc)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		#ifndef WotLK
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender && i->getUInt(Type)==type && i->getUInt(Section)==section && i->getUInt(Color)==color && i->getUInt(IsNPC)==npc)
			return (*i);
		#else
		if (i->getUInt(Race)==race && i->getUInt(Gender)==gender && i->getUInt(Type)==type && i->getUInt(Section)==section && i->getUInt(Color)==color)
			return (*i);
		#endif
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d race:%d, gender:%d, type:%d, section:%d, color:%d"), __FILE__, __FUNCTION__, __LINE__, race, gender, type, section, color);
	throw NotFound();
}

// Races
CharRacesDB::Record CharRacesDB::getByName(wxString name)
{
	for(Iterator i=begin(); i!=end(); ++i) {
		wxString r;
		if (gameVersion == 30100)
			r = i->getString(NameV310);
		else
			r = i->getString(Name);
		if (name.IsSameAs(r, false) == true)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}

CharRacesDB::Record CharRacesDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(RaceID)==id) return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}


// FacialHair

CharFacialHairDB::Record CharFacialHairDB::getByParams(unsigned int race, unsigned int gender, unsigned int style)
{
	if (gameVersion == 40000) {
		for(Iterator i=begin(); i!=end(); ++i)
		{
			if (i->getUInt(RaceV400)==race && i->getUInt(GenderV400)==gender && i->getUInt(StyleV400)==style)
				return (*i);
		}
	} else {
		for(Iterator i=begin(); i!=end(); ++i)
		{
			if (i->getUInt(Race)==race && i->getUInt(Gender)==gender && i->getUInt(Style)==style)
				return (*i);
		}
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}

int CharFacialHairDB::getStylesFor(unsigned int race, unsigned int gender)
{
	int n = 0;
	if (gameVersion == 40000) {
		for(Iterator i=begin(); i!=end(); ++i)
		{
			if (i->getUInt(RaceV400)==race && i->getUInt(GenderV400)==gender) {
				n++;
			}
		}
	} else {
		for(Iterator i=begin(); i!=end(); ++i)
		{
			if (i->getUInt(Race)==race && i->getUInt(Gender)==gender) {
				n++;
			}
		}
	}
	return n;
}


// Classes

CharClassesDB::Record CharClassesDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(ClassID)==id) return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}


// Head and Helmet display info
HelmGeosetDB::Record HelmGeosetDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(TypeID)==id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}
// --


// --
// CREATUREDB.H
//
CreatureModelDB::Record CreatureModelDB::getByFilename(wxString fn)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		wxString str(i->getString(Filename));
		if(str.IsSameAs(fn, false) == true)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}

CreatureModelDB::Record CreatureModelDB::getByID(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(ModelID)==id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}

CreatureSkinDB::Record CreatureSkinDB::getByModelID(unsigned int id)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->getUInt(ModelID) == id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}

CreatureSkinDB::Record CreatureSkinDB::getBySkinID(unsigned int id)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->getUInt(SkinID) == id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}

CreatureTypeDB::Record CreatureTypeDB::getByID(unsigned int id)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->getUInt(ID) == id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}

NPCDB::Record NPCDB::getByFilename(wxString fn)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->getString(Filename) == fn) {
			//std::cout << i->getString(Filename).c_str() << "\n";
			return (*i);
		}
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}

NPCDB::Record NPCDB::getByNPCID(unsigned int id)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->getUInt(NPCID) == id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}
// --



// --
// ITEMDB.H
//
// --------------------------------
// Item Database Stuff
// --------------------------------
const char* ItemTypeNames[NUM_ITEM_TYPES] = {
	"All",
	"Helmets",
	"Neck",
	"Shoulder armor",
	"Shirts",
	"Chest armor",
	"Belts",
	"Pants",
	"Boots",
	"Bracers",
	"Gloves",
	"Rings",
	"Accessories",
	"Daggers",
	"Shields",
	"Bows",
	"Capes",
	"Two-handed weapons",
	"Quivers",
	"Tabards",
	"Robes",
	"One-handed weapons",
	"Offhand weapons",
	"Holdable",
	"Ammo",
	"Thrown",
	"Guns and wands",
	"Unknown",
	"Relic"
};

// ItemDisplayInfo

ItemDisplayDB::Record ItemDisplayDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(ItemDisplayID)==id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}


ItemVisualDB::Record ItemVisualDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(VisualID)==id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}

ItemVisualEffectDB::Record ItemVisualEffectDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(EffectID)==id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}

ItemSetDB::Record ItemSetDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(SetID)==id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}

void ItemSetDB::cleanup(ItemDatabase &p_itemdb)
{
	for(Iterator i=begin(); i!=end(); ++i) {
		for (int j=0; j<NumItems; j++) {
			int id;
			if (gameVersion == 40000)
				id = i->getUInt(ItemIDBaseV400+j);
			else
				id = i->getUInt(ItemIDBase+j);
			if (id > 0) {
				const ItemRecord &r = p_itemdb.getById(id);
				if (r.type > 0) {
					avail.insert(i->getUInt(SetID));
					break;
				}
			}
		}
	}
}

bool ItemSetDB::available(unsigned int id)
{
	return (avail.find(id)!=avail.end());
}


StartOutfitDB::Record StartOutfitDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(StartOutfitID)==id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}



////////////////////

ItemDB::Record ItemDB::getById(unsigned int id)
{
	if (!inited) {
		int j=0;
		for(Iterator i=begin(); i!=end(); ++i)
		{
			itemLookup[i->getUInt(ID)] = j;
			itemDisplayLookup[i->getUInt(ItemDisplayInfo)] = j;
			j++;
		}
		inited = true;
	}

    if (itemLookup.find(id)!=itemLookup.end()) {
		int i = itemLookup[id];
		ItemDB::Record rec = itemdb.getRecord(i);
		return rec;
    }
	throw NotFound();

/*
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(ID)==id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d id:%d"), __FILE__, __FUNCTION__, __LINE__, id);
	throw NotFound();
*/
}

ItemDB::Record ItemDB::getByDisplayId(unsigned int id)
{
	if (!inited) {
		int j=0;
		for(Iterator i=begin(); i!=end(); ++i)
		{
			itemLookup[i->getUInt(ID)] = j;
			itemDisplayLookup[i->getUInt(ItemDisplayInfo)] = j;
			j++;
		}
		inited = true;
	}

    if (itemDisplayLookup.find(id)!=itemDisplayLookup.end()) {
		int i = itemDisplayLookup[id];
		ItemDB::Record rec = itemdb.getRecord(i);
		return rec;
    }
	throw NotFound();

/*
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(ItemDisplayInfo)==id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d id:%d"), __FILE__, __FUNCTION__, __LINE__, id);
*/
	throw NotFound();
}

void ItemRecord::getLine(const char *line)
{
	sscanf(line, "%u,%u,%u,%u,%u,%u,%u", &id, &model, &itemclass, &subclass, &type, &sheath, &quality);
	for (size_t i=strlen(line)-2; i>1; i--) {
		if (line[i]==',') {
			name = wxString(line + i + 1, wxConvUTF8);
			break;
		}
	}
	discovery = true;
}

ItemRecord::ItemRecord(const char* line)
{
	sscanf(line, "%u,%u", &id, &quality);
	try {
		ItemDB::Record r = itemdb.getById(id);
		model = r.getInt(ItemDB::ItemDisplayInfo);
		itemclass = r.getInt(ItemDB::Itemclass);
		subclass = r.getInt(ItemDB::Subclass);
		type = r.getInt(ItemDB::InventorySlot);
		switch(r.getInt(ItemDB::Sheath)) {
			case 1: sheath = PS_RIGHT_BACK_SHEATH; break;
			case 2: sheath = PS_LEFT_BACK; break;
			case 3: sheath = PS_LEFT_HIP_SHEATH; break;
			case 4: sheath = PS_MIDDLE_BACK_SHEATH; break;
			default: sheath = 0;
		}
		discovery = false;
		for (size_t i=strlen(line)-2; i>1; i--) {
			if (line[i]==',') {
				//name = (line + i + 1);
				name.Printf(_T("%s [%d] [%d]"), wxString(line+i+1, wxConvUTF8).c_str(), id, model);
				break;
			}
		}
	} catch (ItemDB::NotFound) {}

}

// Alfred. prevent null items bug.
ItemDatabase::ItemDatabase()
{
	ItemRecord all(_("---- None ----"), IT_ALL);
	items.push_back(all);
}

/*
ItemDatabase::ItemDatabase(const char* filename)
{
	ItemRecord all(_("---- None ----"), IT_ALL);
	items.push_back(all);
	std::ifstream fin(filename);
	char line[512];
	while (fin.getline(line,512)) {
		ItemRecord rec(line);
		if (rec.type > 0) {
			items.push_back(rec);
		}
	}
	fin.close();
	sort(items.begin(), items.end());
}*/

void ItemDatabase::open(wxString filename)
{
	std::ifstream fin(filename.mb_str());
	char line[512];
	if (fin.is_open()) {
		while (fin.getline(line,512)) {
			ItemRecord rec(line);
			if (rec.type > 0) {
				items.push_back(rec);
			}
		}
		fin.close();
	}

	std::ifstream fin2("discoveryitems.csv");
	if (fin2.is_open()) {
		while (fin2.getline(line,512)) {
			ItemRecord rec;
			rec.getLine(line);
			if (rec.type > 0) {
				items.push_back(rec);
			}
		}
		fin2.close();
	}

	sort(items.begin(), items.end());
}

void ItemDatabase::cleanup(ItemDisplayDB &l_itemdisplaydb)
{
	std::set<unsigned int> itemset;
	for (ItemDisplayDB::Iterator it = l_itemdisplaydb.begin(); it != l_itemdisplaydb.end(); ++it) {
		itemset.insert(it->getUInt(ItemDisplayDB::ItemDisplayID));
	}
	for (unsigned int i=0; i<items.size(); ) {
		bool keepItem = (items[i].type==0) || (itemset.find(items[i].model)!=itemset.end());
		if (keepItem) {
			itemLookup[items[i].id] = i;
			i++;
		}
		else items.erase(items.begin() + i);
	}
}

void ItemDatabase::cleanupDiscovery()
{
	for (unsigned int i=0; i<items.size(); ) {
		if (items[i].discovery)
			items.erase(items.begin() + i);
		else
			i++;
	}
}

int ItemDatabase::getItemIDByModel(int id)
{
	if (id == 0)
		return 0;
	for (std::vector<ItemRecord>::iterator it = items.begin(); it != items.end(); ++it)
		if(it->model == id) return it->id;
    
	return 0;
}

const ItemRecord& ItemDatabase::getById(int id)
{
    if (itemLookup.find(id)!=itemLookup.end()) 
		return items[itemLookup[id]];
	else 
		return items[0];
}

const ItemRecord& ItemDatabase::getByPos(int id)
{
	return items[id];
}

bool ItemDatabase::avaiable(int id)
{
	return (itemLookup.find(id)!=itemLookup.end());
/*
	for (std::vector<ItemRecord>::iterator it = items.begin(); it != items.end(); ++it)
		if(it->id == id) return id;

	return 0;
*/
}

int ItemDatabase::getItemNum(int displayid)
{
	for (std::vector<ItemRecord>::iterator it = items.begin(); it != items.end(); ++it)
		if(it->model == displayid) return it->id;
    
	return 0;
}

wxString ItemDatabase::addDiscoveryId(int id, wxString name)
{
	wxString ret = wxEmptyString;

	try {
		ItemDB::Record r = itemdb.getById(id);
		ItemRecord rec;
		rec.id = id;
		rec.model = r.getInt(ItemDB::ItemDisplayInfo);
		rec.itemclass = r.getInt(ItemDB::Itemclass);
		rec.subclass = r.getInt(ItemDB::Subclass);
		rec.type = r.getInt(ItemDB::InventorySlot);
		switch(r.getInt(ItemDB::Sheath)) {
			case 1: rec.sheath = PS_RIGHT_BACK_SHEATH; break;
			case 2: rec.sheath = PS_LEFT_BACK; break;
			case 3: rec.sheath = PS_LEFT_HIP_SHEATH; break;
			case 4: rec.sheath = PS_MIDDLE_BACK_SHEATH; break;
			default: rec.sheath = 0;
		}
		rec.discovery = true;
		rec.name.Printf(_T("%s [%d] [%d]"), name.c_str(), rec.id, rec.model);
		if (rec.type > 0) {
			items.push_back(rec);
			itemLookup[rec.id] = (int)items.size()-1;
			//wxLogMessage(_T("Info: Not exist ItemID: %d, %s..."), id, rec.name.c_str());
			ret.Printf(_T("%d,%d,%d,%d,%d,%d,%d,%s"), rec.id, rec.model, rec.itemclass, rec.subclass,
				rec.type, rec.sheath, rec.quality, rec.name.c_str());
		}
	} catch (ItemDB::NotFound) {}
	return ret;
}

wxString ItemDatabase::addDiscoveryDisplayId(int id, wxString name, int type)
{
	wxString ret = wxEmptyString;

	ItemRecord rec;
	rec.id = id+ItemDB::MaxItem;
	rec.model = id;
	rec.itemclass = 4;
	rec.subclass = 0;
	rec.type = type;
	rec.sheath = 0;
	rec.discovery = true;
	rec.name.Printf(_T("%s [%d]"), name.c_str(), id);
	if (rec.type > 0) {
		items.push_back(rec);
		itemLookup[rec.id] = (int)items.size()-1;
		//wxLogMessage(_T("Info: Not exist ItemID: %d, %s..."), id, rec.name.c_str());
		ret.Printf(_T("%d,%d,%d,%d,%d,%d,%d,%s"), rec.id, rec.model, rec.itemclass, rec.subclass,
			rec.type, rec.sheath, rec.quality, rec.name.c_str());
	}
	return ret;
}

ItemSubClassDB::Record ItemSubClassDB::getById(int id, int subid)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getInt(ClassID)==id && i->getInt(SubClassID)==subid)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}
// ============================================================
// =============================================================

bool NPCDatabase::avaiable(int id)
{
	return (npcLookup.find(id)!=npcLookup.end());
/*
	for (std::vector<NPCRecord>::iterator it = npcs.begin(); it != npcs.end(); ++it)
		if(it->id == id) return it->id;
    
	return 0;
*/
}

wxString NPCDatabase::addDiscoveryId(int id, wxString name)
{
	wxString ret = wxEmptyString;

	NPCRecord rec;
	rec.id = id+100000;
	rec.model = id;
	rec.type = 7;
	rec.discovery = true;
	rec.name.Printf(_T("%s [%d]"), name.c_str(), rec.id);
	if (rec.type > 0) {
		npcs.push_back(rec);
		ret.Printf(_T("%d,%d,%d,%s"), rec.id, rec.model, rec.type, rec.name.c_str());
	}
	return ret;
}


NPCRecord::NPCRecord(const char* line)
{
	sscanf(line, "%u,%u,%u,", &id, &model, &type);
	discovery = false;
	for (size_t i=strlen(line)-2; i>1; i--) {
		if (line[i]==',') {
			//name = (line + i + 1);
			name.Printf(_T("%s [%d] [%d]"), wxString(line+i+1, wxConvUTF8).c_str(), id, model);
			break;
		}
	}
}

NPCDatabase::NPCDatabase(const char* filename)
{
	//ItemRecord all(_("---- None ----"), IT_ALL);
	//items.push_back(all);

	std::ifstream fin(filename);
	char line[512];
	while (fin.getline(line,512)) {
		NPCRecord rec(line);
		if (rec.model > 0) {
			npcs.push_back(rec);
		}
	}
	fin.close();
	sort(npcs.begin(), npcs.end());

	int j=0;
	for (std::vector<NPCRecord>::iterator it=npcs.begin();	it!=npcs.end(); ++it)
	{
		npcLookup[it->id] = j;
		j++;
	}
}

void NPCDatabase::open(wxString filename)
{
	std::ifstream fin(filename.mb_str());
	char line[512];
	if (fin.is_open()) {
		while (fin.getline(line,512)) {
			NPCRecord rec(line);
			if (rec.model > 0) {
				npcs.push_back(rec);
			}
		}
		fin.close();
		sort(npcs.begin(), npcs.end());
	}
}


const NPCRecord& NPCDatabase::get(int id)
{
	return npcs[id];
}

const NPCRecord& NPCDatabase::getByID(int id)
{
    if (npcLookup.find(id)!=npcLookup.end()) {
		return npcs[npcLookup[id]];
    }
	
	return npcs[0];
/*
	for (std::vector<NPCRecord>::iterator it=npcs.begin();  it!=npcs.end(); ++it) {
		if (it->id == id) {
			return (*it);
		}
	}

	return npcs[0];
*/
}

// --



// --
// SPELLDB.H
//

/*

SpellVisualeffects.dbc
column 1 = id, int
column 2 = spell name, string
column 3 = model name, string
column 4 = number between 0 and 11, int
column 5 = number 0 or 1,  1 entry is 50?, int.. possibly boolean.

Column 3, ignore entries starting with "zzOLD__" ?
Column 4, wtf are .mdl files? they're from warcraft 3?

col 5 and 6? figure out what they're for.
Column5 is either Spell Type,  or Spell slot or something similar
*/


SpellEffectsDB::Record SpellEffectsDB::getByName(const wxString name)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (name.IsSameAs(i->getString(EffectName), false) == true)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}

SpellEffectsDB::Record SpellEffectsDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->getUInt(ID)==id)
			return (*i);
	}
	//wxLogMessage(_T("NotFound: %s:%s#%d"), __FILE__, __FUNCTION__, __LINE__);
	throw NotFound();
}
// --
