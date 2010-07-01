#include "areadb.h"
#include <string>

dbc::AreaTable gAreaDB;
dbc::Map gMapDB;
dbc::LoadingScreens gLoadingScreensDB;
dbc::Light gLightDB;
dbc::LightSkybox gLightSkyboxDB;
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

std::string getAreaName(const dbc::AreaTable& table, int pAreaID)
{
	try
	{
		dbc::AreaTable::Record rec = get_by_ID(table, pAreaID);
		unsigned int parent_id = rec.parent();

		if (!parent_id)
			return rec.name();

		return getAreaName(table, parent_id) + ": " + rec.name();
	}
	catch (DBCFile::NotFound)
	{
		return "Unknown location";
	}
}
