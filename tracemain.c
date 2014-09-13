#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "input.h"
#include "bmp.h"
#include "trace.h"

#define NTHR 12

colorint* img;
sceneinfo* scinfo;
scene* sc;
pthread_mutex_t lk = PTHREAD_MUTEX_INITIALIZER;

int	globalx;

void usage()
{
	printf("Uso: rtracer archivo_de_salida\n");
	printf("La salida es exportada en mapa de bits.\n");
	printf("La escena se lee desde la entrada estandar.\n");
}

colorint calcPix(const int x, const int y)
{
	int i,j;
	vec dirbase;
	vec dir;
	colorf col = {0, 0, 0};

	dirbase = vecScale(-0.5 + 1.0/(2*scinfo->resx) + (double)x/scinfo->resx, scinfo->scrx);
	dirbase = vecSum(dirbase,vecScale(-0.5 + 1.0/(2*scinfo->resy) + (double)y/scinfo->resy,scinfo->scry));
	dirbase = vecSum(dirbase,scinfo->scrvec);

	for(i=0; i<scinfo->rpp; ++i)
		for(j=0; j<scinfo->rpp; ++j)
		{
			dir = dirbase;
			dir = vecLinComb(1.0, dir, -0.5 + 1.0/(2*scinfo->rpp) + (double)(i)/scinfo->rpp, vecScale(1.0/scinfo->resx, scinfo->scrx));
			dir = vecLinComb(1.0, dir, -0.5 + 1.0/(2*scinfo->rpp) + (double)(j)/scinfo->rpp, vecScale(1.0/scinfo->resy, scinfo->scry));

			col = colSum(col, trace(sc, scinfo->camera, dir, scinfo->maxdepth, -1, coords2vec(0,0,0), 1));
		}

	col =  colScale(1.0/(scinfo->rpp*scinfo->rpp), col);
	return colf2int(col);
}

void *thread(void* arg)
{
	int x,y;
	int me = *(int*)arg;

	while(1) {
		pthread_mutex_lock(&lk);
		x = globalx;
		globalx++;
		pthread_mutex_unlock(&lk);

		if(x >= scinfo->resx)
			return;

		for(y=0; y<scinfo->resy; ++y)
			img[y*scinfo->resx+x] = calcPix(x,y);
	}
}

int main(int argc, char** argv)
{
	pthread_t th[NTHR];
	int arg[NTHR];
	int i;

	if(argc != 2)
	{
		usage();
		exit(1);
	}

	scinfo = loadSceneInfo();
	sc = loadScene();

	img = malloc(sizeof(colorint) * scinfo->resx * scinfo->resy);

	printf("Starting...\n");
	globalx = 0;

	for(i=0; i<NTHR; ++i)
	{
		arg[i] = i;
		assert(0 == pthread_create(&th[i], NULL, thread, &arg[i]));
	}

	for(i=0; i<NTHR; ++i)
		pthread_join(th[i], NULL);
	
	bmpWrite(argv[1], img, scinfo->resx, scinfo->resy);

	free(img);
	destroyScene(sc);

	return 0;
}
