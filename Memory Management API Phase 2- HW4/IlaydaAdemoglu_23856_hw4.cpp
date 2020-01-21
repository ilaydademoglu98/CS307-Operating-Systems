
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <queue>
#include <semaphore.h>
using namespace std;

#define NUM_THREADS 5
#define MEMORY_SIZE 10

//Ilayda Ademoglu(23856) this is the 4th homework of cs307

struct Node //Node for the linked list
{
    int id;
    int size;
    int index;
    Node *next, *prev;
    
    Node(int id,int size,int index,Node * next,Node * prev)
    :id(id),size(size),index(index),next(next),prev(prev){}
};

struct queuenode //node for threads
{
    int id;
    int size;
};

Node * memoryhead = new Node(-1,MEMORY_SIZE,0,NULL,NULL); //creating of the linked list


class LinkedList //linked list class
{
public:
    int length;
    Node* head;

    LinkedList();
    ~LinkedList();
    void add(int id,int size,int index);
    void print();
};

LinkedList::LinkedList(){
    this->length = 0;
    this->head = NULL;
}

LinkedList::~LinkedList(){
    cout << "LIST DELETED";
}

void LinkedList::print(){
    Node* head = this->head;
    int i = 1;
    while(head){
        cout << i << ": " << head->index << " " << head->id << " " << head->size << endl;
        head = head->next;
        i++;
    }
}


queue<queuenode> myqueue; // shared que
pthread_mutex_t sharedLock = PTHREAD_MUTEX_INITIALIZER; // mutex
sem_t semlist[NUM_THREADS]; // thread semaphores
char  memory[MEMORY_SIZE]; // memory size
pthread_t server; // server thread handle
bool check = true; //to check server is working or not

//initialization of linked list


void release_function() //at the end of the main function when memory is not needed it will call release
{ //to empty the list

    for (int i = 0; i < MEMORY_SIZE; i++)
    {char X = 'X'; memory[i] = X;}
    for(int i = 0; i < NUM_THREADS; i++)
    {sem_destroy(&semlist[i]);}
    
    Node * ptr = new Node(0,0,0,NULL,NULL); //deallocate the linked list
    ptr = memoryhead;
    while(ptr != NULL)
    {
        delete ptr;
        ptr = ptr->next;
    }
}

void use_mem() //sleep for a while
{
    int sleepingTime = rand() % 5; //time is from 0 to 4
    sleepingTime = sleepingTime + 1; //time is from 1 to 5
    sleep(sleepingTime);
}

void free_mem(int id) //merging the holes
{
     Node *copy = memoryhead;
    for(int i=0;i<MEMORY_SIZE;i++)
    {
        if(memory[i] == id + 48)
            memory[i] = 'X';
    }
    while(copy!=NULL && (copy->id != id)){
        copy = copy->next;
    }
    if(copy!=NULL) //ALL THE CASES FOR MERGING IN THE HOMEWORK DOCUMENT
    {
        copy->id = -1;
        if(copy->next && copy->next->id == -1)
        {
            copy->size = copy->size + copy->next->size;
            Node *x = copy->next;
            if(x->next != NULL)
                x->next->prev = copy;
            x = x->next;
        }
       else if(copy->prev && copy->prev->id == -1 && copy->next && copy->next->id == -1)
        {
            copy->prev->size =  copy->prev->size + copy->size + copy->next->size;
            if(copy->next->next)
                copy->next->next->prev = copy->prev;
            copy->prev->next =copy->next->next;
            delete copy;
            delete copy->next;
        }
        else if(copy->prev && copy->prev->id == -1)
        {
            copy->prev->size += copy->size;
            if(copy->next)
                copy->next->prev = copy->prev;
            copy->prev->next = copy->next;
        }
        
    }
}

