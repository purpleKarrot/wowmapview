#ifndef WOWMAPVIEW_H
#define WOWMAPVIEW_H

#include <vector>
#include <string>
#include "appstate.h"

#define gLog printf

float frand();
float randfloat(float lower, float upper);
int randint(int lower, int upper);

// Area database
class AreaDB;
extern AreaDB gAreaDB;

void status_message(std::string const& message);

#endif
