#ifndef ___CYL_H___
#define ___CYL_H___

#include <stdio.h>
#include "geom.h"
#include "common.h"

void* readCyl(void);
int pointInCyl(point pt, void* sol);
int segCylCut(seg s, void* sol, point* in, vec* n);
int segCylCutAll(seg s, void* sol, point* in, vec* n);

#endif
