#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "geom.h"
#include "solids.h"
#include "color.h"
#include "trace.h"
#include "input.h"

colorf BLACK = {0,0,0};
colorf WHITE = {1,1,1};

vec reflect(vec ray, vec norm)
{
	return vecLinComb(1,ray,-2,projVec(ray,norm));
}

int fsign(CORD_TYPE x)
{
	if(x>0) return 1;
	if(x<0) return -1;
	return 0;
}

vec refract(vec ray, vec norm, CORD_TYPE rindexratio)
{
	vec ret;
	CORD_TYPE alpha;

	ret = vecScale(1/rindexratio,orthogonalize(ray,norm));

	/* busco alpha tal que |ret + alpha*norm| = |ray| */

	alpha = (vecModulus2(ray) - vecModulus2(ret))/vecModulus2(norm);

	if(alpha < EPS)
		return coords2vec(0,0,0);

	alpha = fsign(dotProduct(ray,norm)) * sqrt(alpha);

	ret = vecLinComb(1,ret,alpha,norm);
	return ret;
}

sceneinfo* loadSceneInfo()
{
	double scrxsize, scrysize;
	sceneinfo* scinfo = malloc(sizeof(sceneinfo));
	assert(scinfo != NULL);

	scinfo->resx = readInt();
	scinfo->resy = readInt();

	scinfo->camera.x = readDouble();
	scinfo->camera.y = readDouble();
	scinfo->camera.z = readDouble();
	
	scinfo->scrvec.x = readDouble();
	scinfo->scrvec.y = readDouble();
	scinfo->scrvec.z = readDouble();

	scrxsize= readDouble();
	scrysize= readDouble();

	scinfo->scrx = unit(crossProduct(scinfo->scrvec, coords2vec(0,1,0)));
	scinfo->scry = unit(crossProduct(scinfo->scrx, scinfo->scrvec));
	
	scinfo->scrx = vecScale(scrxsize * vecModulus(scinfo->scrvec), scinfo->scrx);
	scinfo->scry = vecScale(scrysize * vecModulus(scinfo->scrvec), scinfo->scry);

/* ahora calculamos scrx y scry */	
/*	scinfo->scrx.x = readDouble();
	scinfo->scrx.y = readDouble();
	scinfo->scrx.z = readDouble();
	
	scinfo->scry.x = readDouble();
	scinfo->scry.y = readDouble();
	scinfo->scry.z = readDouble();	*/
	
	scinfo->maxdepth = readInt();
	scinfo->rpp      = readInt();

	return scinfo;
}

scene* loadScene()
{
	scene* sc;
	int i;

	sc = (scene*)malloc(sizeof(scene));

	sc->ambient_light.x = readDouble();
	sc->ambient_light.y = readDouble();
	sc->ambient_light.z = readDouble();
	
	sc->Nlight = readInt();
	
	sc->light = malloc((sc->Nlight) * sizeof(light));

	for(i=0;i<sc->Nlight;++i)
	{
		sc->light[i].pos.x = readDouble();
		sc->light[i].pos.y = readDouble();
		sc->light[i].pos.z = readDouble();

		sc->light[i].col.x = readDouble();
		sc->light[i].col.y = readDouble();
		sc->light[i].col.z = readDouble();
		
		sc->light[i].aten_quad  = readDouble();
		sc->light[i].aten_lin   = readDouble();
		sc->light[i].aten_const = readDouble();
	}

	sc->Nobj = readInt();

	sc->obj = malloc((sc->Nobj) * sizeof(body));

	for(i=0;i<sc->Nobj;++i)
	{
		sc->obj[i].col_diff.x = readDouble();
		sc->obj[i].col_diff.y = readDouble();
		sc->obj[i].col_diff.z = readDouble();

		sc->obj[i].c_diff_ref   = readDouble();
		sc->obj[i].c_spec_ref   = readDouble();
		sc->obj[i].c_spec_trans = readDouble();
		sc->obj[i].refindex     = readDouble();
				
		sc->obj[i].shape = readSolid();
		assert(sc->obj[i].shape != NULL);
	}

	return sc;
}

void destroyScene(scene* sc)
{
	int i;
	
	free(sc->light);
	
	for(i=0;i<sc->Nobj;++i)
		freeSolid(sc->obj[i].shape);
	
	free(sc->obj);
	free(sc);
}

