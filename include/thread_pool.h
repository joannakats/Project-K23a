#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"

#define THREADS 4
#define BATCH_SIZE 512 // TODO: mention speeds with 1024 or sth else, in Documentation

typedef struct JobScheduler {
	int execution_threads; //number of execution threads
	pthread_t* tids;// execution threads //may have a diff type
	Queue queue; //a queue that holds submitted jobs/tasks
	pthread_mutex_t mutex;
	pthread_cond_t jobs_available;
} JobScheduler;


void jobsch_init(int threadNumber);
int submit_job(Job *job); //TODO: Job parameters instead of job struct?
//void executeJob(Queue* q, Job* job); // Maybe master thread creates the job parameters and passes them here
//void enqueueJob(Queue* q,void (*function_push)(void*), void* arg_push);

#endif /* THREAD_POOL_H */
