#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h> 
#include <queue> 
#include <semaphore.h>
using namespace std;

#define NUM_THREADS 5
#define MEMORY_SIZE 1000

struct node
{
	int id;
	int size;
};


queue<node> myqueue; // shared que
pthread_mutex_t sharedLock = PTHREAD_MUTEX_INITIALIZER; // mutex
pthread_t server; // server thread handle
sem_t semlist[NUM_THREADS]; // thread semaphores

int thread_message[NUM_THREADS]; // thread memory information
char  memory[MEMORY_SIZE]; // memory size



void release_function()
{
	//This function will be called
	//whenever the memory is no longer needed.
	//It will kill all the threads and deallocate all the data structures.
}

void my_malloc(int thread_id, int size)
{
	//This function will add the struct to the queue
	
}

void * server_function(void *)
{
	//This function should grant or decline a thread depending on memory size.
}

void * thread_function(void * id) 
{
	//This function will create a random size, and call my_malloc
	//Block
	//Then fill the memory with 1's or give an error prompt
}

void init()	 
{
	pthread_mutex_lock(&sharedLock);	//lock
	for(int i = 0; i < NUM_THREADS; i++) //initialize semaphores
	{sem_init(&semlist[i],0,0);}
	for (int i = 0; i < MEMORY_SIZE; i++)	//initialize memory 
  	{char zero = '0'; memory[i] = zero;}
   	pthread_create(&server,NULL,server_function,NULL); //start server 
	pthread_mutex_unlock(&sharedLock); //unlock
}



void dump_memory() 
{
 // You need to print the whole memory array here.
}

int main (int argc, char *argv[])
 {

 	//You need to create a thread ID array here

 	init();	// call init

 	//You need to create threads with using thread ID array, using pthread_create()

 	//You need to join the threads

 	dump_memory(); // this will print out the memory
 	printf("\nMemory Indexes:\n" );
 	for (int i = 0; i < NUM_THREADS; i++)
 	{
 		printf("[%d]" ,thread_message[i]); // this will print out the memory indexes
 	}
 	printf("\nTerminating...\n");
 }