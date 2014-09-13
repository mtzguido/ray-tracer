#ifndef ___PLANE_H___
#define ___PLANE_H___

#include "common.h"
#include "geom.h"

typedef struct
{
	CORD_TYPE a,b,c,d; /* ax + by + cz + d <= 0 */
} plane;

void* readPlane(void);
int pointInPlane(point p, void* sol);
int segPlaneCut(seg s, void* sol, point* in, vec* n);

#endif
