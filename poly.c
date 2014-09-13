#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "geom.h"
#include "sphere.h"
#include "poly.h"
#include "input.h"

int validPolyg(polyg P) /* revisa si son coplanares los puntos */
{
	vec a,b,c;
	int i;

	assert(P.nvertices > 2);

	a = points2vec(P.vertices[0],P.vertices[1]);
	b = points2vec(P.vertices[0],P.vertices[2]);
	
	for(i=3;i<P.nvertices;++i)
	{
		c = points2vec(P.vertices[0],P.vertices[i]);
		if(fabs(tripleProduct(a,b,c)) > EPS) return 0;
	}

	return 1;
}

vec polygNormal(polyg P)
{
	vec a,b;
	
	assert(validPolyg(P));

	a = points2vec(P.vertices[0],P.vertices[1]);
	b = points2vec(P.vertices[0],P.vertices[2]);

	return crossProduct(a,b);
}

int pointInPolyg(point a, polyg P)
{
	vec o;
	seg ray;
	int i;
	int c;

	assert(validPolyg(P));

	if(!coplanar(P.vertices[0],P.vertices[1],P.vertices[2],a))
		return 0; /* no coplanares */	

	for(i=1;i<P.nvertices;++i)
		if(pointInSeg(a,points2seg(P.vertices[i],P.vertices[i-1])))
			return 1;
		
	if(pointInSeg(a,points2seg(P.vertices[0],P.vertices[P.nvertices-1])))
		return 1;

	newray:
	o = orthoRand(polygNormal(P));
	ray = extend(points2seg(a,vecSum(a,o)));

	c = 0;

	for(i=0;i<P.nvertices;++i)
		if(pointInSeg(P.vertices[i],ray))
			goto newray;

	for(i=1;i<P.nvertices;++i)
		if(segsIntersect(ray,points2seg(P.vertices[i],P.vertices[i-1])))
			c=!c;

	if(segsIntersect(ray, points2seg(P.vertices[0],P.vertices[P.nvertices-1])))
		c=!c;

	return c;
}

int segPlaneIntersect(seg s, point a, point b, point c, point* r,vec normal)
{
	CORD_TYPE d1,d2;

	if(dotProduct(points2vec(a,s.p),normal) /* si estan del mismo lado */
	  * dotProduct(points2vec(a,s.q),normal) > 0)
			return 0;
	
	d1 = distPoint2Plane(s.p,a,b,c);
	d2 = distPoint2Plane(s.q,a,b,c);
	
	if(r != NULL)
	{	
		*r = vecLinComb(d2,s.p,d1,s.q); /* corte de s con el plano */
		*r = vecScale(1/(d1+d2),*r);
	}
	
	return 1;
}

int segPolygCut(seg s, polyg P, point* r, vec* normal)
{
	vec tn;
	point tp;

	assert(validPolyg(P));
	
	tn = polygNormal(P);
	if(normal != NULL) *normal = tn;
	
	if(!segPlaneIntersect(s,P.vertices[0],P.vertices[1],P.vertices[2],&tp,tn))
		return 0;

	if(r != NULL) *r = tp;

	return pointInPolyg(tp,P);
}

int pointInPolyh(point p, void* data)
{
	int i,j;
	int c;
	seg ray;
	vec o;
	polyh H;

	H = *(polyh*)data;
	
#ifndef DONT_USE_BOUND
	if(!pointInSphere(p,&(H.bound)))
		return 0;
#endif

	for(i=0;i<H.nfaces;++i)
		if(pointInPolyg(p,H.faces[i]))
			return 1;
		
	newray:
	o = orthoRand(orthoRand(p));
	ray.p = p;
	ray.q = vecSum(p,o);
	ray = extend(ray);
	
	c = 0;
	for(i=0;i<H.nfaces;++i)
		for(j=0;j<H.faces[i].nvertices;++j)
			if(segsIntersect(ray,points2seg(H.faces[i].vertices[j],H.faces[i].vertices[(j+1)%H.faces[i].nvertices])))
				goto newray;
				
	for(i=0;i<H.nfaces;++i)
		if(segPolygCut(ray,H.faces[i],NULL,NULL))
			c = !c;

	return c;
}

