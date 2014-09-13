#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "solids.h"

/* PRIMITIVAS: */
#include "sphere.h"
#include "plane.h"
#include "poly.h"
#include "tile.h"
#include "cyl.h"

#define MAXCUTS 50

int pointInIntersection(point, void*);
int pointInUnion(point, void*);
int pointInDiff(point, void*);

int segIntersectionCut(seg, void*, point*, vec*);
int segUnionCut(seg, void*, point*, vec*);
int segDiffCut(seg, void*, point*, vec*);

int segIntersectionCutAll(seg, void*, point[], vec[]);
int segUnionCutAll(seg, void*, point[], vec[]);
int segDiffCutAll(seg, void*, point[], vec[]);

void* readCouple(void);
void freeCouple(void*);

char* ID[] = {"INTERSECT", "UNION", "DIFF", "SPHERE",
              "POLYH", "PLANE", "TILE", "CYL"};
int (*POINTIN[])(point, void*) = {pointInIntersection, pointInUnion, pointInDiff, pointInSphere,
                                  pointInPolyh, pointInPlane, pointInTile, pointInCyl};
int (*SEGCUT[])(seg, void*, point*, vec*) = {segIntersectionCut, segUnionCut, segDiffCut, segSphereCut,
                                             segPolyhCut, segPlaneCut, segTileCut, segCylCut};
int (*SEGCUTALL[])(seg, void*, point[], vec[]) = {segIntersectionCutAll, segUnionCutAll, segDiffCutAll, segSphereCutAll,
                                                  segPolyhCutAll, segPlaneCut, segTileCut, segCylCutAll};
void* (*READ[])(void) = {readCouple, readCouple, readCouple, readSphere,
                         readPolyh, readPlane, readTile, readCyl};
void (*FREE[])(void*) = {freeCouple, freeCouple, freeCouple, free,
                         freePolyh, free, free, free};

void* readSolid()
{
	char str[100];
	unsigned int i;
	solid* s;

	s = (solid*)malloc(sizeof(solid));
	
	scanf("%s",str);
	
	for(i=0;i<sizeof(ID)/sizeof(char*);++i)
		if(strcmp(str,ID[i]) == 0)
		{
			s->type = i;
			s->data = READ[i]();
			return s;
		}

	return NULL;
}

void freeSolid(void* sol)
{
	solid* s = sol;
	
	FREE[s->type](s->data);
}

void* readCouple()
{
	void** arr;
	arr = malloc(2 * sizeof(void*));
	arr[0] = readSolid();
	arr[1] = readSolid();
	
	return arr;
}

void freeCouple(void* arr)
{
	void** t = arr;
	freeSolid(*t);
	freeSolid(*(t+1));
}

int pointInSolid(point p, void* sol)
{
	solid* s = (solid*)sol;
	return POINTIN[s->type](p,s->data);
}

int pointInIntersection(point p, void* data)
{
	void** arr;
	arr = data;
	
	return pointInSolid(p,arr[0]) && pointInSolid(p,arr[1]);
}

int pointInUnion(point p, void* data)
{
	void** arr;
	arr = data;
	
	return pointInSolid(p,arr[0]) || pointInSolid(p,arr[1]);
}

int pointInDiff(point p, void* data)
{
	void** arr;
	arr = data;
	
	return pointInSolid(p,arr[0]) && !pointInSolid(p,arr[1]);
}

int segSolidCut(seg sg, void* sol, point* p, vec* normal)
{
	solid* s = (solid*)sol;
	
	return SEGCUT[s->type](sg,s->data,p,normal);
}

int segSolidCutAll(seg sg, void* sol, point p[], vec normal[])
{
	solid* s = (solid*)sol;
		
	return SEGCUTALL[s->type](sg,s->data,p,normal);
}

int segIntersectionCut(seg s, void* data, point* p, vec* normal)
{
	point p1[MAXCUTS], p2[MAXCUTS];
	vec n1[MAXCUTS],n2[MAXCUTS];
	solid *l,*r;
	int l1,l2;
	int c1,c2;
	
	l = *((solid**)data);
	r = *((solid**)data + 1);
	
	l1 = segSolidCutAll(s,l,p1,n1);
	l2 = segSolidCutAll(s,r,p2,n2);
	
	c1 = c2 = 0;
	
	while(c1<l1 && c2<l2)
	{
		if(fabs(pointDist2(s.p,p1[c1]) - pointDist2(s.p,p2[c2])) < EPS)
		{
			if(p != NULL) *p = p[c1];
			if(normal != NULL) *normal = n1[c1];
			return 3;
		} else if(pointDist2(s.p,p1[c1]) < pointDist2(s.p,p2[c2])){
			if(pointInSolid(p1[c1],r))
			{
				if(p != NULL) *p = p1[c1];
				if(normal != NULL) *normal = n1[c1];
				return 1;
			}
			c1++;
		} else {
			if(pointInSolid(p2[c2],l))
			{
				if(p != NULL) *p = p2[c2];
				if(normal != NULL) *normal = n2[c2];
				return 2;
			}
			c2++;
		}
	}
	
	while(c1<l1)
	{
		if(pointInSolid(p1[c1],r))
		{
			if(p != NULL) *p = p1[c1];
			if(normal != NULL) *normal = n1[c1];
			return 1;
		}
		c1++;
	}
	
	while(c2<l2)
	{
		if(pointInSolid(p2[c2],l))
		{
			if(p != NULL) *p = p2[c2];
			if(normal != NULL) *normal = n2[c2];
			return 2;
		}
		c2++;
	}
	
	return 0;
	
}		

