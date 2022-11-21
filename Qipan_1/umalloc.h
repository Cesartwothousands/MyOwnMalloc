#include <stdlib.h>
#include <stdio.h>
#define malloc(x) umalloc(x,__FILE__, __LINE__)
#define free(x) ufree(x,__FILE__, __LINE__)
#define MEMSIZE 10485760

#pragma pack(1)

struct block{
    size_t size;    //  size of the block
    int free;       //  free:1 occupied:0
    struct block *next;  // pointer to next block
    struct block *prev;  // pointer to previous block
};

void *umalloc(size_t size, char *file, int line);
void ufree(void* ptr, char* file, int line);

