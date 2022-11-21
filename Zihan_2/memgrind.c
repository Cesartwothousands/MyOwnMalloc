#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "umalloc.h"
#include <string.h>


void Consistency_test(){
    for (int i = 0; i < 2; i++){
        char* ptr;
        ptr = (char*)malloc(2 * sizeof(char));
        if (ptr == NULL) {
            printf("Memory not allocated.\n");
            exit(0);
        }
        else{
            printf("Small block %d successfully allocated using malloc.\n", i + 1);
            printf("Address of ptr: %p\n", ptr);
        }
        free(ptr);
    }
}

int Maximization_test(){
    int max_allocation = 1;
    char* ptr=NULL;
    ptr = malloc(max_allocation);
    while(ptr!=NULL){
        free(ptr);
        printf("Allocate %d B\n",max_allocation);
        max_allocation*=2;
        ptr = malloc(max_allocation);
    }
    max_allocation/=2;
    ptr = malloc(max_allocation);
    free(ptr);
    return max_allocation;
}

void BasicCoalescence_test(int max_allocation){
    char* ptr1 = malloc(max_allocation/2);
    free(ptr1);
    printf("Allocate max_allocation/2: %d B and free it\n",max_allocation/2);
    char* ptr2 = malloc(max_allocation/4);
    free(ptr2);
    printf("Allocate max_allocation/4: %d B and free it\n",max_allocation/4);
    char* ptr3 = malloc(max_allocation);
    free(ptr3);
    printf("Allocate max_allocation: %d B and free it\n",max_allocation);
}

void ErrorDetection(){
    printf("\nStart the ErrorDetection test:\n\n");
    //  Free()ing addresses that are not pointers:
    int x;
    free( (int*)x );

    //  Free()ing pointers that were not allocated by malloc():
    int * y;
    free( y );

    // Redundant free()ing of the same pointer:
    int *p = (char*)malloc(100);
    free(p);
    free(p);

    printf("\nNormal_test:\n");
    p = (char *)malloc( 100 );
    free( p );
    p = (char *)malloc( 100 );
    free( p );    
    printf("Normal_test passed!");

}

int main(){
    printf("\nStart the test of our project\n\n");

    printf("\n0. Consistency_test:\n\n");
    Consistency_test();
    printf("Consistency_test passed!\n\n");

    printf("\n1. Maximization_test:\n\n");    
    int max_allocation = Maximization_test();
    printf("Maximal allocation is: %d B\n",max_allocation);
    printf("Maximization_test passed!\n\n");

    BasicCoalescence_test(max_allocation);
    printf("BasicCoalescence_test passed!\n\n");
   

    // ******* Saturation test *********
    char *ptrs[102400] = {0};
    int allocationsNum = 0;

    ptrs[allocationsNum] = malloc(1024);
    while(ptrs[allocationsNum]!=NULL){
        //printf("%d %d \n",ptrs[allocationsNum], allocationsNum);
        allocationsNum++;
        if(allocationsNum<9216){
            ptrs[allocationsNum] = malloc(1024);
        }
        else{
            ptrs[allocationsNum] = malloc(1);
        }
    }

    printf("maximal number of allocations is: %d\n",allocationsNum - 9215);
    printf("Saturation_test passed!\n\n");


    // ******* Time Overhead test *********
    allocationsNum--;
    free(ptrs[allocationsNum]);

    struct timeval start,end;
    gettimeofday(&start, NULL);
    ptrs[allocationsNum] = malloc(1);
    gettimeofday(&end, NULL);

    long timeuse =1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;  
    printf("Cost time: %fs\n",timeuse /1000000.0);  

    printf("Time_overhead passed!\n\n");


    // ******* Intermediate Coalescence test *********
    while(allocationsNum){
        //printf("%d %d \n",ptrs[allocationsNum], allocationsNum);
        free(ptrs[allocationsNum--]);
    }
    //printf("1 %d %d \n",ptrs[allocationsNum], allocationsNum);
    free(ptrs[0]);
    //printf("2 %d %d \n",ptrs[allocationsNum], allocationsNum);
    printf("Free all one by one\n");
    ptrs[0] = malloc(max_allocation);
    //printf("3 %d %d \n",ptrs[allocationsNum], allocationsNum);
    free(ptrs[0]);
    //printf("4 %d %d \n",ptrs[allocationsNum], allocationsNum);
    printf("Allocate max_allocation: %d B and free it\n",max_allocation);
    printf("Intermediate_coalescence passed!\n\n");

    ErrorDetection();

    return 0;
}