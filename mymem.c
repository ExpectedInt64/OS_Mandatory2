#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>

#define true 1
#define false 0




/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList
{
  // doubly-linked list
  struct memoryList *last;
  struct memoryList *next;

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  char *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy

typedef struct memoryList MemoryList;

size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;


/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz)
{
	myStrategy = strategy;

	/* all implementations will need an actual block of memory to use */
	mySize = sz;

	if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

	/* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
	if(head != NULL) {
		MemoryList *temp = NULL;
		while(head != NULL){
			temp = head->next;
			free(head);
			head = temp;
		}
	}

	myMemory = malloc(sz);
	
	//Make new memoryList and set last and next to NULL as we make a new list of structs
	head = (MemoryList*) malloc(sizeof(MemoryList));
	head -> next = NULL;
	head -> last = NULL;
	head -> size = sz;
	head -> alloc = 0;
	head -> ptr = myMemory;


	/* TODO: Initialize memory management structure. */

	
	
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested)
{
	assert((int)myStrategy > 0);
	
	switch (myStrategy)
	  {
	  case NotSet: 
	            return NULL;
	  case First:
	            return NULL;
	// Best fit assigned 
	  case Best: ;
	  			MemoryList *cPtr = NULL;
				//Set currentPointer to head
	  			cPtr = head;
				MemoryList *bPtr = NULL; //best pointer
				/*Best fit*/
				while(cPtr != NULL){
					
					if(cPtr->size >= requested && cPtr->alloc == false) {
						if(bPtr == NULL){
							bPtr = cPtr;
						}
						int b = bPtr->size;
						if(cPtr->size < b){
							bPtr = cPtr;
						}
					}
					cPtr=cPtr->next;
				}
				//Temp node to allocate memory and insert into linked list.
				//We can do this because we found a block of memory large enough to fit and be larger or equal to requested size.
				if(requested == bPtr->size && bPtr->alloc == false){
					bPtr->alloc = true;
					if(bPtr->next==NULL){
						bPtr->ptr=bPtr->last->ptr+bPtr->last->size;
					}
					return bPtr->ptr;
				} 
				
				MemoryList *temp = malloc(sizeof(MemoryList)*1);
				temp->size = requested;
				temp->alloc = true;
				//Check if last in next exists
				if(bPtr->last != NULL)temp->ptr=(bPtr->last->ptr+bPtr->last->size);
				else temp->ptr = bPtr->ptr;
				bPtr->size-=requested;
				if(bPtr->last != NULL){
					bPtr->last->next=temp;
					temp->last=bPtr->last;
					temp->next=bPtr;
					bPtr->last=temp;
				} else{
					temp->last=NULL;
					temp->next=bPtr;
					bPtr->last=temp;
					head = temp;
				}
	            return temp->ptr;


	  case Worst:
	            return NULL;
	  case Next:
	            return NULL;
	  }
	return NULL;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
	MemoryList *cPtr = NULL;
	//We take inspiration from the previous func (Best-fit)
	cPtr = head;
	MemoryList *bPtr = NULL;
	while(cPtr != NULL){
		if(cPtr->ptr == block && cPtr->alloc == 1) 	bPtr = cPtr;	
	cPtr=cPtr->next;
	}
	bPtr->alloc=0;
	return;
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
	MemoryList *cPtr = NULL;
	//Count holes, same idea as previous functions
	int holes = 0;
	cPtr = head;
	while(1){
		if(cPtr->alloc == false) holes++;
		if(cPtr->next != NULL) {cPtr = cPtr->next;}
		else break;
	}
	if(head->last != NULL){
		cPtr = head->next;
		while(true){
			if(cPtr->alloc == false) holes++;
			if(cPtr->last != NULL) {cPtr = cPtr->last;}
			else break;
		}
	}
	return holes;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
	MemoryList *cPtr = NULL;
	//We do the same as count holes, but we count size of allocated blocks
	int allocated = 0;
	cPtr = head;
	while(1){
		if(cPtr->alloc == true) allocated+=cPtr->size;
		if(cPtr->next != NULL) {cPtr = cPtr->next;}
		else break;
	}
	if(head->last != NULL){
		cPtr = head->next;
		while(true){
			if(cPtr->alloc == true) allocated+=cPtr->size;
			if(cPtr->last != NULL) {cPtr = cPtr->last;}
			else break;
		}
	}
	return allocated;
}

