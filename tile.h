#ifndef ___TILE_H___
#define ___TILE_H___

#include "common.h"
#include "geom.h"

typedef struct
{
	CORD_TYPE a,b,c,d; /* ax + by + cz + d <= 0 */
	CORD_TYPE sqsize;
	int parity;
} tile;

void* readTile(void);
int pointInTile	(point p, void* sol);
int segTileCut(seg s, void* sol, point* in, vec* n);

#endif
