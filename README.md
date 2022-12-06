# My own malloc() library

Our own Malloc() library function, create a memory allocator using malloc() as the template. 

## Contributors: Zihan Chen, Jiayi Zhang and Qipan Xu

## Academic Integrity Policy

In order to respect our university's academic integrity policy, we have decided not to show all of codes of tests own by professor, including the specific requirements of this assignment.

## 1. Design and Implementation of umalloc

Firstly we load with 10MB static char array

We use **inUse** to store the status whether it can be freed, we use **size** to store the size of this block and we use a ptr to help check neighbor blocks

- You can use run.sh we attached to run the demo

## 2. Tests

### Consistency

Allocate and free twice, the address of the pointers are the same

[pic1](pic1.png)

### Maximization

Find max_allocation: 8388608 B

[pic2](pic2.png)

### Basic Coalescence

Pass all the test, it could work

[pic3](pic3.png)

### Saturation, Time Overhead and Intermediate Coalescence

Do 9K 1KB allocation and allocate 1B till full

Saturation space is 9518551 B

Use **gettimeofday** to calculate Cost Time 0.000481s

Free one by one and allocate max allocation, pass all the test, it could work

[pic4](pic4.png)

## 3. Error Detection

Use three error tests and one successful test given by Introduction

Pass all the test, it could work

[pic5](pic5.png)