void dump_memory() //dump memory function
{
    cout << "List:" << endl;
    Node * copy = memoryhead;
    cout << "[" << copy->id << "]" << "[" << copy->size << "]"
    << "[" << copy->index << "]";
    copy = copy->next;
    while(copy != NULL)
    {
        cout << "--[" << copy->id << "]" << "[" << copy->size << "]"
        << "[" << copy->index << "]";
        copy = copy->next;
    }
    cout << endl << "Memory Dump:" << endl;
    for(int i=0; i < MEMORY_SIZE; i++)
        cout << memory[i];
    cout << endl << "******************************************************************" << endl;
}

void my_malloc(int id, int size) //mymalloc function
{
    queuenode * nodetopush = new queuenode();
    nodetopush->id = id;
    nodetopush->size = size;
    myqueue.push(*nodetopush);
}



bool checkmem(queuenode *node){ //this function checks the memory
    Node * copy = memoryhead;
    while(copy)
    {
        if(copy->id == -1 && (node->size <= copy->size))
        {
            for(int i=copy->index; i < copy->index+node->size;i++)
                memory[i] = 48 + node->id;
            Node * toBeInserted = new Node(node->id,node->size,0,copy,copy->prev);
            if(copy->prev)
                toBeInserted->index = copy->prev->index + copy->prev->size;
            copy->index += node->size;
            copy->size -= node->size;
            if(copy->prev)
                copy->prev->next = toBeInserted;
            else
                memoryhead = toBeInserted;
            copy->prev = toBeInserted;
            if(!(copy->size)){
                copy->prev->next = copy->next;
                if(copy->next)
                    copy->next->prev = copy->prev;
                delete copy;
            }
            
            return true;
        }
        copy = copy->next;
    }
    return false;
}

void * server_function(void *)
{
    check = false;
    time_t start = time(NULL);
    queuenode * quenode = NULL;
    while(true)
    {
        while(!myqueue.empty())
        {
            pthread_mutex_lock(&sharedLock);    //lock
            quenode = &myqueue.front(); //popping from the queue
            if(checkmem(quenode)) //it its available
            {
                myqueue.pop();
                dump_memory();
                int idx = quenode->id;
                sem_post(&semlist[idx]);
            }
            else
            {
                myqueue.pop();
                myqueue.push(*quenode);
            }
            pthread_mutex_unlock(&sharedLock); //unlock
            time_t end = time(NULL);
            if(end-start >= 10)
            {
                check = true;
                return 0;
            }
        }
    }
    return 0;
}

void * thread_function(void * id) //this is thread function
{
    int x, * idp;
    while(!check) //is available
    {
        pthread_mutex_lock(&sharedLock);    //lock
        idp = (int *) id;
        x = rand() % (MEMORY_SIZE/3);
        x += 1;
        my_malloc(*idp,x);
        pthread_mutex_unlock(&sharedLock);
        sem_wait(&semlist[*idp]);
        if(check)
            break;
        use_mem(); //thread will go to sleep
        
        //mutex
        pthread_mutex_lock(&sharedLock);    //lock
        free_mem(*idp);
        pthread_mutex_unlock(&sharedLock); //unlock
    }
    return 0;
}


int main() {

    srand(time(NULL));
    int thread_id[NUM_THREADS]; //threadid array
    for(int i=0; i < NUM_THREADS; i++) // assigning id's to threads
        thread_id[i] = i;
    
    for(int i = 0; i < NUM_THREADS; i++) //initialize semaphores
    {sem_init(&semlist[i],0,0);}
    for (int i = 0; i < MEMORY_SIZE; i++)    //initialize memory
      {char X = 'X'; memory[i] = X;}
    
    pthread_create(&server,NULL,server_function,NULL); //start server
    
    pthread_t threads[NUM_THREADS];
    for(int i=0; i < NUM_THREADS; i++)
        pthread_create(&threads[i],NULL,thread_function,(void *) &thread_id[i]);
    
    pthread_join(server,NULL);
    for(int i = 0; i < NUM_THREADS; i++) //server is down, all semaphore can be up
        sem_post(&semlist[i]);
    for(int i=0; i < NUM_THREADS; i++)
            pthread_join(threads[i],NULL);

    release_function();
    return 0;
}
