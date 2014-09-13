#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "geom.h"
#include "common.h"
#include "sphere.h"
#include "input.h"

void* readSphere()
{
	sphere* s;
	s = (sphere*)malloc(sizeof(sphere));
	
	s->center.x = readDouble();
	s->center.y = readDouble();
	s->center.z = readDouble();
	
	s->radius   = readDouble();
	
	return s;
}

int segSphereCut(seg s, void* sol, point* in, vec* normal)
{
	CORD_TYPE a,b,c;
	CORD_TYPE disc;
	CORD_TYPE alpha;
	point tp;
	sphere S;
	
	S = *(sphere*)sol;
	
	a = vecModulus2(seg2vec(s));
	
	if(a < EPS)
		return 0;
	
	b = 2 * dotProduct(seg2vec(s),points2vec(S.center,s.p));
	c = vecModulus2(points2vec(S.center,s.p)) - S.radius*S.radius;
	
	disc = b*b - 4*a*c;
	
	if(disc < 0)
		return 0;
		
	disc = sqrt(disc);
	
	alpha = (-b - disc)/(2*a);

	if(alpha*(1-alpha)>=0)
	{
		tp = vecLinComb(1-alpha,s.p,alpha,s.q);

		if(in != NULL) *in = tp;
		if(normal != NULL) *normal = points2vec(S.center,tp);
		return 1;
	}

	alpha = (-b + disc)/(2*a);

	if(alpha*(1-alpha)>=0)
	{
		tp = vecLinComb(1-alpha,s.p,alpha,s.q);

		if(in != NULL) *in = tp;
		if(normal != NULL) *normal = points2vec(S.center,tp);
		return 1;
	}
	
	return 0;
}

int segSphereCutAll(seg s, void* sol, point* parr, vec* normalarr)
{
	CORD_TYPE a,b,c;
	CORD_TYPE disc;
	CORD_TYPE alpha;
	point tp;
	int count = 0;
	sphere S;
	
	S = *(sphere*)sol;
	
	a = vecModulus2(seg2vec(s));
	
	if(a < EPS)
		return 0;
	
	
	b = 2 * dotProduct(seg2vec(s),points2vec(S.center,s.p));
	c = vecModulus2(points2vec(S.center,s.p)) - S.radius*S.radius;
	
	disc = b*b - 4*a*c;
		
	if(disc < 0)
		return 0;
			
	disc = sqrt(disc);
	
	alpha = (-b - disc)/(2*a);

	if(alpha*(1-alpha)>=0)
	{
		tp = vecLinComb(1-alpha,s.p,alpha,s.q);

		if(parr != NULL) parr[count] = tp;
		if(normalarr != NULL) normalarr[count] = points2vec(S.center,tp);
		count++;
	}

	alpha = (-b + disc)/(2*a);

	if(alpha*(1-alpha)>=0)
	{
		tp = vecLinComb(1-alpha,s.p,alpha,s.q);

		if(parr != NULL) parr[count] = tp;
		if(normalarr != NULL) normalarr[count] = points2vec(S.center,tp);
		count++;
	}
	
	return count;
}

int pointInSphere(point p, void* s) /* r inutil */
{
	sphere S;
	
	S = *(sphere*)s;
	
	return (pointDist2(p,S.center) <= S.radius*S.radius);
}
