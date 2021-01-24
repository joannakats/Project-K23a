#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"
#include "job.h"
#include "thread_pool.h"
#include "loregression.h"

/* Global variables */
static logistic_regression *model;

static JobScheduler jobscheduler;
static volatile sig_atomic_t worker_quit; // Used to notify threads it's time to go

// Temporary struct used by threads
static struct line batch[BATCH_SIZE];
static long batch_size;
long total_hits;
static struct {
	pthread_mutex_t mutex;
	pthread_cond_t thread_done;
	int counter; // Used to verify that all threads finished working on this mini-batch
} notify;

void *worker_thread(void *arg);

long min(long num1, long num2) {
	return (num1 > num2 ) ? num2 : num1;
}

// need an initialize and delete for job scheduler
void jobsch_init(logistic_regression *_model, int threadNumber) {
	int i;

	/* Globals */
	model = _model;

	/* Job Scheduler */
	if (threadNumber > BATCH_SIZE)
		threadNumber = BATCH_SIZE;

	jobscheduler.execution_threads=threadNumber;
	jobscheduler.tids=malloc(threadNumber*sizeof(pthread_t));
	jobscheduler.queue=queue_init();

	pthread_mutex_init(&(jobscheduler.mutex), NULL);
	pthread_cond_init(&(jobscheduler.jobs_available), NULL);

	/* Notify sync structure */
	pthread_mutex_init(&(notify.mutex), NULL);
	pthread_cond_init(&(notify.thread_done), NULL);

	/* Create threads */
	printf("Spawning %d threads...\n", jobscheduler.execution_threads);
	for (i = 0; i < jobscheduler.execution_threads; ++i)
		pthread_create(jobscheduler.tids + i, NULL, worker_thread, NULL);
}

void submit_job(Job *job) {
	pthread_mutex_lock(&jobscheduler.mutex);

	queue_push(&jobscheduler.queue, job);

	pthread_cond_signal(&jobscheduler.jobs_available);
	pthread_mutex_unlock(&jobscheduler.mutex);
}

void wait_for_batch() {
	pthread_mutex_lock(&notify.mutex);

	/* Wait to see that all jobs are finished.
	   Counter will be equal to number of jobs, which happens to be the
	   execution threads number.

	   So it'll work even if one thread handles multiple jobs */
	while (notify.counter < jobscheduler.execution_threads)
		pthread_cond_wait(&notify.thread_done, &notify.mutex);

	pthread_mutex_unlock(&notify.mutex);
}

int run_batch(enum job_type type,int testing_n) {
	long i, size;
	const long lines_per_thread = batch_size / jobscheduler.execution_threads;
	Job job = {0};

	// Ready sync struct
	notify.counter = 0;

	job.type = type;
	// DEBUG:
	//printf("Got batch type %d\n", job.type);

	/* Split batch between the threads */
	for (i = 0; i < batch_size; i += size) {
		size = min(lines_per_thread, batch_size - i);

		job.start = i;
		job.end = i + size - 1;

		submit_job(&job);
	}

	wait_for_batch();

	/* Threads are done processing. Update weights */
	if(job.type==train)
		loregression_update_weights(model, batch, batch_size);
	else
		printf("Prediction accuracy: %5.2f%% (%ld hits)\n",total_hits / (double) testing_n * 100.0, total_hits);
	return 0;
}

void batch_push(node *spec1, node *spec2, int label) {
	batch[batch_size].spec[0] = spec1;
	batch[batch_size].spec[1] = spec2;
	batch[batch_size].label = label;

	batch_size++;
}

void batch_destroy() {
	long i;

	for (i = 0; i < batch_size; i++) {
		free(batch[i].x);
	}

	batch_size = 0;
}

void jobsch_destroy() {
	int i;

	/* Notify sync structure */
	pthread_mutex_destroy(&notify.mutex);
	pthread_cond_destroy(&notify.thread_done);

	/* Job Scheduler */
	worker_quit = 1;
	pthread_cond_broadcast(&jobscheduler.jobs_available);

	/* Join threads */
	for (i = 0; i < jobscheduler.execution_threads; i++)
		pthread_join(jobscheduler.tids[i], NULL);

	pthread_mutex_destroy(&jobscheduler.mutex);
	pthread_cond_destroy(&jobscheduler.jobs_available);

	free(jobscheduler.tids);
}

void *worker_thread(void *arg) {
	Job job;

	// Unused warning
	(void) arg;

	while (!worker_quit) {
		/* Pull job from queue */
		pthread_mutex_lock(&jobscheduler.mutex);

		while (empty(&jobscheduler.queue)) {
			pthread_cond_wait(&jobscheduler.jobs_available, &jobscheduler.mutex);

			if (worker_quit) {
				/* Time to go */
				pthread_mutex_unlock(&jobscheduler.mutex);
				return (void*) 0;
			}
		}

		job = queue_pull(&jobscheduler.queue);

		pthread_mutex_unlock(&jobscheduler.mutex);

		/* Loss computation */
		//DEBUG:
		//printf("Got job type %d: [%ld, %ld]\n", job.type, job.start, job.end);
		if(job.type==train)
			loregression_loss(model,batch, job.start, job.end);
		else{
			loregression_pbatch(model,batch,job.start,job.end);
			for(long i=job.start;i<job.end;i++){
				if(batch[i].label==batch[i].loss)
					thread_hits++;
			}
			
		}
		/* Done. Notify master thread */
		pthread_mutex_lock(&notify.mutex);
		total_hits+=thread_hits;
		notify.counter++;

		pthread_cond_signal(&notify.thread_done);
		pthread_mutex_unlock(&notify.mutex);
	}

	return (void*) 0;
}
