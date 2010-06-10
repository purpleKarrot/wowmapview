#ifndef WOWMAPVIEW_H
#define WOWMAPVIEW_H

#include <vector>
#include <string>
#include "appstate.h"

extern std::vector<AppState*> gStates;
extern bool gPop;

#define gLog printf

float frand();
float randfloat(float lower, float upper);
int randint(int lower, int upper);

// Area database
class AreaDB;
extern AreaDB gAreaDB;

void status_message(std::string const& message);

#endif
