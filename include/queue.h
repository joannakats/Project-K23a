#ifndef _TQUEUE_H
#define _TQUEUE_H

#include "job.h"

typedef struct qnode{
	Job job; // check the type
	struct qnode* next;
}qnode;

typedef struct Queue{
	int qcount;
	qnode* head;
	qnode* tail;
}Queue;

Queue queue_init(void);
int empty(Queue* q);
void queue_push(Queue* q, Job* job);
Job queue_pull(Queue* q);

#endif
