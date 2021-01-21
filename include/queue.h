#ifndef _TQUEUE_H
#define _TQUEUE_H

enum job_type {
	train,
	test
};

typedef struct Job {
	enum job_type type;
	long start_offset; // byte offset in dataset W' (for fast fseek)
	long start_line;
	long end_line; // Needed because batch might be less than BATCH_SIZE
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