int segIntersectionCutAll(seg s, void* data, point* p, vec* normal)
{
	point p1[MAXCUTS], p2[MAXCUTS];
	vec n1[MAXCUTS],n2[MAXCUTS];
	solid *l,*r;
	int l1,l2;
	int c1,c2;
	int c;
	
	l = *((solid**)data);
	r = *((solid**)data + 1);
	
	l1 = segSolidCutAll(s,l,p1,n1);
	l2 = segSolidCutAll(s,r,p2,n2);
	
	c1 = c2 = 0;
	c = 0;
	
	while(c1<l1 && c2<l2)
	{
		if(fabs(pointDist2(s.p,p1[c1]) - pointDist2(s.p,p2[c2])) < EPS)
		{
			if(p != NULL) p[c] = p1[c1];
			if(normal != NULL) normal[c] = n1[c1];
			c++;
			c1++;c2++;
		} else if(pointDist2(s.p,p1[c1]) < pointDist2(s.p,p2[c2])) {
			if(pointInSolid(p1[c1],r))
			{
				if(p != NULL) p[c] = p1[c1];
				if(normal != NULL) normal[c] = n1[c1];
				c++;
			}
			c1++;
		} else {
			if(pointInSolid(p2[c2],l))
			{
				if(p != NULL) p[c] = p2[c2];
				if(normal != NULL) normal[c] = n2[c2];
				c++;
			}
			c2++;
		}
	}
	
	while(c1<l1)
	{
		if(pointInSolid(p1[c1],r))
		{
			if(p != NULL) p[c] = p1[c1];
			if(normal != NULL) normal[c] = n1[c1];
			c++;
		}
		c1++;
	}
	
	while(c2<l2)
	{
		if(pointInSolid(p2[c2],l))
		{
			if(p != NULL) p[c] = p2[c2];
			if(normal != NULL) normal[c] = n2[c2];
			c++;
		}
		c2++;
	}
	
	return c;
	
}

int segUnionCut(seg s, void* data, point* p, vec* normal)
{
	point p1[MAXCUTS], p2[MAXCUTS];
	vec n1[MAXCUTS],n2[MAXCUTS];
	solid *l,*r;
	int l1,l2;
	int c1,c2;
	
	l = *((solid**)data);
	r = *((solid**)data + 1);
	
	l1 = segSolidCutAll(s,l,p1,n1);
	l2 = segSolidCutAll(s,r,p2,n2);
	
	c1 = c2 = 0;
	
	while(c1<l1 && c2<l2)
	{
		if(fabs(pointDist2(s.p,p1[c1]) - pointDist2(s.p,p2[c2])) < EPS)
		{
			if(p != NULL) *p = p[c1];
			if(normal != NULL) *normal = n1[c1];
			return 3;
		} else if(pointDist2(s.p,p1[c1]) < pointDist2(s.p,p2[c2])){
			if(!pointInSolid(p1[c1],r))
			{
				if(p != NULL) *p = p1[c1];
				if(normal != NULL) *normal = n1[c1];
				return 1;
			}
			c1++;
		} else {
			if(!pointInSolid(p2[c2],l))
			{
				if(p != NULL) *p = p2[c2];
				if(normal != NULL) *normal = n2[c2];
				return 2;
			}
			c2++;
		}
	}
	
	while(c1<l1)
	{
		if(!pointInSolid(p1[c1],r))
		{
			if(p != NULL) *p = p1[c1];
			if(normal != NULL) *normal = n1[c1];
			return 1;
		}
		c1++;
	}
	
	while(c2<l2)
	{
		if(!pointInSolid(p2[c2],l))
		{
			if(p != NULL) *p = p2[c2];
			if(normal != NULL) *normal = n2[c2];
			return 2;
		}
		c2++;
	}
	
	return 0;
	
}		

