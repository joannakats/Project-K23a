#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "queue.h"
#include "thread_pool.h"
#define THREAD_NUMBER 4


struct JobScheduler jobscheduler; 

// need an initialise and delete for job sceduler
void jobsch_init(int threadNumber){
	jobscheduler.execution_threads=threadNumber;
	jobscheduler.tids=malloc(threadNumber*sizeof(pthread_t));
	jobscheduler.q=queue_init();
	pthread_mutex_init(&(jobscheduler.mutexQ), NULL);
    pthread_cond_init(&(jobscheduler.condQ), NULL);
}

/*
void enqueueJob(Queue* q,void (*function_push)(void*), void* arg_push) { //needs check for mutex ,here or in queue_push??
    pthread_mutex_lock(&(jobscheduler.mutexQ));
    //printf("push tasks\n");
  	Task* task;
  	task=(Task*)malloc(sizeof(Task));
	if(task!=NULL){ 
		task->taskFunction=function_push; //function for train or test
		task->arg=arg_push;
		queue_push(q,task);//
	}else{
		printf("error task==NULL in enqueueJob\n");
	}
    pthread_mutex_unlock(&(jobscheduler.mutexQ));
    pthread_cond_signal(&(jobscheduler.condQ));
}
*/
