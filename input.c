#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

void digest()
{
	int c;

	/* consumimos hasta algun caracter relevante */
	while((c=getchar()) != EOF && isspace(c));
	
	/* si es un comentario */
	if(c == '#')
	{
		while((c=getchar()) != '\n' && c != EOF);
		digest();
	} else ungetc(c,stdin);
}

void exitwerr(char* errdesc, ...)
{
	va_list vl;
	va_start(vl,errdesc);
	
	vfprintf(stderr,errdesc,vl);
	va_end(vl);
	
	exit(1);
}

int readInt()
{
	int x;

	digest();

	if(scanf("%i",&x) != 1)
		exitwerr("Entrada mal formada (Se esperaba un int en pos %li).\n",ftell(stdin));
		
	return x;
}

double readDouble()
{
	double x;

	digest();

	if(scanf("%lf",&x) != 1)
		exitwerr("Entrada mal formada (Se esperaba un double en pos %li).\n",ftell(stdin));
	
	return x;
}
