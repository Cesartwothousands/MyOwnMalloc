CC = gcc
CFLAG = -Wall -Wextra -pedantic -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -g -Og -ggdb -fsanitize=undefined,leak,bounds
OBJ = umalloc.o

all:
	make umalloc memgrind

umalloc: umalloc.c umalloc.h Makefile
	$(CC) $(CFLAG) $< -c

memgrind: memgrind.c umalloc.c umalloc.h Makefile
	$(CC) $(CFLAG) $(OBJ) $< -o $@

clean: 
	rm -f memgrind *.o
