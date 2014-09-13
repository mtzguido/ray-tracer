COMO SE USA:
Ejecutar ./rtracer para instrucciones basicas.


Los archivos de entrada tienen el siguiente formato
RESX RESY
POS_CAMARA
VECTOR_CAMARA, un vector que apunta (desde POS_CAMARA) hacia el centro de la pantalla
PANTALLA_X, un vector que indica donde esta el horizontal de la pantalla
PANTALLA_Y, analogo
PROF_MAXIMA, los rayos refractados y reflejados solo se extienden hasta llegar a este valor (o al hacerse de infima potencia)
DETALLE, Cada pixel se partira en una cuadricula de DETALLE*DETALLE puntos y se tirará un rayo por cada uno, luego se promediarán los resultados. Mejora la calidad de imagen (anti aliasing)

LUZ_AMBIENTAL, un color (RGB, cada componente entre 0 y 1) que indica la luz que reciben todos los cuerpos de la escena

NLUCES, numero de luces

cada luz se define como
POS, un vector posicion
COLOR, un color RGB
FACTORES DE ATENUACION(a b c), la potencia de la luz será proporcional a 1/(a*d*d + b*d + c) siendo d la distancia desde la fuente de luz al punto afectado

NCUERPOS, cantidad de cuerpos

COLOR, el col del cuerpo
DIFF REFLEX TRANS REFINDEX, coef de reflexion difusa, coef de reflexion especular, coef de transmision, indice de refracción
NOMBRE_PRIMITIVA, una cadena, que debe estar en el array ID de solids.c
DATOS DE PRIMITA, cada primitiva lee distintos datos, para averiguar cómo describirla, referirse a su .c
 
