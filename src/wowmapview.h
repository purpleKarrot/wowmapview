#ifndef WOWMAPVIEW_H
#define WOWMAPVIEW_H

#include <vector>
#include <string>
#include "appstate.h"
#include "font.h"

extern std::vector<AppState*> gStates;
extern bool gPop;

#define gLog printf

extern Font *f16, *f24, *f32;

float frand();
float randfloat(float lower, float upper);
int randint(int lower, int upper);

// Area database
class AreaDB;
extern AreaDB gAreaDB;

#endif
