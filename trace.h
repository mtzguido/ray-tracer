#ifndef ___TRACE_H___
#define ___TRACE_H___

#include "geom.h"
#include "color.h"
#include "solids.h"

typedef struct
{
	solid* shape;
	CORD_TYPE c_spec_ref, c_spec_trans; /* para rayos hijo */
	CORD_TYPE c_diff_ref;
	colorf col_diff;
	CORD_TYPE refindex;
} body;

typedef struct
{
	point pos;
	colorf col;
	CORD_TYPE aten_const,aten_lin,aten_quad;
} light;

typedef struct
{
	int Nlight;
	light* light;
	int Nobj;
	body* obj;
	colorf ambient_light;
} scene;

typedef struct {
	int resx,resy;
	point camera;
	vec scrvec;
	vec scrx;
	vec scry;
	int maxdepth;
	int rpp;
} sceneinfo;

scene* loadScene(void);
sceneinfo* loadSceneInfo(void);
void destroyScene(scene*);
colorf trace(const scene* sc, point start, vec dir, int depth, int obj, vec olddir, CORD_TYPE power);

#endif
