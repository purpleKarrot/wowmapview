
#include "database.h"
#include "mpq.hpp"

ItemDatabase		items;

HelmGeosetDB		helmetdb;
ItemVisualEffectDB	effectdb;
ItemDisplayDB		itemdisplaydb;
dbc::CharStartOutfit		startdb;
ItemSubClassDB		subclassdb;
ItemVisualDB		visualdb;
ItemSetDB			setsdb;
ItemDB				itemdb;
AnimDB				animdb;
CharHairGeosetsDB	hairdb;
CharSectionsDB		chardb;
CharClassesDB		classdb;
CharFacialHairDB	facialhairdb;
CharRacesDB			racedb;
CreatureModelDB		modeldb;
CreatureSkinDB		skindb;
CreatureTypeDB		npctypedb;
NPCDB				npcdb;
LightSkyBoxDB			skyboxdb;
SpellItemEnchantmentDB	spellitemenchantmentdb;
ItemVisualsDB			itemvisualsdb;

AnimDB::Record AnimDB::getByAnimID(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->Get<unsigned int>(AnimID) == id)
			return (*i);
	}
	throw NotFound();
}

CharHairGeosetsDB::Record CharHairGeosetsDB::getByParams(unsigned int race, unsigned int gender, unsigned int section)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->Get<unsigned int>(Race)==race && i->Get<unsigned int>(Gender)==gender && i->Get<unsigned int>(Section)==section)
			return (*i);
	}
	throw NotFound();
}

int CharHairGeosetsDB::getGeosetsFor(unsigned int race, unsigned int gender)
{
	int n = 0;
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->Get<unsigned int>(Race)==race && i->Get<unsigned int>(Gender)==gender) {
			n++;
		}
	}

    return n;
}

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
					if (FS().getSize(fn.fn_str()) > 0)
						n++;
					else
						break;
				}
				break;
			case 1: // Character\Worgen\Female\WorgenFemaleFaceUpper27_09.blp
				for(int i=0; i<30; i++) {
					fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleFaceUpper%02d_%02d.blp"), section, i);
					if (FS().getSize(fn.fn_str()) > 0)
						n++;
					else
						break;
				}
				break;
			case 3: // Character\Worgen\Hair00_00.blp
				for(int i=0; i<20; i++) {
					fn.Printf(_T("Character\\Worgen\\Hair00_%02d.blp"), i);
					if (FS().getSize(fn.fn_str()) > 0)
						n++;
					else
						break;
				}
				break;
			case 4: // Character\Worgen\Male\WorgenMaleNakedPelvisSkin00_00.blp
				for(int i=0; i<20; i++) {
					fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleNakedPelvisSkin%02d_%02d.blp"), section, i);
					if (FS().getSize(fn.fn_str()) > 0)
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
		if (i->Get<unsigned int>(Race)==race && i->Get<unsigned int>(Gender)==gender && i->Get<unsigned int>(Type)==type && i->Get<unsigned int>(Section)==section) {
			n++;
		}
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
					if (FS().getSize(fn.fn_str()) > 0)
						n++;
					else
						break;
				}
				break;
			case 1: // Character\Worgen\Female\WorgenFemaleFaceUpper27_09.blp
				for(int i=0; i<30; i++) {
					fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleFaceUpper%02d_%02d.blp"), i, color);
					if (FS().getSize(fn.fn_str()) > 0)
						n++;
					else
						break;
				}
				break;
			case 3: // Character\Worgen\Hair00_00.blp
				for(int i=0; i<20; i++) {
					fn.Printf(_T("Character\\Worgen\\Hair%02d_%02d.blp"), i, color);
					if (FS().getSize(fn.fn_str()) > 0)
						n++;
					else
						break;
				}
				break;
			case 4: // Character\Worgen\Male\WorgenMaleNakedPelvisSkin00_00.blp
				for(int i=0; i<20; i++) {
					fn.Printf(_T("Character\\Worgen\\Female\\WorgenFemaleNakedPelvisSkin%02d_%02d.blp"), i, color);
					if (FS().getSize(fn.fn_str()) > 0)
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
		if (i->Get<unsigned int>(Race)==race && i->Get<unsigned int>(Gender)==gender && i->Get<unsigned int>(Type)==type && i->Get<unsigned int>(Color)==color) {
			n++;
		}
	}
    return n;
}

CharSectionsDB::Record CharSectionsDB::getByParams(unsigned int race, unsigned int gender, unsigned int type, unsigned int section, unsigned int color, unsigned int npc)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->Get<unsigned int>(Race)==race && i->Get<unsigned int>(Gender)==gender && i->Get<unsigned int>(Type)==type && i->Get<unsigned int>(Section)==section && i->Get<unsigned int>(Color)==color)
			return (*i);
	}
	throw NotFound();
}

CharRacesDB::Record CharRacesDB::getByName(wxString name)
{
	for(Iterator i=begin(); i!=end(); ++i) {
		const char* r;
		if (gameVersion == 30100)
			r = i->getString(NameV310);
		else
			r = i->getString(Name);
		if (name.IsSameAs(wxString(r,wxConvUTF8), false) == true)
			return (*i);
	}

	throw NotFound();
}

