#include "areadb.h"
#include <string>

AreaDB gAreaDB;
dbc::Map gMapDB;
LoadingScreensDB gLoadingScreensDB;
dbc::Light gLightDB;
LightSkyboxDB gLightSkyboxDB;
LightIntBandDB gLightIntBandDB;
LightFloatBandDB gLightFloatBandDB;
GroundEffectDoodadDB gGroundEffectDoodadDB;
GroundEffectTextureDB gGroundEffectTextureDB;
LiquidTypeDB gLiquidTypeDB;

void OpenDBs()
{
	gAreaDB.open();
	gMapDB.open();
	gLoadingScreensDB.open();
	gLightDB.open();
	gLightSkyboxDB.open();
	gLightIntBandDB.open();
	gLightFloatBandDB.open();
	gGroundEffectDoodadDB.open();
	gGroundEffectTextureDB.open();
	gLiquidTypeDB.open();
}


std::string AreaDB::getAreaName( int pAreaID )
{
	unsigned int regionID = 0;
	std::string areaName = "";
	try 
	{
		AreaDB::Record rec = get_by_ID(gAreaDB, pAreaID );
		areaName = rec.name();
		regionID = rec.parent();
	} 
	catch(DBCFile::NotFound)
	{
		areaName = "Unknown location";
	}
	if (regionID != 0) 
	{
		try 
		{
			AreaDB::Record rec = get_by_ID(gAreaDB, regionID );
			areaName = std::string(rec.name()) + std::string(": ") + areaName;
		} 
		catch(DBCFile::NotFound)
		{
			areaName = "Unknown location";
		}
	}
	return areaName;
}
