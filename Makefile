.PHONY:clean all bench re
CFLAGS=-O99 -Wall -pedantic
SHELL=/bin/bash

mods=bmp color cyl geom input plane poly solids sphere tile trace tracemain
objs=$(patsubst %,%.o,$(mods))

all: rtracer gen rand

bench: all
	time ./rtracer bench.bmp <in/wall

rtracer: $(objs)
	gcc $(objs) -lm -lpthread -o rtracer

gen: gen.c
	gcc ${CFLAGS} gen.c -o gen

rand: rand.c geom.h geom.c
	gcc ${CFLAGS} rand.c geom.c -lm -o rand

clean:
	rm -f rtracer gen rand $(objs)

re: clean all

%.o: %.c $(wildcard *.h)
	gcc ${CFLAGS} -c $<	-o $@