CharFacialHairDB::Record CharFacialHairDB::getByParams(unsigned int race, unsigned int gender, unsigned int style)
{
	if (gameVersion == 40000) {
		for(Iterator i=begin(); i!=end(); ++i)
		{
			if (i->Get<unsigned int>(RaceV400)==race && i->Get<unsigned int>(GenderV400)==gender && i->Get<unsigned int>(StyleV400)==style)
				return (*i);
		}
	} else {
		for(Iterator i=begin(); i!=end(); ++i)
		{
			if (i->Get<unsigned int>(Race)==race && i->Get<unsigned int>(Gender)==gender && i->Get<unsigned int>(Style)==style)
				return (*i);
		}
	}

	throw NotFound();
}

int CharFacialHairDB::getStylesFor(unsigned int race, unsigned int gender)
{
	int n = 0;
	if (gameVersion == 40000) {
		for(Iterator i=begin(); i!=end(); ++i)
		{
			if (i->Get<unsigned int>(RaceV400)==race && i->Get<unsigned int>(GenderV400)==gender) {
				n++;
			}
		}
	} else {
		for(Iterator i=begin(); i!=end(); ++i)
		{
			if (i->Get<unsigned int>(Race)==race && i->Get<unsigned int>(Gender)==gender) {
				n++;
			}
		}
	}
	return n;
}

HelmGeosetDB::Record HelmGeosetDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->Get<unsigned int>(TypeID)==id)
			return (*i);
	}

	throw NotFound();
}

CreatureModelDB::Record CreatureModelDB::getByFilename(wxString fn)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		wxString str(i->model_path(),wxConvUTF8);
		if(str.IsSameAs(fn, false) == true)
			return (*i);
	}

	throw DBCFile::NotFound();
}

CreatureModelDB::Record CreatureModelDB::getByID(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->id()==id)
			return (*i);
	}

	throw DBCFile::NotFound();
}

CreatureSkinDB::Record CreatureSkinDB::getByModelID(unsigned int id)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->Get<unsigned int>(ModelID) == id)
			return (*i);
	}

	throw NotFound();
}

CreatureSkinDB::Record CreatureSkinDB::getBySkinID(unsigned int id)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->Get<unsigned int>(SkinID) == id)
			return (*i);
	}

	throw NotFound();
}

CreatureTypeDB::Record CreatureTypeDB::getByID(unsigned int id)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->Get<unsigned int>(ID) == id)
			return (*i);
	}

	throw NotFound();
}

NPCDB::Record NPCDB::getByFilename(wxString fn)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(wxString(i->getString(Filename),wxConvUTF8) == fn) {
			//std::cout << i->getString(Filename).c_str() << "\n";
			return (*i);
		}
	}

	throw NotFound();
}

NPCDB::Record NPCDB::getByNPCID(unsigned int id)
{
	/// Brute force search for now
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if(i->Get<unsigned int>(NPCID) == id)
			return (*i);
	}

	throw NotFound();
}

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

void ItemSetDB::cleanup(ItemDatabase &p_itemdb)
{
	for(Iterator i=begin(); i!=end(); ++i) {
		for (int j=0; j<NumItems; j++) {
			int id;
			if (gameVersion == 40000)
				id = i->Get<unsigned int>(ItemIDBaseV400+j);
			else
				id = i->Get<unsigned int>(ItemIDBase+j);
			if (id > 0) {
				const ItemRecord &r = p_itemdb.getByID(id);
				if (r.type > 0) {
					avail.insert(i->Get<unsigned int>(SetID));
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

ItemDB::Record ItemDB::getByID(unsigned int id)
{
	if (!inited) {
		int j=0;
		for(Iterator i=begin(); i!=end(); ++i)
		{
			itemLookup[i->Get<unsigned int>(ID)] = j;
			itemDisplayLookup[i->Get<unsigned int>(ItemDisplayInfo)] = j;
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
}

ItemDB::Record ItemDB::getByDisplayId(unsigned int id)
{
	if (!inited) {
		int j=0;
		for(Iterator i=begin(); i!=end(); ++i)
		{
			itemLookup[i->Get<unsigned int>(ID)] = j;
			itemDisplayLookup[i->Get<unsigned int>(ItemDisplayInfo)] = j;
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
		ItemDB::Record r = itemdb.getByID(id);
		model = r.Get<int>(ItemDB::ItemDisplayInfo);
		itemclass = r.Get<int>(ItemDB::Itemclass);
		subclass = r.Get<int>(ItemDB::Subclass);
		type = r.Get<int>(ItemDB::InventorySlot);
		switch(r.Get<int>(ItemDB::Sheath)) {
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
		itemset.insert(it->id());
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

const ItemRecord& ItemDatabase::getByID(int id)
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
		ItemDB::Record r = itemdb.getByID(id);
		ItemRecord rec;
		rec.id = id;
		rec.model = r.Get<int>(ItemDB::ItemDisplayInfo);
		rec.itemclass = r.Get<int>(ItemDB::Itemclass);
		rec.subclass = r.Get<int>(ItemDB::Subclass);
		rec.type = r.Get<int>(ItemDB::InventorySlot);
		switch(r.Get<int>(ItemDB::Sheath)) {
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

SpellEffectsDB::Record SpellEffectsDB::getByName(const wxString name)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (name.IsSameAs(wxString(i->getString(EffectName),wxConvUTF8), false) == true)
			return (*i);
	}

	throw NotFound();
}

SpellEffectsDB::Record SpellEffectsDB::getById(unsigned int id)
{
	for(Iterator i=begin(); i!=end(); ++i)
	{
		if (i->Get<unsigned int>(ID)==id)
			return (*i);
	}

	throw NotFound();
}