/* Number of non-allocated bytes */
int mem_free()
{
	MemoryList *cPtr = NULL;
	//Same as allocated, but with cPtr->alloc == false
	int unAllocated = 0;
	cPtr = head;
	while(1){
		if(cPtr->alloc == false) unAllocated+=cPtr->size;
		if(cPtr->next != NULL) {cPtr = cPtr->next;}
		else break;
	}
	if(head->last != NULL){
		cPtr = head->next;
		while(true){
			if(cPtr->alloc == false) unAllocated+=cPtr->size;
			if(cPtr->last != NULL) {cPtr = cPtr->last;}
			else break;
		}
	}
	return unAllocated;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
	MemoryList *cPtr = NULL;
	//Same as the previous functions again, this time we just check if it's larger than the other blocks
	int largest = 0;
	cPtr = head;
	while(1){
		if(cPtr->alloc == false && cPtr->size > largest) largest=cPtr->size;
		if(cPtr->next != NULL) {cPtr = cPtr->next;}
		else break;
	}
	if(head->last != NULL){
		cPtr = head->next;
		while(true){
			if(cPtr->alloc == false && cPtr->size > largest) largest=cPtr->size;
			if(cPtr->last != NULL) {cPtr = cPtr->last;}
			else break;
		}
	}
	return largest;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
	MemoryList *cPtr = NULL;
	//Smallest this time
	int smallest = 1;
	cPtr = head;
	while(1){
		if(cPtr->alloc == false && cPtr->size < smallest) smallest=cPtr->size;
		if(cPtr->next != NULL) {cPtr = cPtr->next;}
		else break;
	}
	if(head->last != NULL){
		cPtr = head->next;
		while(true){
			if(cPtr->alloc == false && cPtr->size < smallest) smallest=cPtr->size;
			if(cPtr->last != NULL) {cPtr = cPtr->last;}
			else break;
		}
	}
	return smallest;
}       

char mem_is_alloc(void *ptr)
{
	MemoryList *cPtr = NULL;
	//Same code as myFree(), just removed freeing stuff and returning if allocated
	cPtr = head;
	while(1){
		//We find the block we want to see if allocated
		if(cPtr->ptr == ptr){
			return cPtr->alloc;
		} else if(cPtr->next != NULL){
			cPtr = cPtr->next;
		} else {
			break;
		}
	}
	//We do the same as before
	if(cPtr != NULL){
		if(head->last!=NULL){
			cPtr = head->last;
		}
		while(1){
			if(cPtr->ptr == ptr){
				return cPtr->alloc;
			}
			else if(cPtr->last != NULL){
				cPtr = cPtr->last;
			} else{
				break;
			}
		}
	}
	return false;
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
	return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy)
{
	switch (strategy)
	{
		case Best:
			return "best";
		case Worst:
			return "worst";
		case First:
			return "first";
		case Next:
			return "next";
		default:
			return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
	if (!strcmp(strategy,"best"))
	{
		return Best;
	}
	else if (!strcmp(strategy,"worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy,"first"))
	{
		return First;
	}
	else if (!strcmp(strategy,"next"))
	{
		return Next;
	}
	else
	{
		return 0;
	}
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{
	MemoryList *cPtr = NULL;
	int counter = 1;
	cPtr = head;
	while(1){
		printf("Block %i, size %i, allocated %i\n",counter,cPtr->size,cPtr->alloc);
		counter++;
		if(cPtr->next != NULL) {cPtr = cPtr->next;}
		else break;
	}
	if(head->last != NULL){
		cPtr = head->next;
		while(true){
			printf("Block %i, size %i, allocated %i\n",counter,cPtr->size,cPtr->alloc);
			counter++;
			if(cPtr->last != NULL) {cPtr = cPtr->last;}
			else break;
		}
	}
	return;
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
        strategies strat;
	void *a, *b, *c, *d, *e;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */
	
	initmem(strat,500);
	
	a = mymalloc(100);
	b = mymalloc(100);
	c = mymalloc(100);
	myfree(b);
	d = mymalloc(50);
	myfree(a);
	e = mymalloc(25);
	
	print_memory();
	print_memory_status();
	
}
