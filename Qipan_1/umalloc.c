#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "umalloc.h"

static char memory[10240];
struct block* freeList = NULL;

void initialize()
{
    freeList = (void *)memory;
    freeList->size=10240 - sizeof(struct block);              //可用空间大小
    freeList->free = 1;                                     //1：空闲 0：使用
    freeList->next=NULL;                                   //指向空
}

void split(struct block *fitting_slot,size_t size)
{
    struct block *new=(void*)((void*)fitting_slot+size+sizeof(struct block));   //定义new的地址
    new->size=(fitting_slot->size)-size-sizeof(struct block);                   //定义size大小
    new->free=1;                                                                //设置是否工作
    new->next=fitting_slot->next;                                               //独立出去，形成新的块
    new->prev = fitting_slot;
    fitting_slot->size=size;
    fitting_slot->free=0;
    fitting_slot->next=new;
}


void *umalloc(size_t noOfBytes, char *file, int line)
{
    struct block *curr,*prev;
    void *result;
    if(freeList == NULL)
    {
        initialize();
    }
    curr=freeList;
    while((((curr->size)<noOfBytes)||((curr->free)==0))&&(curr->next!=NULL))
    {
        prev=curr;
        curr=curr->next;
    }
    if((curr->size)==noOfBytes)
    {
        curr->free=0;
        result=(void*)(curr);
        return result;
    }
    else if((curr->size)>(noOfBytes+sizeof(struct block)))            //所需要的内存大小小于区块大小
    {
        split(curr,noOfBytes);                                        //分割区块函数
        result=(void*)(curr);                                       //使用的位置
        return result;
    }
    else
    {
        result=NULL;
        return result;
    }
}

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
    if (((void *) memory <= ptr) && (ptr <= (void *) (memory + 10240))) {
        struct block *curr = ptr;
        //curr--;
        curr->free = 1;
        //merge();
        if (curr->prev){
            if (curr->prev->free){
                curr = merge(curr->prev);
                //printf("merge at curr->prev\n");
            }
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
    else
        return;
}


