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

static long total_hits;
static struct {
	pthread_mutex_t mutex;
	pthread_cond_t thread_done;
	int counter; // Used to verify that all threads finished working on this mini-batch
} notify;

void *worker_thread(void *arg);

long min(long num1, long num2) {
	return (num1 > num2 ) ? num2 : num1;
}

void reset_hits() {
	total_hits = 0;
}

long get_hits() {
	return total_hits;
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

	/* Batch structure */
	batch_size = 0;
	for (i = 0; i < BATCH_SIZE; ++i)
		batch[i].x = malloc(model->size * sizeof(batch[0].x[0]));

	pthread_mutex_init(&(notify.mutex), NULL);
	pthread_cond_init(&(notify.thread_done), NULL);

	/* Spawn threads */
	printf("[Run sets (%d threads)]\n", jobscheduler.execution_threads);
	for (i = 0; i < jobscheduler.execution_threads; ++i)
		pthread_create(jobscheduler.tids + i, NULL, worker_thread, NULL);
}

void submit_job(Job *job) {
	pthread_mutex_lock(&jobscheduler.mutex);

	queue_push(&jobscheduler.queue, job);

	pthread_cond_signal(&jobscheduler.jobs_available);
	pthread_mutex_unlock(&jobscheduler.mutex);
}

void wait_for_batch(int jobs) {
	pthread_mutex_lock(&notify.mutex);

	/* Wait to see that all jobs are finished.
	   Threads increment the counter after finishing job */
	while (notify.counter < jobs)
		pthread_cond_wait(&notify.thread_done, &notify.mutex);

	pthread_mutex_unlock(&notify.mutex);
}

int run_batch(enum job_type type) {
	long i, size;
	/* At least <execution_threads> jobs */
	const long lines_per_thread = batch_size / jobscheduler.execution_threads;
	Job job = {0};
	int jobs_n = 0;

	/* Initialize notify counter */
	notify.counter = 0;

	// DEBUG:
	//printf("Got batch for job type %d\n", type);

	/* Split batch between the threads */
	job.type = type;
	for (i = 0; i < batch_size; i += size) {
		size = min(lines_per_thread, batch_size - i);

		job.start = i;
		job.end = i + size - 1;

		submit_job(&job);
		jobs_n++;
	}

	wait_for_batch(jobs_n);

	/* Threads are done processing. Update weights, if training */
	if (type == train)
		loregression_update_weights(model, batch, batch_size);

	/* Reset batch */
	batch_size = 0;

	return 0;
}

void batch_push(node *spec1, node *spec2, int label) {
	batch[batch_size].spec[0] = spec1;
	batch[batch_size].spec[1] = spec2;
	batch[batch_size].label = label;

	batch_size++;
}

void jobsch_destroy() {
	int i;

	/* Job Scheduler */
	worker_quit = 1;
	pthread_cond_broadcast(&jobscheduler.jobs_available);

	/* Join threads */
	for (i = 0; i < jobscheduler.execution_threads; i++)
		pthread_join(jobscheduler.tids[i], NULL);

	free(jobscheduler.tids);

	pthread_mutex_destroy(&jobscheduler.mutex);
	pthread_cond_destroy(&jobscheduler.jobs_available);

	/* Batch structures */
	pthread_mutex_destroy(&notify.mutex);
	pthread_cond_destroy(&notify.thread_done);

	for (i = 0; i < BATCH_SIZE; i++)
		free(batch[i].x);

	batch_size = 0;
}

void *worker_thread(void *arg) {
	Job job;
	int thread_hits = 0;

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

		//DEBUG:
		//printf("Got job type %d: [%ld, %ld]\n", job.type, job.start, job.end);

		if (job.type == train) /* Training: Distributed loss computation */
			loregression_loss(model, batch, job.start, job.end);
		else /* Count hits */
			thread_hits = loregression_pbatch(model,batch,job.start,job.end);

		/* Done. Notify master thread */
		pthread_mutex_lock(&notify.mutex);

		notify.counter++;

		if (job.type != train)
			total_hits += thread_hits;

		pthread_cond_signal(&notify.thread_done);
		pthread_mutex_unlock(&notify.mutex);
	}

	return (void*) 0;
}
