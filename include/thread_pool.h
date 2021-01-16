#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "queue.h"

typedef struct JobScheduler{
	int execution_threads; //number of execution threads
	Queue q; //a queue that holds submitted jobs/tasks
	pthread_t* tids;// execution threads //may have a diff type
	pthread_mutex_t mutexQ;
	pthread_cond_t condQ;	
}JobScheduler;


void jobsch_init(int threadNumber); 
void executeJob(Queue* q,Task* task); //TODO 
void* thread_function(void* args); //
void enqueueJob(Queue* q,void (*function_push)(void*), void* arg_push);
