#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "umalloc.h"
#include <string.h>

double difftimeval(const struct timeval *start, const struct timeval *end);

void Consistency_test(){
    char* ptr = (char*)malloc(10*sizeof(char));
    strcpy(ptr,"hello");
    free(ptr);
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

int main(){
    Consistency_test();
    int max_allocation = Maximization_test();
    printf("maximal allocation is: %d\n",max_allocation);

    BasicCoalescence_test(max_allocation);
   

    // ******* Saturation test *********
    char *ptrs[10500] = {0};
    int allocationsNum = 0;
    ptrs[allocationsNum] = malloc(1);
    while(ptrs[allocationsNum]!=NULL){
        allocationsNum++;
        ptrs[allocationsNum] = malloc(1);
    }
    printf("maximal number of allocations is: %d\n",allocationsNum);

    // ******* Time Overhead test *********
    allocationsNum--;
    free(ptrs[allocationsNum]);
    struct timeval start,end;
    gettimeofday(&start, NULL);
    long start1 = ((long)start.tv_sec)*1000+(long)start.tv_usec/1000;
    ptrs[allocationsNum] = malloc(1);
    gettimeofday(&end, NULL);
    long end1 = ((long)end.tv_sec)*1000+(long)end.tv_usec/1000;
    printf("Cost time: %.8ld ms\n", end1 - start1);
    // printf("%.8f\n", difftimeval(&end, &start));
    double time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);
   
    // ******* Intermediate Coalescence test *********
    while(allocationsNum){
        free(ptrs[allocationsNum--]);
    }
    free(ptrs[0]);

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