#ifndef ___GEOM_H___
#define ___GEOM_H___

#include "common.h"

struct cord_triple
{
	CORD_TYPE x,y,z;
};

struct cord_triple_2
{
	struct cord_triple p,q;
};

typedef struct cord_triple point;
typedef struct cord_triple vec;
typedef struct cord_triple_2 seg;

seg points2seg(point a, point b);
vec seg2vec(seg s);
vec points2vec(point a, point b);
vec vecSubtract(vec v, vec u);

CORD_TYPE randomVal(void);
void printVec(vec v);
vec coords2vec(CORD_TYPE x, CORD_TYPE y, CORD_TYPE z);

vec vecSum(vec u, vec v);
vec vecScale(CORD_TYPE a, vec u);
vec vecLinComb(CORD_TYPE a, vec u, CORD_TYPE b, vec v);

CORD_TYPE dotProduct(vec u, vec v);
vec crossProduct(vec u, vec v);
CORD_TYPE tripleProduct(vec u, vec v, vec w);

CORD_TYPE vecModulus(vec v);
CORD_TYPE vecModulus2(vec v);
CORD_TYPE segLength(seg s);
CORD_TYPE pointDist(point a, point b);
CORD_TYPE pointDist2(point a, point b);

vec unit(vec v);
seg extend(seg s);

vec projVec(vec u, vec v);
vec orthogonalize(vec u, vec v);
vec orthoRand(vec v);

int coplanar(point a, point b, point c, point d);
int segsIntersect(seg s, seg r);
int pointInSeg(point p, seg s);

point project2Plane(point p, point a, point b, point c);
CORD_TYPE distPoint2Plane(point p, point a, point b, point c);
CORD_TYPE distPoint2Line(point p, point a, point b);
CORD_TYPE cosAngle(vec u, vec v);

#endif
