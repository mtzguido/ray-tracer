#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "geom.h"
#include "common.h"
#include "input.h"

typedef struct
{
	point start,end;
	CORD_TYPE rad;
} cyl;

void* readCyl()
{
	point p;
	cyl* c;
		
	c = malloc(sizeof(cyl));
	
	p.x = readDouble();
	p.y = readDouble();
	p.z = readDouble();
	c->start = p;
	
	p.x = readDouble();
	p.y = readDouble();
	p.z = readDouble();
	c->end = p;
	
	c->rad = readDouble();
	
	return c;
}
	
int pointInCyl(point pt, void* sol)
{
	cyl* c = sol;
	
	return (distPoint2Line(pt,c->start,c->end) < c->rad);
}

int segCylCut(seg s, void* sol, point* in, vec* n)
{
	vec u,v;
	cyl cy;
	CORD_TYPE a,b,c;
	CORD_TYPE disc;
	CORD_TYPE alpha;
	point tp;
	
	cy = *(cyl*)sol;
	
	u = crossProduct(points2vec(cy.start,cy.end),points2vec(cy.start,s.p));
	v = crossProduct(points2vec(cy.start,cy.end),points2vec(s.p,s.q));
	
	a = vecModulus2(v);
	b = 2 * dotProduct(u,v);
	c = vecModulus2(u) - cy.rad*cy.rad*vecModulus2(points2vec(cy.start,cy.end));
	
	disc = b*b - 4*a*c;
	
	if(disc < 0)
		return 0;
		
	disc = sqrt(disc);
	
	alpha = (-b - disc)/(2*a);
	if(alpha*(1-alpha) >= 0)
	{
		tp = vecLinComb(1-alpha,s.p,alpha,s.q);
		
		if(in != NULL) *in = tp;
		if(n  != NULL) *n = orthogonalize(points2vec(cy.start,tp),points2vec(cy.start,cy.end));
		
		return 1;
	}
	
	alpha = (-b + disc)/(2*a);
	if(alpha*(1-alpha) >= 0)
	{
		tp = vecLinComb(1-alpha,s.p,alpha,s.q);
		
		if(in != NULL) *in = tp;
		if(n  != NULL) *n = orthogonalize(points2vec(cy.start,tp),points2vec(cy.start,cy.end));
		
		return 1;
	}
	
	return 0;
}

int segCylCutAll(seg s, void* sol, point* in, vec* n)
{
	vec u,v;
	cyl cy;
	CORD_TYPE a,b,c;
	CORD_TYPE disc;
	CORD_TYPE alpha;
	point tp;
	int count = 0;
	
	cy = *(cyl*)sol;
	
	u = crossProduct(points2vec(cy.start,cy.end),points2vec(cy.start,s.p));
	v = crossProduct(points2vec(cy.start,cy.end),points2vec(s.p,s.q));
	
	a = vecModulus2(v);
	b = 2 * dotProduct(u,v);
	c = vecModulus2(u) - cy.rad*cy.rad*vecModulus2(points2vec(cy.start,cy.end));
	
	disc = b*b - 4*a*c;
	
	if(disc < 0)
		return 0;
		
	disc = sqrt(disc);
	
	alpha = (-b - disc)/(2*a);
	if(alpha*(1-alpha) >= 0)
	{
		tp = vecLinComb(1-alpha,s.p,alpha,s.q);
		
		if(in != NULL) in[count] = tp;
		if(n  != NULL) n[count]  = orthogonalize(points2vec(cy.start,tp),points2vec(cy.start,cy.end));
		
		count++;
	}
	
	alpha = (-b + disc)/(2*a);
	if(alpha*(1-alpha) >= 0)
	{
		tp = vecLinComb(1-alpha,s.p,alpha,s.q);
		
		if(in != NULL) in[count] = tp;
		if(n  != NULL) n[count]  = orthogonalize(points2vec(cy.start,tp),points2vec(cy.start,cy.end));
		
		count++;
	}
	
	return count;
}
