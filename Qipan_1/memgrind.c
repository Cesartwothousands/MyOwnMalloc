#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "umalloc.h"
#include <string.h>
#include <stdint.h>
#include <time.h>


void Consistency_test(){
    printf("0.Starting consistency test\n");
    for (int i = 0; i < 2; i++){
        char* ptr;
        ptr = (char*)malloc(2 * sizeof(char));
        if (ptr == NULL) {
            printf("Memory not allocated.\n");
            exit(0);
        }
        else{
            printf("Memory successfully allocated using malloc.\n");
            printf("Address of ptr: %p\n", ptr);
        }
        free(ptr);
    }
    printf("Consistency test succeed!\n");
    puts( "" );
}

unsigned long long Maximization_test(){
    printf("1.Starting maximization test\n");
    unsigned long long int max_allocation = 1;
    char* ptr = NULL;
    ptr = malloc(max_allocation);
    while(ptr != NULL){
        free(ptr);
        max_allocation *= 2;
        //printf("maximal  is: %d\n",max_allocation);
        ptr = malloc(max_allocation);
    }
    max_allocation /= 2;
    ptr = malloc(max_allocation);
    free(ptr);
    return max_allocation;
}

void BasicCoalescence_test(unsigned long long max_allocation){
    printf("2.Starting BasicCoalescence test\n");
    char* ptr1 = malloc(max_allocation/2);
    char* ptr2 = malloc(max_allocation/4);
    free(ptr1);
    free(ptr2);
    char* ptr3 = malloc(max_allocation);
    free(ptr3);
    printf("successful in BasicCoalescence_test\n");
    puts( "" );
}


void test345( size_t max )
{
    printf( "3. Starting Saturation test:\n" );
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
            break;
        }
        counter++;
    }
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
    puts( "Intermediate Coalescence succeeded\n" );
}


void ErrorDetection(){
    //  Free()ing addresses that are not pointers:
    int x;
    free((int*)x);

    //  Free()ing pointers that were not allocated by malloc():
    int * y;
    free(y);

    // Redundant free()ing of the same pointer:
    char* p = (char*)malloc(100);
    free(p);
    free(p);

}


int main(){
    printf("program starting\n");
    Consistency_test();

    unsigned long long int max_allocation = Maximization_test();
    printf("maximal allocation is: %llu\n",max_allocation);
    puts( "" );

    BasicCoalescence_test(max_allocation);
    test345(max_allocation);

    puts("Exception handling:");
    ErrorDetection();
    return 0;

}
