#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "geom.h"
#include "tile.h"
#include "input.h"

static int fsign(CORD_TYPE x)
{
	if(x>0) return 1;
	if(x<0) return -1;
	return 0;
}

void* readTile()
{
	tile* pl;
	pl = malloc(sizeof(tile));
	
	pl->a = readDouble();
	pl->b = readDouble();
	pl->c = readDouble();
	pl->d = readDouble();
	
	pl->sqsize = readDouble();
	pl->parity = readInt();
	
	return pl;
}

int pointInTile(point p, void* sol)
{
	tile* pl = (tile*)sol;
	CORD_TYPE a = 0;
	
	a = pl->a*p.x + pl->b*p.y + pl->c*p.z + pl->d;
	
	return (a <= 0);
}

int segTileCut(seg s, void* sol, point* in, vec* n)
{
	tile* pl;
	point tp;
	CORD_TYPE d1,d2;
	
	pl = (tile*)sol;
		
	if(n != NULL) *n = coords2vec(pl->a,pl->b,pl->c);
	
	d1 = pl->a*s.p.x + pl->b*s.p.y + pl->c*s.p.z + pl->d;
	d2 = pl->a*s.q.x + pl->b*s.q.y + pl->c*s.q.z + pl->d;

	if(fsign(d1)*fsign(d2) >= 0)
		return 0;
	
	tp = vecLinComb(d2/(d2-d1),s.p,d1/(d1-d2),s.q);
	if(in != NULL) *in = tp;
	
	if(((int)(fabs(floor(tp.x/pl->sqsize))
	 + fabs(floor(tp.y/pl->sqsize))
	 + fabs(floor(tp.z/pl->sqsize))))
	            %2 == pl->parity%2)
		return 0;
	
	return 1;
}
