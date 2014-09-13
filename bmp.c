#include "bmp.h"
#include "color.h"
#include <fcntl.h>
#include <stdio.h>

int bmpWrite(char* path, colorint* arr, int width, int height)
{
	int i,j;
	int filesize;
	char buff[4];
	int pad;
	FILE* file;
	file = fopen(path,"wb");

	if(file == NULL)
		return 1;

	if ((width*3)%4 == 0)
		pad = 0;
	else
		pad = 4 - ((width*3)%4);

	filesize = 54 + height*(3*width + pad);

	fwrite("BM",1,2,file); /* BM obligatorio */

	buff[0]=filesize%256;
	buff[1]=filesize>>8%256;
	buff[2]=filesize>>16%256;
	buff[3]=filesize>>24%256;
	fwrite(buff,1,4,file);

	/* Se escriben los 4 reservados */
	buff[0]=0;
	buff[1]=0;
	buff[2]=0;
	buff[3]=0;
	fwrite(buff,1,4,file);

	/* Se escribe el 54 y los 3 vacios */
	buff[0] = 54;
	buff[1] = 0;
	buff[2] = 0;
	buff[3] = 0;
	fwrite(buff,1,4,file);

	/* Se escribe el 40 y los 3 vacios */
	buff[0] = 40;
	buff[1] = 0;
	buff[2] = 0;
	buff[3] = 0;
	fwrite(buff,1,4,file);

	/* Se escriben la anchura y la altura(MAL) */

	buff[0]=width%256;
	buff[1]=width>>8%256;
	buff[2]=width>>16%256;
	buff[3]=width>>24%256;
	fwrite(buff,1,4,file);

	buff[0]=height%256;
	buff[1]=height>>8%256;
	buff[2]=height>>16%256;
	buff[3]=height>>24%256;
	fwrite(buff,1,4,file);

	buff[0] = 1;
	buff[1] = 0;
	buff[2] = 24;
	buff[3] = 0;
	fwrite(buff,1,4,file);

	/* Se escriben 4 bytes por la no compresion
	 * Se escribe despues el tamaño de la imagen de 4 bytes (?) (le chupa un huevo, lo clavamos en cero y vemos si se rompe)
	 * agregamos otros 16 bytes de mierda que le chupan un huevo (Si no es asi, lo cambiamos)
	 */

	buff[0] = 0;
	buff[1] = 0;
	buff[2] = 0;
	buff[3] = 0;

	fwrite(buff,1,4,file);
	fwrite(buff,1,4,file);
	fwrite(buff,1,4,file);
	fwrite(buff,1,4,file);
	fwrite(buff,1,4,file);
	fwrite(buff,1,4,file);

	for(j = 0; j<height; ++j) {
		for(i = 0; i<width; ++i)
			fwrite(arr + j*width+i,1,3,file);
		for(i = 0; i<pad; ++i)
			fwrite(arr, 1, 1, file);
	}

	
	return 0;
}


