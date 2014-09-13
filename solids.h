#ifndef ___SOLIDS_H___
#define ___SOLIDS_H___

#include "geom.h"

typedef struct
{
	int type;
	void* data;
} solid;

void* readSolid(void);
int pointInSolid(point p, void* sol);
int segSolidCut(seg sg, void* sol, point* p, vec* normal);
int segSolidCutAll(seg sg, void* sol, point p[], vec normal[]);
void freeSolid(void* s);

#endif
