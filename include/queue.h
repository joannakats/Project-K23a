#ifndef _TQUEUE_H
#define _TQUEUE_H

enum job_type {
	train,
	test
};

typedef struct Job {
	enum job_type type;
	long start; // byte offset in dataset W' (for fseek)
	long end;
} Job;

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