colorf freePath(const scene* sc, point start, point end)
{
	seg ray;
	int i;
	colorf ret = {1.0, 1.0, 1.0};

	ray = points2seg(start,end);
	ray.p = vecLinComb(1,ray.p,EPS,unit(seg2vec(ray)));

	for(i=0;i<sc->Nobj;++i)
	{
		if(segSolidCut(ray,sc->obj[i].shape,NULL,NULL))
			ret = colScale(sc->obj[i].c_spec_trans,colFilter(ret,sc->obj[i].col_diff));
			
		if(colModulus(ret) < EPS)
			return BLACK;
	}

	return ret;
}

colorf trace(const scene* sc, point start, vec dir, int depth, int obj, vec olddir, CORD_TYPE power)
{
	int i;
	point tp[50],bp;
	vec tn[50],bn;
	seg ray;
	int hitn = -1;
	colorf ret;
	colorf lighting;
	colorf l;
	CORD_TYPE c;
	vec dir_reflect;
	CORD_TYPE new_ref_index;
	int cutcant;
	int j;
		
	vec dir_refract;
	colorf reflect_col,refract_col,light_col;

	if(depth == 0)
		return BLACK;

	/* impide mala refraccion */
	if(vecModulus2(dir) < EPS)
		return BLACK;
		
	if(power < EPS)
		return BLACK;

	/* creamos el rayo */
	ray.p = start;
	ray.q = vecSum(start,dir);
	ray = extend(ray);

	for(i=0;i<sc->Nobj;++i)
	{
		if(pointDist(ray.q,start) < EPS) break;
		cutcant = segSolidCutAll(ray,sc->obj[i].shape,tp,tn);

		for(j=0;j<cutcant;++j)
		{
			if(pointDist(ray.p,tp[j]) > EPS)
			{
				bp = tp[j];
				bn = tn[j];
				hitn = i;
				
				ray.q = bp;
				
				break;
			}
			
			if((i>obj) && (dotProduct(dir,tn[j])*dotProduct(olddir,tn[j]) > 0))
			{
				bp = tp[j];
				bn = tn[j];
				hitn = i;
				
				ray.q = bp;
				depth++;
				
				goto a;
				break;
			}
		}
	}
	
	a:

	if(hitn == -1)
		return BLACK;

	/*
	return colScale(fabs(cosAngle(bn,dir)),sc->obj[hitn].col_diff);
	*/
	
	dir_reflect = reflect(dir,bn);

	if(dotProduct(dir,bn) > 0)
		new_ref_index = 1 / sc->obj[hitn].refindex;
	else
		new_ref_index = sc->obj[hitn].refindex;

	dir_refract = refract(dir,bn,new_ref_index);

	lighting = sc->ambient_light;

	for(i = 0;i < sc->Nlight;++i) /* calcular iluminacion del punto */
	{
		vec blinnDir;
		CORD_TYPE blinnVal;
		
		l = freePath(sc,bp,sc->light[i].pos);
		
		/* aca hay que cambiar para poner distintos tipos de luces */

		c =                                   sc->light[i].aten_const
		  + pointDist(bp,sc->light[i].pos)  * sc->light[i].aten_lin
		  + pointDist2(bp,sc->light[i].pos) * sc->light[i].aten_quad;

		
		blinnDir = vecSubtract(points2vec(sc->light[i].pos,bp),dir);
		if(vecModulus2(blinnDir) > EPS)
		{
			blinnDir = unit(blinnDir);
			blinnVal = fabs(dotProduct(blinnDir,bn)) * sc->obj[hitn].c_spec_ref;
		}  else blinnVal = 0;
		
		
		// blinnVal = 0;
		
		
		c = (1+blinnVal) * cosAngle(bn,points2vec(bp,sc->light[i].pos)) / c;
		
		lighting = colLinComb(1,lighting,fabs(c),colFilter(sc->light[i].col,l));
	}

	light_col = colScale(sc->obj[hitn].c_diff_ref,colFilter(lighting,sc->obj[hitn].col_diff));

	reflect_col = colScale(sc->obj[hitn].c_spec_ref,
					trace(sc,bp,dir_reflect,
						depth-1,hitn,
						dir,power*sc->obj[hitn].c_spec_ref));
						
	refract_col = colScale(sc->obj[hitn].c_spec_trans,
					trace(sc,bp,dir_refract,
						depth-1,hitn,
							dir,power*sc->obj[hitn].c_spec_trans));

	ret = colSum(light_col,colSum(reflect_col,refract_col));

	return ret;
}
