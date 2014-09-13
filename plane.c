#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "geom.h"
#include "plane.h"
#include "input.h"

static int fsign(CORD_TYPE x)
{
	if(x>0) return 1;
	if(x<0) return -1;
	return 0;
}

void* readPlane()
{
	plane* pl;
	pl = malloc(sizeof(plane));
	
	pl->a = readDouble();
	pl->b = readDouble();
	pl->c = readDouble();
	pl->d = readDouble();
	
	return pl;
}

int pointInPlane(point p, void* data)
{
	plane* pl = (plane*)data;
	CORD_TYPE a = 0;
	
	a = pl->a*p.x + pl->b*p.y + pl->c*p.z + pl->d;
	
	return (a <= 0);
}

int segPlaneCut(seg s, void* data, point* in, vec* n)
{
	plane* pl;
	point tp;
	CORD_TYPE d1,d2;
	
	pl = (plane*)data;
		
	if(n != NULL) *n = coords2vec(pl->a,pl->b,pl->c);

	d1 = pl->a*s.p.x + pl->b*s.p.y + pl->c*s.p.z + pl->d;
	d2 = pl->a*s.q.x + pl->b*s.q.y + pl->c*s.q.z + pl->d;

	if(fsign(d1)*fsign(d2) >= 0)
		return 0;
	
	tp = vecLinComb(d2/(d2-d1),s.p,d1/(d1-d2),s.q);
	if(in != NULL) *in = tp;
	
	return 1;
}

