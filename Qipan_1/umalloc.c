#include <stddef.h>
#include <stdio.h>
#include "umalloc.h"

static char memory[MEMSIZE];
struct block* freeList = NULL;


// initialization of memory
void initialize()
{
    freeList = (void *)memory;
    // memory available
    freeList->size=MEMSIZE - sizeof(struct block);
    // set the block of memory to free
    freeList->free = 1;
    freeList->next=NULL;
}


/*  a block into two parts, if the requested block
    has smaller capacity than the fitting slot  */
void split(struct block *fitting_slot,size_t size)
{
    // the address for new free block
    struct block *new = (void*)((void*)fitting_slot + size + sizeof(struct block));
    new->size = (fitting_slot->size) - size - sizeof(struct block);
    new->free = 1;
    // similar to 'insert' a free block in double linked list
    new->next = fitting_slot->next;
    new->prev = fitting_slot;
    if (new->next)
        new->next->prev = new;
    // the allocated block
    fitting_slot->size = size;
    fitting_slot->free = 0;
    fitting_slot->next = new;
}


void *umalloc(size_t noOfBytes, char *file, int line)
{
    struct block *curr,*prev;
    void *result;
    // check the memory is initialized or not
    if(freeList == NULL)
    {
        initialize();
    }
    curr=freeList;

    // find the block
    while((((curr->size) < noOfBytes) ||
    ((curr->free) == 0)) && (curr->next != NULL))
    {
        prev = curr;
        curr = curr->next;
    }
    // the size we find equals to the requested bytes exactly
    if((curr->size) == noOfBytes)
    {
        curr->free = 0;
        result = (void*)(curr);
        return result;
    }
    // the size of free block is larger than we request, split it
    else if((curr->size) > (noOfBytes+sizeof(struct block)))
    {
        split(curr,noOfBytes);
        result = (void*)(curr);
        return result;
    }
    else
    {
        if (noOfBytes > MEMSIZE - sizeof(struct block)){
            printf("there is no free memory\n  "
                   "file:%s\n  line:%d\n", file,line);
        }
        else{
            printf("there is no enough memory available at"
                   "file:%s\n  line:%d\n", file,line);
        }
        result = NULL;
        return result;
    }
}


// merge two adjacent blocks
struct block* merge(struct block* b)
{
    if((b->free) && (b->next->free)){
        b->size += (b->next->size) + sizeof(struct block);
        b->next = b->next->next;
        if (b->next){
            b->next->prev = b;
        }
    }
    return b;
}


void ufree(void* ptr, char* file, int line) {
    if(ptr == NULL) {
        printf("Error on free(): Free()ing addresses that are not pointers\n  file:%s\n  line:%d\n", file, line);
        return;
    }

    // check if the address of ptr is legal
    if (((void *) memory <= ptr) && (ptr <= (void *) (memory + MEMSIZE))) {
        struct block *curr = ptr;
        // check if it is double freed
        if (curr->free == 1){
            printf("Error on free(): "
                   "Redundant free()ing of the same pointer\n  "
                   "file:%s\n  line:%d\n", file, line);
            return;
        }
        curr->free = 1;
        // merge adjacent free blocks
        if (curr->prev && curr->prev->free){
            curr = merge(curr->prev);
            //printf("merge at curr->prev\n");
        }
        if (curr->next){
            merge(curr);
            //printf("merge at curr->next\n");
        }
        else {
            if (curr->prev)
                curr->prev->prev = NULL;
            else
                freeList = NULL;
        }
    }
    else{
        printf("Error on free(): Invalid call to free, "
               "pointer was never allocated\n  "
               "file:%s\n  line:%d\n", file, line);
        return;
    }
}