int segPolyhCut(seg s, void* data, point* in, vec* normal)
{
	point r_;
	vec normal_;
	CORD_TYPE dist_ = 0;
	polyh H;
	char found = 0;
	int i;

	H = *(polyh*)data;

#ifndef DONT_USE_BOUND
	/* if(!segSphereCut(s,&(H.bound),NULL,NULL))
		return 0; */
		
	/* Esto esta mal! Puede cortar el poliedro por mas que no
	 * corte a la esfera. Cuando esto pasa, ambos puntos son
	 * interiores a la esfera, o sea que podría arreglarse
	 * checkeando que al menos un punto sea exterior, pero
	 * la ganancia en performance no se justifica. Se deberia
	 * usar una esfera inscripta ademas de la circunscripta o
	 * algun otro método... */
#endif
	
	for(i=0;i<H.nfaces;++i)
		if(segPolygCut(s,H.faces[i],&r_,&normal_))
			if((!found || (pointDist2(s.p,r_) < dist_)))
			{
				if(in != NULL) *in = r_;
				if(normal != NULL) *normal = normal_;
				dist_ = pointDist2(s.p,r_);
				found = 1;
			}

	return found;
}

int segPolyhCutAll(seg s, void* data, point in[], vec normal[])
{
	CORD_TYPE* dist;

	CORD_TYPE tdist;
	vec tnormal;
	point tin;
	
	polyh H;
	int i;
	int j;
	int count = 0;

	H = *(polyh*)data;

#ifndef DONT_USE_BOUND
	/*
	if(!segSphereCut(s,&(H.bound),NULL,NULL))
		return 0; 
	*/
	/* esto tambien esta mal (ver segPolyhCut) */
#endif

	dist = (CORD_TYPE*)malloc(H.nfaces * sizeof(CORD_TYPE));
	assert(dist != NULL);
	
	for(i=0;i<H.nfaces;++i)
		if(segPolygCut(s,H.faces[i],&tin,&tnormal))
		{
			tdist = vecModulus2(points2vec(s.p,tin));
			
			for(j=count-1;j>=0 && dist[j]>tdist;--j)
			{
				dist[j+1] = dist[j];
				if(in != NULL) in[j+1] = in[j];
				if(normal != NULL) normal[j+1] = normal[j];
			}
		
			dist[j+1] = tdist;
			if(in != NULL) in[j+1] = tin;
			if(normal != NULL) normal[j+1] = tnormal;
			
			count++;
		}
		
	for(i=1;i<count;++i)
		assert(dist[i]>=dist[i-1]); /* ordenado */

	free(dist);
	return count;
}

polyg readPolyg()
{
	polyg P;
	int i;
	
	P.nvertices = readInt();
	P.vertices = (point*)malloc(P.nvertices * sizeof(point));
	
	for(i=0;i<P.nvertices;++i)
	{
		P.vertices[i].x = readDouble();
		P.vertices[i].y = readDouble();
		P.vertices[i].z = readDouble();
	}
	
	return P;
}

void* readPolyh()
{
	polyh* H;
	int i;
	int nvert;
	
#ifndef DONT_USE_BOUND
	int j;
	CORD_TYPE rad;
#endif
	
	H = (polyh*) malloc(sizeof(polyh));

	H->nfaces = readInt();
	H->faces = (polyg*)malloc(H->nfaces * sizeof(polyg));
	
	nvert = 0;
	for(i=0;i<H->nfaces;++i)
	{
		H->faces[i] = readPolyg();
	}
	
#ifndef DONT_USE_BOUND	
	H->bound.center = coords2vec(0,0,0);

	for(i=0;i<H->nfaces;++i)
	{
		for(j=0;j< H->faces[i].nvertices;++j)
			H->bound.center = vecSum(H->bound.center,H->faces[i].vertices[j]);
		
		nvert += j;
	}
	
	H->bound.center = vecScale(1.0/nvert,H->bound.center);
	
	rad = 0;
	for(i=0;i<H->nfaces;++i)
		for(j=0;j< H->faces[i].nvertices;++j)
			if(pointDist(H->bound.center,H->faces[i].vertices[j]) > rad)
				rad = pointDist(H->bound.center,H->faces[i].vertices[j]);
	
	H->bound.radius = rad;
#endif
	
	return H;
}

void freePolyg(polyg P)
{
	free(P.vertices);
}

void freePolyh(void* data)
{
	polyh* H = data;
	int i;
	
	for(i=0;i< H->nfaces;++i)
		freePolyg(H->faces[i]);
}
