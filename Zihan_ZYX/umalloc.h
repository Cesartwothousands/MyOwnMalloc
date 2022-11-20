#ifndef __UMALLOC_H__
#define __UMALLOC_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


#define SIZEOFMEM 10240*1024

//function
void* umalloc( size_t size, char* file, int line );
void ufree( void* ptr, char* file, int line );




//macro
#define malloc( size ) umalloc( size, __FILE__, __LINE__ )
#define free( ptr ) ufree( ptr, __FILE__, __LINE__ ) 


#endif 