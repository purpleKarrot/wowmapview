#ifndef AREADB_H
#define AREADB_H
#include "AreaTable.hpp"
#include "Map.hpp"
#include "Light.hpp"
#include "LoadingScreens.hpp"
#include "LightSkybox.hpp"
#include <string>

std::string getAreaName(const dbc::AreaTable& table, int pAreaID);

class LightIntBandDB: public DBCFile
{
public:
	LightIntBandDB() :
		DBCFile("DBFilesClient\\LightIntBand.dbc")
	{
	}

	/// Fields
	static const size_t ID = 0; // uint
	static const size_t Entries = 1; // uint
	static const size_t Times = 2; // uint
	static const size_t Values = 18; // uint
};

class LightFloatBandDB: public DBCFile
{
public:
	LightFloatBandDB() :
		DBCFile("DBFilesClient\\LightFloatBand.dbc")
	{
	}

	/// Fields
	static const size_t ID = 0; // uint
	static const size_t Entries = 1; // uint
	static const size_t Times = 2; // uint
	static const size_t Values = 18; // float
};

class GroundEffectTextureDB: public DBCFile
{
public:
	GroundEffectTextureDB() :
		DBCFile("DBFilesClient\\GroundEffectTexture.dbc")
	{
	}

	/// Fields
	static const size_t ID = 0; // uint
	static const size_t Doodads = 1; // uint[4]
	static const size_t Weights = 5; // uint[4]
	static const size_t Amount = 9; // uint
	static const size_t TerrainType = 10; // uint
};

class GroundEffectDoodadDB: public DBCFile
{
public:
	GroundEffectDoodadDB() :
		DBCFile("DBFilesClient\\GroundEffectDoodad.dbc")
	{
	}

	/// Fields
	static const size_t ID = 0; // uint
	static const size_t Filename = 1; // string
};

class LiquidTypeDB: public DBCFile
{
public:
	LiquidTypeDB() :
		DBCFile("DBFilesClient\\LiquidType.dbc")
	{
	}

	/// Fields
	static const size_t ID = 0; // uint
	static const size_t Filename = 1; // string
	static const size_t flags = 2; // Water: 1, 2, 4, 8; Magma: 8, 16, 32, 64; Slime: 2, 64, 256; WMO Ocean: 1, 2, 4, 8, 512
	static const size_t type = 3; // 0: Water, 1: Ocean, 2: Magma, 3: Slime
};

void OpenDBs();

extern dbc::AreaTable gAreaDB;
extern dbc::Map gMapDB;
extern dbc::LoadingScreens gLoadingScreensDB;
extern dbc::Light gLightDB;
extern dbc::LightSkybox gLightSkyboxDB;
extern LightIntBandDB gLightIntBandDB;
extern LightFloatBandDB gLightFloatBandDB;
extern GroundEffectDoodadDB gGroundEffectDoodadDB;
extern GroundEffectTextureDB gGroundEffectTextureDB;
extern LiquidTypeDB gLiquidTypeDB;

#endif
