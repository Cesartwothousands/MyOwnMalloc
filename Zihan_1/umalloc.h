#include <stdlib.h>


#define malloc(x) umalloc(x,__FILE__, __LINE__)
#define free(x) ufree(x,__FILE__, __LINE__)

#pragma pack(1)
typedef struct metadata {
	short int inUse;
	short int size;
	struct metadata *next;  

} metadata; 

void *umalloc(size_t size, char *file, int line);
void ufree(void* ptr, char* file, int line);

