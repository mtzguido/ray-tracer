#include "geom.h"
#include "color.h"

unsigned char min(int a, int b)
{
	return (a<b)?a:b;
}

unsigned char max(int a, int b)
{
	return (a>b)?a:b;
}

colorint colf2int(colorf c)
{
	colorint r;
	
	r.r = max(min(c.x*255,255),0);
	r.g = max(min(c.y*255,255),0);
	r.b = max(min(c.z*255,255),0);	

	return r;
}

colorf colLinComb(CORD_TYPE a, colorf c, CORD_TYPE b, colorf d)
{
	return vecLinComb(a,c,b,d);
}

CORD_TYPE colModulus(colorf c)
{
	return c.x*c.x + c.y*c.y + c.z*c.z;
}

colorf colFilter(colorf c, colorf filter)
{
	colorf ret;
	
	ret.x = c.x * filter.x;
	ret.y = c.y * filter.y;
	ret.z = c.z * filter.z;
	
	return ret;
}

colorf colScale(CORD_TYPE a, colorf col)
{
	return vecScale(a,col);
}

colorf colSum(colorf a, colorf b)
{
	return vecSum(a,b);
}
