#include <time.h>

#include "umalloc.h"


void test0( void )
{
    // test 0
    char* q0 = NULL;
    printf( "0. Consistency:\n" );
    for ( int i = 1; i < 10; i++ )
    {
        q0 = malloc( i );
        for ( int j = 0; j < i; j++ )
        {
            q0[j] = ( j + 65 );
        }
        //the address will be different, because our heap is kind of up side down. 
        printf( "address: %p, value: ", q0 );
        puts( q0 );
        free( q0 );
    }
}

size_t test1( void )
{
    // test 1
    printf( "1. Maximization: (simple coalescence)\n" );
    char* arr = NULL;
    size_t size = 1;
    while ( true )
    {
        arr = malloc( size );
        if ( !arr )
        {
            printf( "failed on %ld bytes\n", size );
            arr = malloc( size /= 2 );
            if ( arr )
            {
                printf( "succeed on %ld bytes\n", size );
                free( arr );
                break;
            }
        }
        printf( "allocated %ld bytes of memory\n", size );
        //try if this is valid
        for ( size_t i = 0; i < size; i++ )
        {
            arr[i] = 0;
        }
        size = size * 2;
        free( arr );
    }
    return size;
}


void test2( size_t max )
{
    printf( "2. Basic Coalescence:\n" );
    size_t size = max / 2;
    char* arr0 = malloc( max );
    if ( !arr0 ) puts( "arr0 is NULL" );
    char* arr1 = malloc( size );
    if ( !arr1 )
    {
        puts( "arr1 is NULL" );
        printf( "%ld + %ld = %ld > %d\n", max, size, max + size, 10 * 1024 * 1024 );
    }
    free( arr0 );
    free( arr1 );
    char* arr2 = malloc( max );
    if ( !arr2 ) puts( "arr2 is NULL" );
    free( arr2 );
}


void test345( size_t max )
{
    printf( "3. Saturation:\n" );
    uintptr_t arr[9216] = { 0 };
    for ( size_t i = 0; i < 9216; i++ )
    {
        arr[i] = (uintptr_t) malloc( 1024 );
    }

    size_t counter = 0;
    uintptr_t arr_1b[2048] = { 0 };
    for ( size_t i = 0; i < 2048; i++ )
    {
        if ( ! (arr_1b[i] = (uintptr_t) malloc( 1 ) ) )
        {
            goto end;
        }
        counter++;
    }
    end: do{}while( false );
    size_t saturation = 9216 * 1024 + counter;
    printf( "saturation: %ld bytes\n", saturation );
    puts( "" );



    // test 4
    printf( "4. Time Overhead:\n" );
    free( (void*) arr_1b[counter - 1] );
    struct timespec start, end;
    clock_gettime( CLOCK_REALTIME, &start );
    malloc( 1 );
    clock_gettime( CLOCK_REALTIME, &end );
    printf( "running time: %lu micro second\n", ( end.tv_sec - start.tv_sec ) * 1000 + ( end.tv_nsec - start.tv_nsec ) / 1000000 );
    puts( "" );



    // test 5
    puts( "5. Intermediate Coalescence:" );
    for ( int i = counter - 1; i >= 0; i-- )
    {
        free( (void*) arr_1b[i] );
    }
    for ( int i = 9215; i >= 0; i-- )
    {
        free( (void*) arr[i] );
    }
    char* arr_q5 = malloc( max );
    free( arr_q5 );
    puts( "" );
}


int main( void )
{
    test0();
    puts( "" );

    size_t max = test1();
    puts( "" );
    
    test2( max );
    puts( "" );

    test345( max );
    puts( "" );

    return 0;
}