#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <queue>
#include <semaphore.h>
using namespace std;

// I'M ILAYDA ADEMOGLU (23856) THIS IS THE THIRD HOMEWORK OF CS307 COURSE
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
char memory[MEMORY_SIZE]; // memory size
int mem_size_left = 1000;


void release_function()
{
    //This function will be called
    //whenever the memory is no longer needed.
    //It will kill all the threads and deallocate all the data structures.
    pthread_mutex_destroy(&sharedLock);
    for (int i = 0; i < MEMORY_SIZE; i++)  //empty the memory
      {char zero = '0'; memory[i] = zero;}
    for(int i = 0; i < NUM_THREADS; i++) //deletes semaphores
       {sem_destroy(&semlist[i]);}

    
}

void my_malloc(int thread_id, int size)
{
    pthread_mutex_lock(&sharedLock);    //lock //This function will add the struct to the queue
    node *push = new node();
    push->id = thread_id;
    push->size = size;
    myqueue.push(*push);
    pthread_mutex_unlock(&sharedLock); //unlock
}

void * server_function(void *)
{
    //This function should grant or decline a thread depending on memory size.
    while(true){
        pthread_mutex_lock(&sharedLock);    //lock
        while (!myqueue.empty()) {
            node * having = new node();
            having = &myqueue.front();
            if(having->size <= mem_size_left){

                thread_message[having->id] = 1000 - mem_size_left;
                mem_size_left -= having->size;
            }
            else{

                thread_message[having->id] = -1;
            }

            myqueue.pop();
            sem_post(&semlist[having->id]);
        }

        pthread_mutex_unlock(&sharedLock); //unlock
    }
}

void * thread_function(void * id)
{
    pthread_mutex_lock(&sharedLock);    //lock
    //This function will create a random size, and call my_malloc
    //Block
    //Then fill the memory with 1's or give an error prompt
    int minimum = 1;
    int maximum = 600;
    int random_size = rand()%(maximum-minimum + 1) + minimum;

    int * pointerforid = (int *) id;
    cout << "Thread "<<*pointerforid << ": Is going to try to allocate " << random_size << endl;
    pthread_mutex_unlock(&sharedLock); //unlock

    my_malloc(*pointerforid, random_size);
    sem_wait(&semlist[*pointerforid]);

    pthread_mutex_lock(&sharedLock);    //lock
    if(thread_message[*pointerforid] != -1)
    {
        for (int i = 0; i < random_size; ++i)
        {
            memory[thread_message[*pointerforid]] = '1';
            thread_message[*pointerforid]++;
        }
    }
    else if(thread_message[*pointerforid] == -1){
        cout <<"Thread "<<*pointerforid << ": Not enough memory." << endl;
    }
    pthread_mutex_unlock(&sharedLock); //unlock
}

void init()
{
    pthread_mutex_lock(&sharedLock);    //lock

    for(int i = 0; i < NUM_THREADS; i++) //initialize semaphores
    {
        sem_init(&semlist[i],0,0);
    }
    for (int i = 0; i < MEMORY_SIZE; i++)    //initialize memory
      {
          char zero = '0';
          memory[i] = zero;
      }
       pthread_create(&server,NULL,server_function,NULL); //start server

    pthread_mutex_unlock(&sharedLock); //unlock
}



void dump_memory()
{
     // You need to print the whole memory array here.
     pthread_mutex_lock(&sharedLock);    //lock

     cout <<"Memory Dump: " << endl;
     for (int i = 0; i < MEMORY_SIZE; ++i)
     {
         cout<< memory[i];
     }
     cout << endl;

     pthread_mutex_unlock(&sharedLock); //unlock
}

int main (int argc, char *argv[])
 {
     pthread_t threadarray[NUM_THREADS];

     //You need to create a thread ID array here
     int arrayid[NUM_THREADS];
     for (int i = 0; i < NUM_THREADS; ++i)
     {
         arrayid[i] = i;
     }

     init();    // call init
     
      srand(time(NULL));

     //You need to create threads with using thread ID array, using pthread_create()
     
     for(int i=0; i < NUM_THREADS; i++)
         pthread_create(&threadarray[i],NULL,thread_function,(void *) &arrayid[i]);
     
     //You need to join the threads
     for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threadarray[i], NULL);

     dump_memory(); // this will print out the memory
     printf("\nMemory Indexes:\n" );
     for (int i = 0; i < NUM_THREADS; i++)
     {
         pthread_mutex_lock(&sharedLock);    //lock
         printf("[%d]" ,thread_message[i]); // this will print out the memory indexes
         pthread_mutex_unlock(&sharedLock); //unlock
     }
     printf("\nTerminating...\n");

     release_function();
     return 0;
 }
