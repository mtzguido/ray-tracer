#include "geom.h"
#include "common.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

inline seg points2seg(point a, point b)
{
	seg s;
	s.p = a;
	s.q = b;
	
	return s;
}

inline vec seg2vec(seg s)
{
	vec v;
	v.x = s.q.x - s.p.x;
	v.y = s.q.y - s.p.y;
	v.z = s.q.z - s.p.z;
	
	return v;
}

inline vec points2vec(point a, point b)
{
	vec v;
	
	v.x = b.x - a.x;
	v.y = b.y - a.y;
	v.z = b.z - a.z;
	
	return v;
}

inline vec vecSubtract(vec v, vec u)
{
	v.x = v.x - u.x;
	v.y = v.y - u.y;
	v.z = v.z - u.z;
	
	return v;
}

inline CORD_TYPE randomVal()
{
	CORD_TYPE x;
	x = (rand()%32768) - 16384;
	x = x/32768.0;

	return (CORD_TYPE)x;
}

inline void printVec(vec v)
{
	printf("%f %f %f\n",v.x,v.y,v.z);
}

inline vec coords2vec(CORD_TYPE x,CORD_TYPE y,CORD_TYPE z)
{
	vec v;
	v.x = x;
	v.y = y;
	v.z = z;
	
	return v;
}

inline vec vecSum(vec u, vec v)
{
	u.x = u.x + v.x;
	u.y = u.y + v.y;
	u.z = u.z + v.z;
	
	return u;
}

inline vec vecScale(CORD_TYPE a, vec u)
{
	u.x = u.x * a;
	u.y = u.y * a;
	u.z = u.z * a;
	
	return u;
}

inline vec vecLinComb(CORD_TYPE a, vec u, CORD_TYPE b, vec v)
{
	u.x = a * u.x + b * v.x;
	u.y = a * u.y + b * v.y;
	u.z = a * u.z + b * v.z;

	return u;
}

inline CORD_TYPE dotProduct(vec u, vec v)
{
	return u.x*v.x + u.y*v.y + u.z*v.z;
}

inline vec crossProduct(vec u, vec v)
{
	vec w;
	
	w.x = u.y * v.z - u.z * v.y;
	w.y = u.z * v.x - u.x * v.z;
	w.z = u.x * v.y - u.y * v.x;

	return w;
}

inline CORD_TYPE tripleProduct(vec u, vec v, vec w)
{
	CORD_TYPE ret;
	
	ret = u.x*v.y*w.z
	     -u.x*v.z*w.y
	     -u.y*v.x*w.z
	     +u.y*v.z*w.x
	     +u.z*v.x*w.y
	     -u.z*v.y*w.x;

	return ret;
}

inline CORD_TYPE vecModulus(vec v)
{
	return (CORD_TYPE)sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

inline CORD_TYPE vecModulus2(vec v)
{
	return v.x*v.x + v.y*v.y + v.z*v.z;
}

inline CORD_TYPE segLength(seg s)
{
	return vecModulus(seg2vec(s));
}

inline CORD_TYPE pointDist(point a, point b)
{
	return vecModulus(points2vec(a,b));
}

inline CORD_TYPE pointDist2(point a, point b)
{
	return vecModulus2(points2vec(a,b));
}

inline vec unit(vec v)
{
	return vecScale(1/vecModulus(v),v);
}

inline seg extend(seg s)
{
	vec diff;
	
	diff = seg2vec(s);
	diff = unit(diff);
	
	diff = vecScale(4*MAXDIST,diff);
	
	s.q = vecSum(s.p,diff);
	
	return s;
}
	
inline vec projVec(vec u, vec v) /* u sobre v */
{
	CORD_TYPE k;
	
	k = dotProduct(u,v) / dotProduct(v,v);

	return vecScale(k,v);
}

inline vec orthogonalize(vec u, vec v)
{
	/* assert(fabs(dotProduct(v,vecLinComb(1,u,-1,projVec(u,v)))) < EPS); */
	return vecLinComb(1,u,-1,projVec(u,v));
}

inline vec orthoRand(vec v) /* devuelve un VERSOR ortogonal al azar */
{
	/* static int mostrado = 0; */
	vec x;
	CORD_TYPE m;
	x = coords2vec(randomVal(),randomVal(),randomVal());

	x = crossProduct(v,x);
	m = vecModulus(x);
	
	if(m<EPS)
	{
		/*if(mostrado == 0)
		{
			printf("Sirvió el check del orthoRand!\n");
			mostrado = 1;
		}*/
		return orthoRand(v);
	}
	else
		return vecScale(1/m,x);
}

inline int coplanar(point a, point b, point c, point d)
{
	vec u,v,w;

	u = points2vec(a,b);
	v = points2vec(a,c);
	w = points2vec(a,d);

	return (fabs(tripleProduct(u,v,w)) < EPS);
}

inline int segsIntersect(seg s, seg r) /* sin incluir vertices */
{
	vec u,v,w;
	
	if(!coplanar(s.p,s.q,r.p,r.q)) return 0;

	u = points2vec(s.p,s.q);
	v = points2vec(s.p,r.p);
	w = points2vec(s.p,r.q);
	
	v = crossProduct(u,v);
	w = crossProduct(u,w);

	if(dotProduct(v,w) > EPS) return 0;

	u = points2vec(r.p,r.q);
	v = points2vec(r.p,s.p);
	w = points2vec(r.p,s.q);

	v = crossProduct(u,v);
	w = crossProduct(u,w);

	if(dotProduct(v,w) > EPS) return 0;

	return 1;
}

inline int pointInSeg(point p, seg s) /* mejorar */
{
	if(vecModulus2(crossProduct(seg2vec(s),points2vec(p,s.p))) > EPS)
		return 0;
	
	if((p.x - s.p.x) * (p.x - s.q.x) > 0) return 0;
	if((p.y - s.p.y) * (p.y - s.q.y) > 0) return 0;
	if((p.z - s.p.z) * (p.z - s.q.z) > 0) return 0;
	
	return 1;
}

inline point project2Plane(point p, point a, point b, point c)
{
	vec v;

	v = crossProduct(points2vec(a,b),points2vec(a,c));
	p = points2vec(a,p);
	p = orthogonalize(p,v);

	return vecSum(a,p);
}

inline CORD_TYPE distPoint2Plane(point p, point a, point b, point c)
{
	vec v;
	
	v = crossProduct(points2vec(a,b),points2vec(a,c));
	
	return fabs(tripleProduct(points2vec(a,p),points2vec(a,b),points2vec(a,c)) / vecModulus(v));
}


inline CORD_TYPE distPoint2Line(point p, point a, point b)
{
	return sqrt(vecModulus2(crossProduct(points2vec(a,b),points2vec(a,p))) / vecModulus2(points2vec(a,b)));
}

inline CORD_TYPE cosAngle(vec u, vec v)
{
	CORD_TYPE ans;
	
	ans = dotProduct(u,v);
	ans = ans / (vecModulus(u) * vecModulus(v));
	
	return ans;
}
