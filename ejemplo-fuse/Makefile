gragefs : gragefs.o grage-log.o
	gcc -g `pkg-config fuse --libs` -o gragefs gragefs.o grage-log.o

gragefs.o : gragefs.c grage-log.h grage-params.h
	gcc -g -Wall `pkg-config fuse --cflags` -c gragefs.c

grage-log.o : grage-log.c grage-log.h grage-params.h
	gcc -g -Wall `pkg-config fuse --cflags` -c grage-log.c

clean:
	rm -f gragefs *.o
