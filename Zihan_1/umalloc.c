#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "umalloc.h"


static char mem[SIZEOFMEM];


void ufree(void* ptr, char* file, int line) {

	if(ptr == NULL) {
		return; 
	}
	
	//run through the meta data, check if given pointer exist or is being used. 
	void* ptrBlock = mem; 
	int memSize = 0; 
	
	//create a way to access the previous metadata.
	metadata *prev = NULL; 
	for(metadata *crnt = ptrBlock; crnt != NULL; crnt = crnt->next ) {
			void *ptrCrnt = &crnt->inUse; 	
			
			//the ptr that was called on is found, check if it can be freed or not
			if((ptrCrnt+sizeof(metadata)) == ptr) {

				//Free it since it is inUse
				if(crnt->inUse == 2) {
					//sets the values of the return pointers to 0. 
					for(int i = (memSize+sizeof(metadata)); i < (memSize+crnt->size+sizeof(metadata)); i++) {
							mem[i] = 0; 
						}
					crnt->inUse = 1; 

					//Now check for other possible neighboring Free block to merge with.
					//the free is between two other freed metadata.  
					if(prev != NULL && crnt->next != NULL) {
						if(prev->inUse == 1 && crnt->next->inUse == 1) {

							int oldSize = crnt->size; 
							int oldSize2 = prev->size; 
							int oldSize3 = crnt->next->size; 

							prev->size = (prev->size+sizeof(metadata)+sizeof(metadata)+crnt->size+crnt->next->size);
							prev->next = crnt->next->next;
						       	crnt = prev; 	
							for(int i = (memSize - oldSize2); i < (memSize - oldSize2 + sizeof(metadata)+oldSize2+sizeof(metadata)+oldSize+oldSize3); i++) {
								mem[i] = 0; 
							}
							return;
						}
					}

					if(prev != NULL) {

						//the previous block is also free, merge them 
						if(prev->inUse == 1) {
						
							int oldSize = crnt->size; 

							//merge the previous block
							prev->size = (prev->size+sizeof(metadata)+crnt->size); 
							prev->next = prev->next->next; 
							crnt = prev; 

							for(int i = (memSize); i < (memSize+sizeof(metadata)+oldSize); i++) {
								mem[i] = 0; 
							}

							return; 
						}
					}

					if(crnt->next != NULL) {
						//the next block is also not in use, therefore merge it
						if(crnt->next->inUse == 1) {
							int oldSize = crnt->next->size; 
							crnt->size = (crnt->size+sizeof(metadata)+crnt->next->size); 
							crnt->next = crnt->next->next; 

								for(int i = (memSize+sizeof(metadata)); i < (memSize+sizeof(metadata)+sizeof(metadata)+(crnt->size-oldSize)); i++) {
									mem[i] = 0;
								}	

							return; 
							
						}
					}
						
					return;
				}

				//The pointer is not being used. 
				else{
					printf("pointer already has been freed\nfile:%s\nline:%d\n", file, line);
					return;
				}
			}

			//move the previous foward
			prev = crnt; 
			//update memSize
		 	memSize = memSize + sizeof(metadata) + crnt->size;
	}

	printf("Invalid call to free, pointer was never malloced\n file:%s\n line:%d\n", file, line);
}


//return a void pointer 
void* umalloc(size_t size, char* file, int line) {
			
	//if the size is greater than 10240 minus the size of meta, there is not enough space
	if(size > (SIZEOFMEM-sizeof(metadata))) {
		printf("there is no free memory\n file:%s\n line:%d\n", file,line);
		return NULL;
	}

	//if they malloc a size = 0, it is not an error, just returns a null pointer
	if(size == 0) {
		printf("the amount requested makes no sense\n file:%s\n line:%d\n", file,line);
		return NULL; 
	}

	//create a pointer to the begining of the block, if the has a size initiallized to zero, then it must be empty and it is the first call to malloc
	void *f = mem;
	metadata *isEmpty = f; 
	//the array is empty
	if(isEmpty->size == 0) {

		//create a new struct to hold the first block allocated and a second to watch over the remaining unused space. 
		metadata* front = (metadata*) mem; 
		front->inUse = 2; 
		front->size = size; 
		front->next = NULL;
		void* ptr = mem+sizeof(metadata); 

		//if the entire array is being used, then there is nothing left over. 
		if(sizeof(metadata)+size == SIZEOFMEM) {
			return ptr; 
		}

		//the metadata for the remaining memory which will not be in use
		metadata* leftover = (metadata*) (mem+sizeof(metadata)+size); 
		leftover->inUse = 1; 
		leftover->size = (SIZEOFMEM-sizeof(metadata)-size); 
		leftover->next = NULL;
		front->next = leftover; 
	    return ptr;	
	}

	//array is not empty, search for a node of empty metadata to store the mem, or create a new. 
	//create a pointer at the front of the block to access the first set of metadata. 
	void *r = mem;
    int memUsed = 0; 	
	//traverse through all of the meta data that can either be in use or free. 
	for(metadata *crnt = r; crnt != NULL; crnt = crnt->next ) {			

		//keep track of the total mem used that way the next meta location is known if one needs to be made.  
		memUsed= memUsed + crnt->size + sizeof(metadata);
		//printf("block size: %d inUse: %d      block+meta %ld\n", crnt->size, crnt->inUse, crnt->size+sizeof(meta));	
		
		if(crnt->inUse == 1) {
			
			//it is just the right size
			if(size == crnt->size) {

				//make it inUse and return the pointer. 
				crnt->inUse = 2;
			    void* r = crnt; 	
				return (r+sizeof(metadata)); 		
			}

			//the size that is asked for is less than is available, split into another block
			if(size < crnt->size) {

				//there is not enough room to hold another meta data block, return the pointer with extra space
				if(size + sizeof(metadata) > crnt->size) {
					crnt->inUse = 2;
			       	void* r = crnt; 	
					return (r+sizeof(metadata)); 
				}
				
				//split into two block of meta data
				//create the new block at the appropriate location	
				metadata *temp = (metadata*) (mem+memUsed-crnt->size+size);

				//since it will be empty set it to free
				temp->inUse = 1; 
				temp->size = (crnt->size-sizeof(metadata)-size); 
				temp->next = crnt->next;
				crnt->next = temp; 
			
				//return the pointer at the same spot since that did not change. 
				crnt->size = size;
				crnt->inUse = 2; 
				void* r = crnt;
				return (r+sizeof(metadata));

			}
		}

		//The last meta data was reached, a new block must be allocated. 
		if(crnt->next == NULL) {

			if(memUsed + size + sizeof(metadata) > SIZEOFMEM) {
				printf("Not enough memory avaliable for malloc call\nfile:%s\nline%d\n", file, line);
				return NULL;
			}

			//MUST USE parenthesis for addition, otherwise it will add by a size of the struct. 		
			metadata *temp = (metadata*) (mem+memUsed);
		       	

			temp->size = size;
			temp->inUse = 2;
			temp->next = NULL;
		 	//copy the mem to the array
	
			//creates a pointer to the struct in order to add it to the end of the link. 
			crnt->next = temp;
			
			//return the unspecified pointer.
			void *ptr = mem+memUsed+sizeof(metadata); 
			return ptr; 
		}
		
	}

	//this should not be reached.  
	return NULL;  	
}
