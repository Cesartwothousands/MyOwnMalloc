all:memgrind errordetection

memgrind: memgrind.o umalloc.o
	gcc memgrind.o umalloc.o -o memgrind

memgrind.o: memgrind.c umalloc.h
	gcc -c memgrind.c 

errordetection: errordetection.o umalloc.o
	gcc errordetection.o umalloc.o -o errordetection

errordetection.o: errordetection.c umalloc.h
	gcc -c errordetection.c 

umalloc.o: umalloc.c umalloc.h
	gcc -c umalloc.c

clean: 
	rm -f memgrind *.o
