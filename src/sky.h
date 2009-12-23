#ifndef SKY_H
#define SKY_H

#include "video.h"
#include "model.h"
#include "mpq.h"
#include <vector>
#include "DBCFile.h"

struct SkyColor {
	Vec3D color;
	int time;

	SkyColor( int t, int col );
};

class Sky {
public:
	Vec3D pos;
	float r1, r2;

	Sky( DBCFile::Iterator data );

	std::vector<SkyColor> colorRows[36];
	int mmin[36];

	char name[32];

	Vec3D colorFor(int r, int t) const;

	float weight;
	bool global;

    bool operator<(const Sky& s) const
	{
		if (global) return false;
		else if (s.global) return true;
		else return r2 < s.r2;
	}
};

enum SkyColorNames {
	LIGHT_GLOBAL_DIFFUSE,
	LIGHT_GLOBAL_AMBIENT,
	SKY_COLOR_0,
	SKY_COLOR_1,
	SKY_COLOR_2,
	SKY_COLOR_3,
	SKY_COLOR_4,
	FOG_COLOR,
	SKY_UNKNOWN_1,
	SUN_COLOR,
	SUN_HALO_COLOR,
	SKY_UNKNOWN_2,
	CLOUD_COLOR,
	SKY_UNKNOWN_3,
	SKY_UNKNOWN_4,
	WATER_COLOR_DARK,
	WATER_COLOR_LIGHT,	
	SHADOW_COLOR
};

class Skies {
	std::vector<Sky> skies;
	int numSkies;
	int cs;
	Model *stars;
	char skyname[128];

public:

	Vec3D colorSet[18];

	Skies( int mapid );
	~Skies();

	void findSkyWeights(Vec3D pos);
	void initSky(Vec3D pos, int t);

	void draw();

	bool drawSky(const Vec3D &pos);
	bool hasSkies() { return numSkies > 0; }

	void setupLighting();

	//void debugDraw(unsigned int *buf, int dim);
};


/*
	It seems that lighting info is also stored in lights.lit, so I
	wonder what the heck is in Dnc.db. Maybe just light directions and/or
	sun/moon positions...?
*/
struct OutdoorLightStats {
	int time; // converted from hour:min to the 2880 half-minute ticks thing used in the other Sky thing

	float dayIntensity, nightIntensity, ambientIntensity, fogIntensity, fogDepth;
	Vec3D dayColor, nightColor, ambientColor, fogColor, dayDir, nightDir;

	void init(MPQFile &f);

	void interpolate(OutdoorLightStats *a, OutdoorLightStats *b, float r);
	void setupLighting();
    // void setupFog(); // TODO: add fog maybe?

};


class OutdoorLighting {

	std::vector<OutdoorLightStats> lightStats;

public:
	OutdoorLighting(char *fname);

	OutdoorLightStats getLightStats(int time);

};




#endif

