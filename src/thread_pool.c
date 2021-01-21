#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"
#include "thread_pool.h"

#define THREAD_NUMBER 4

struct JobScheduler jobscheduler;

void *worker_thread(void *args);

// need an initialize and delete for job scheduler
void jobsch_init(int threadNumber) {
	int i;

	// Initialization
	jobscheduler.execution_threads=threadNumber;
	jobscheduler.tids=malloc(threadNumber*sizeof(pthread_t));
	jobscheduler.queue=queue_init();

	pthread_mutex_init(&(jobscheduler.mutex), NULL);
	pthread_cond_init(&(jobscheduler.jobs_available), NULL);

	/* Create threads */
	for (i = 0; i < jobscheduler.execution_threads; ++i)
		pthread_create(jobscheduler.tids + i, NULL, worker_thread, NULL);
}

int submit_job(Job *job) {
	pthread_mutex_lock(&jobscheduler.mutex);

	queue_push(&jobscheduler.queue, job);

	pthread_cond_signal(&jobscheduler.jobs_available);
	pthread_mutex_unlock(&jobscheduler.mutex);
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

void jobsch_destroy() {
	pthread_mutex_destroy(&jobscheduler.mutex);
	pthread_cond_destroy(&jobscheduler.jobs_available);
}

void *worker_thread(void *args) {
	Job job;

	for (;;) {
		// Pull job from queue
		pthread_mutex_lock(&jobscheduler.mutex);

		while (empty(&jobscheduler.queue))
			pthread_cond_wait(&jobscheduler.jobs_available, &jobscheduler.mutex);

		job = queue_pull(&jobscheduler.queue);

		pthread_mutex_unlock(&jobscheduler.mutex);

		// Do things
		printf("Got job %d: [%ld, %ld]\n", job.type, job.start, job.end);
	}
}
