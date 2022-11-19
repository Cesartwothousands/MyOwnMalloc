#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "umalloc.h"
#include <string.h>

double difftimeval(const struct timeval *start, const struct timeval *end);

/*void Consistency_test(){
    char* ptr = (char*)malloc(10*sizeof(char));
    strcpy(ptr,"hello");
    free(ptr);
}*/

void Consistency_test(){
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
}

int Maximization_test(){
    int max_allocation = 1;
    char* ptr=NULL;
    ptr = malloc(max_allocation);
    while(ptr!=NULL){
        free(ptr);
        max_allocation*=2;
        // printf("maximal  is: %d\n",max_allocation);
        ptr = malloc(max_allocation);
    }
    max_allocation/=2;
    ptr = malloc(max_allocation);
    free(ptr);
    return max_allocation;
}

void BasicCoalescence_test(int max_allocation){
    char* ptr1 = malloc(max_allocation/2);
    char* ptr2 = malloc(max_allocation/4);
    free(ptr1);
    free(ptr2);
    char* ptr3 = malloc(max_allocation);
    free(ptr3);
}

void ErrorDetection(){
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


}

int main(){
    printf("\nStart the test of our project\n\n");

    Consistency_test();
    printf("Consistency_test passed!\n\n");
    
    int max_allocation = Maximization_test();
    printf("maximal allocation is: %d\n",max_allocation);
    printf("Maximization_test passed!\n\n");

    BasicCoalescence_test(max_allocation);
    printf("BasicCoalescence_test passed!\n\n");
   

    // ******* Saturation test *********
    char *ptrs[SIZEOFMEM+1] = {0};
    int allocationsNum = 0;
    ptrs[allocationsNum] = malloc(1);
    while(ptrs[allocationsNum]!=NULL){
        allocationsNum++;
        ptrs[allocationsNum] = malloc(1);
    }
    printf("maximal number of allocations is: %d\n",allocationsNum);
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

    //printf("Cost time: %.16ld ms\n", end1 - start1);
    printf("Time_overhead passed!\n\n");

    // printf("%.8f\n", difftimeval(&end, &start));
    //double time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);
   
    // ******* Intermediate Coalescence test *********
    while(allocationsNum){
        free(ptrs[allocationsNum--]);
    }
    free(ptrs[0]);
    ptrs[0] = malloc(max_allocation);
    free(ptrs[0]);
    printf("Intermediate_coalescence passed!\n\n");

    ErrorDetection();

    return 0;
}

double difftimeval(const struct timeval *start, const struct timeval *end)
{
        double d;
        time_t s;
        suseconds_t u;

        s = start->tv_sec - end->tv_sec;
        u = start->tv_usec - end->tv_usec;
        //if (u < 0)
        //        --s;

        d = s;
        d *= 1000000.0;
        d += u;

        return d;
}