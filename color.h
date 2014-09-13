#ifndef ___COLOR_H___
#define ___COLOR_H___

#include "common.h"
#include "geom.h"

typedef struct {
	unsigned char b;
	unsigned char g;
	unsigned char r;
} colorint;

typedef vec colorf;

colorint colf2int(colorf c);
colorf colLinComb(CORD_TYPE a, colorf c, CORD_TYPE b, colorf d);
CORD_TYPE colModulus(colorf c);
colorf colFilter(colorf c, colorf filter);
colorf colScale(CORD_TYPE a, colorf col);
colorf colSum(colorf a, colorf b);

#endif
