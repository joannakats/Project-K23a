#include <stdio.h>

#ifndef _TQUEUE_H
#define _TQUEUE_H

typedef struct Task{
	// may need int type; //for the type of task..for train is 1 ,for test is 2.
	void (*taskFunction)(void*); //
	void* arg;
}Task;

typedef struct qnode{
	Task task; // check the type
	struct qnode* next;
}qnode;

typedef struct Queue{
	int qcount;
	qnode* head;
	qnode* tail;
}Queue;

Queue queue_init(void);
int empty(Queue* q);
void queue_push(Queue* q,Task* task);
Task queue_pull(Queue* q);

#endif