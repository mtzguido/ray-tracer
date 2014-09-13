#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

double r()
{
	return (double)rand() / RAND_MAX;
}

double r2()
{
	return ((double) 2*rand())/RAND_MAX - 1;
}

int main(int argc, char **argv)
{
	int i;
	double x,y,z,ra;
	int c;

	if(argc < 2) {
		printf("1 Argument required\n");
		exit(1);
	}

	c = atoi(argv[1]);
	srand(time(NULL) + getpid());
	
	for (i=0; i<c; ++i) {
		x = 500* r2();
		y = 500* r2();
		z = 500* r2();
		ra = 20*r() + 1;
	
		if(x*x + y*y + z*z <= 2*ra*ra) { i--; continue; }

		printf("%f %f %f 0.2 1 0 1 SPHERE %f %f %f %f\n", r(), r(), r(),
		        x,y,z,ra);
	}

	return 0;
}

