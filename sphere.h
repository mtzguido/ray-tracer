#ifndef ___SPHERE_H___
#define ___SPHERE_H___

#include "common.h"
#include "geom.h"

typedef struct
{
	point center;
	CORD_TYPE radius;
} sphere;

void* readSphere(void);
int segSphereCut(seg, void*, point*, vec*);
int segSphereCutAll(seg, void*, point*, vec*);
int pointInSphere(point, void*);

#endif
