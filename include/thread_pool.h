#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "loregression.h"
#include "queue.h"
#include "spec_hashtable.h"

#define BATCH_SIZE 512 // TODO: mention speeds with 1024 or sth else, in Documentation

typedef struct JobScheduler {
	int execution_threads; //number of execution threads
	pthread_t *tids;// execution threads //may have a diff type
	Queue queue; //a queue that holds submitted jobs/tasks
	pthread_mutex_t mutex;
	pthread_cond_t jobs_available;
} JobScheduler;

long min(long num1, long num2);
void jobsch_init(logistic_regression *_model, int threadNumber);
void jobsch_destroy();

void batch_init();
void batch_push(node *spec1, node *spec2, int label);
void batch_destroy();

int run_batch(enum job_type type,int dataset);

#endif /* THREAD_POOL_H */
