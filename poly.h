#ifndef ___POLY_H___
#define ___POLY_H___

#include "geom.h"

/* #define DONT_USE_BOUND */
/* para no usar la bounding sphere */

struct polyg
{
	int nvertices;
	point* vertices;
};

struct polyh
{
	int nfaces;
	struct polyg* faces;
#ifndef DONT_USE_BOUND
	sphere bound;
#endif
};

typedef struct polyg polyg;
typedef struct polyh polyh;

int pointInPolyh(point p, void* s);
int segPolyhCut(seg s, void* sol, point* in, vec* normal);
int segPolyhCutAll(seg s, void* sol, point in[], vec normal[]);
void* readPolyh(void);
void freePolyh(void*);


#endif