int segUnionCutAll(seg s, void* data, point* p, vec* normal)
{
	point p1[MAXCUTS], p2[MAXCUTS];
	vec n1[MAXCUTS],n2[MAXCUTS];
	solid *l,*r;
	int l1,l2;
	int c1 = 0,c2 = 0;
	int c = 0;
	
	l = *((solid**)data);
	r = *((solid**)data + 1);
	
	l1 = segSolidCutAll(s,l,p1,n1);
	l2 = segSolidCutAll(s,r,p2,n2);
	
	while(c1<l1 && c2<l2)
	{
		if(fabs(pointDist2(s.p,p1[c1]) - pointDist2(s.p,p2[c2])) < EPS)
		{
			if(p != NULL) p[c] = p1[c1];
			if(normal != NULL) normal[c] = n1[c1];
			c++;
			c1++;c2++;
		} else if(pointDist2(s.p,p1[c1]) < pointDist2(s.p,p2[c2])) {
			if(!pointInSolid(p1[c1],r))
			{
				if(p != NULL) p[c] = p1[c1];
				if(normal != NULL) normal[c] = n1[c1];
				c++;
			}
			c1++;
		} else {
			if(!pointInSolid(p2[c2],l))
			{
				if(p != NULL) p[c] = p2[c2];
				if(normal != NULL) normal[c] = n2[c2];
				c++;
			}
			c2++;
		}
	}
		
	while(c1<l1)
	{
		if(!pointInSolid(p1[c1],r))
		{
			if(p != NULL) p[c] = p1[c1];
			if(normal != NULL) normal[c] = n1[c1];
			c++;
		}
		c1++;
	}
	
	while(c2<l2)
	{
		if(!pointInSolid(p2[c2],l))
		{
			if(p != NULL) p[c] = p2[c2];
			if(normal != NULL) normal[c] = n2[c2];
			c++;
		}
		c2++;
	}
	
	return c;
}

int segDiffCut(seg s, void* data, point* p, vec* normal)
{
	point p1[MAXCUTS], p2[MAXCUTS];
	vec n1[MAXCUTS],n2[MAXCUTS];
	solid *l,*r;
	int l1,l2;
	int c1,c2;
	
	l = *((solid**)data);
	r = *((solid**)data + 1);
	
	l1 = segSolidCutAll(s,l,p1,n1);
	l2 = segSolidCutAll(s,r,p2,n2);
	
	c1 = c2 = 0;
	
	while(c1<l1 && c2<l2)
	{
		if(fabs(pointDist2(s.p,p1[c1]) - pointDist2(s.p,p2[c2])) < EPS)
		{
			if(p != NULL) *p = p[c1];
			if(normal != NULL) *normal = n1[c1];
			return 3;
		} else if(pointDist2(s.p,p1[c1]) < pointDist2(s.p,p2[c2])){
			if(!pointInSolid(p1[c1],r))
			{
				if(p != NULL) *p = p1[c1];
				if(normal != NULL) *normal = n1[c1];
				return 1;
			}
			c1++;
		} else {
			if(pointInSolid(p2[c2],l))
			{
				if(p != NULL) *p = p2[c2];
				if(normal != NULL) *normal = vecScale(-1,n2[c2]);
				return 2;
			}
			c2++;
		}
	}
	
	while(c1<l1)
	{
		if(!pointInSolid(p1[c1],r))
		{
			if(p != NULL) *p = p1[c1];
			if(normal != NULL) *normal = n1[c1];
			return 1;
		}
		c1++;
	}
	
	while(c2<l2)
	{
		if(pointInSolid(p2[c2],l))
		{
			if(p != NULL) *p = p2[c2];
			if(normal != NULL) *normal =  vecScale(-1,n2[c2]);
			return 2;
		}
		c2++;
	}
	
	return 0;
	
}		

int segDiffCutAll(seg s, void* data, point* p, vec* normal)
{
	point p1[MAXCUTS], p2[MAXCUTS];
	vec n1[MAXCUTS],n2[MAXCUTS];
	solid *l,*r;
	int l1,l2;
	int c1,c2;
	int c;
	
	l = *((solid**)data);
	r = *((solid**)data + 1);
	
	l1 = segSolidCutAll(s,l,p1,n1);
	l2 = segSolidCutAll(s,r,p2,n2);
	
	c1 = c2 = 0;
	c = 0;
	
	while(c1<l1 && c2<l2)
	{
		if(fabs(pointDist2(s.p,p1[c1]) - pointDist2(s.p,p2[c2])) < EPS)
		{
			if(p != NULL) p[c] = p1[c1];
			if(normal != NULL) normal[c] = n1[c1];
			c++;
			c1++;c2++;
		} else if(pointDist2(s.p,p1[c1]) < pointDist2(s.p,p2[c2])) {
			if(!pointInSolid(p1[c1],r))
			{
				if(p != NULL) p[c] = p1[c1];
				if(normal != NULL) normal[c] = n1[c1];
				c++;
			}
			c1++;
		} else {
			if(pointInSolid(p2[c2],l))
			{
				if(p != NULL) p[c] = p2[c2];
				if(normal != NULL) normal[c] =  vecScale(-1,n2[c2]);
				c++;
			}
			c2++;
		}
	}
	
	while(c1<l1)
	{
		if(!pointInSolid(p1[c1],r))
		{
			if(p != NULL) p[c] = p1[c1];
			if(normal != NULL) normal[c] = n1[c1];
			c++;
		}
		c1++;
	}
	
	while(c2<l2)
	{
		if(pointInSolid(p2[c2],l))
		{
			if(p != NULL) p[c] = p2[c2];
			if(normal != NULL) normal[c] =  vecScale(-1,n2[c2]);
			c++;
		}
		c2++;
	}
	
	return c;
	
}

