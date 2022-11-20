#include "umalloc.h"


// "heap" size: 10 * 1024 * 1024
#define BUFF_SIZE 10485760


//struct
typedef struct pointer
{
    //max for 24 bits unsigned int is 16777216. 
    uintptr_t ptr;
    uint32_t size : 24;
}pointer;

typedef struct metadata
{
    size_t size;
    pointer* data;
}metadata;




//global variable
static char buff[BUFF_SIZE] = { 0 };
static uintptr_t end = 0;
static metadata* meta = NULL;





//function
static inline int cmp( const void* pointer1, const void* pointer2 )
{
    pointer* ptr1 = (pointer*) pointer1;
    pointer* ptr2 = (pointer*) pointer2;

    return - ( ptr1->ptr - ptr2->ptr );
}


static inline uintptr_t get_meta_end( void )
{
    uintptr_t temp = (uintptr_t) &buff;
    temp += sizeof (metadata);
    temp += sizeof (pointer) * meta->size;
    return temp;
}


void leak_sanitizer( void )
{
    //print leak info when exit
    if ( meta->size )
    {
        size_t size = 0;
        printf( "\n[WARNING]: detected memory leaks\n" );
        for ( size_t i = 0; i < meta->size; i++ )
        {
            printf( "#%ld 0x%lx: %d byte(s)\n", i, meta->data[i].ptr, meta->data[i].size );
            size += meta->data[i].size;
        }
        printf( "SUMMARY: Leak: %ld byte(s) leaked in %ld allocation(s)\n", size, meta->size );
    }
}


void* umalloc( size_t size, char* file, int line )
{
    //initlize metadata
    static bool init = false;
    if ( !init )
    {
        init = true;
        meta = (metadata*) &buff;
        meta->size = 0;
        meta->data = (pointer*) ( (char*) meta + sizeof( metadata ) );
        end = ( ( uintptr_t ) &buff ) + BUFF_SIZE;
        atexit( leak_sanitizer );
        //printf( "start: 0x%p, end: 0x%lx\n", (void*) &buff, end );
    }


    //allocate memory
    uintptr_t temp = end - size;
    uintptr_t last = end;

    //first allocate or all freed
    if ( meta->size == 0 && ( ( get_meta_end() + size ) < end ) )
    {
        meta->data[ meta->size ] = (pointer)
        {
            .ptr = temp,
            .size = size,
        };
        meta->size++;
        return (void*) temp;
    }

    //find free space
    for ( size_t i = 0; i < meta->size; i++ )
    {
        //found a free space, return the ptr. 
        if ( ( temp + size ) <= last && temp > (uintptr_t) ( meta->data[i].ptr + meta->data[i].size ) )
        {
            meta->data[ meta->size ] = (pointer)
            {
                .ptr = temp,
                .size = size,
            };
            meta->size++;
            //sort metadata
            qsort( meta->data, meta->size, sizeof (pointer), cmp );
            return (void*) temp;
        }
        last = meta->data[i].ptr;
        temp = meta->data[i].ptr - size;
    }

    //didn't find free space between already allocated spaces

    //if don't have enough memory
    if ( get_meta_end() + size >= meta->data[ meta->size -1 ].ptr || size >= BUFF_SIZE )
    {
        printf( "[ERROR]: malloc: Out of memory. File: %s: %d\n", file, line );
        //printf( "0x%lx, 0x%lx\n", get_meta_end(), meta->data[ meta->size -1 ].ptr );
        temp = 0;
    }

    //if have enough memory
    else
    {
        temp = meta->data[ meta->size -1 ].ptr - size;
        meta->data[ meta->size ] = (pointer)
        {
            .ptr = temp,
            .size = size,
        };
        meta->size++;
    }
    return (void*) temp;
}

void ufree( void* ptr, char* file, int line )
{
    uintptr_t temp = (uintptr_t) ptr;
    //ignore NULL just like free
    if ( !temp )
    {
        return;
    }
    //find ptr from metadata
    if ( !meta->size )
    {
        printf( "[ERROR]: free: Invalid ptr. File: %s: %d\n", file, line );
        return;
    }
    else if ( meta->size == 1 )
    {
        meta->size--;
        return;
    }
    for ( size_t i = 0; i < meta->size; i++ )
    {
        if ( temp == meta->data[i].ptr )
        {
            meta->data[i] = meta->data[ --meta->size ];
            qsort( meta->data, meta->size, sizeof (pointer), cmp );
            return;
        }
    }
    printf( "[ERROR]: free: Invalid ptr. File: %s: %d\n", file, line );
}