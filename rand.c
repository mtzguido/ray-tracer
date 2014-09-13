#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "geom.h"

int main()
{
	vec x;
	srand(time(NULL) + getpid());

	x = coords2vec(randomVal(), randomVal(), randomVal());
	printVec(vecScale(1/vecModulus(x), x));
	return 0;
}

