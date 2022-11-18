#include <stdlib.h>
#include <stdio.h>
#define malloc(x) umalloc(x,__FILE__, __LINE__)
#define free(x) ufree(x,__FILE__, __LINE__)

#pragma pack(1)

struct block{
    size_t size;                  //区块大小
    int free;                     //是否已使用
    struct block *next;          //指向下一个区块
    struct block *prev;
};

void *umalloc(size_t size, char *file, int line);
void ufree(void* ptr, char* file, int line);

