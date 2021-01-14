#include <stdio.h>

#ifndef _TQUEUE_H
#define _TQUEUE_H

typedef struct Task{
	int* start;
	int*end;
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
void queue_push(Queue* q,int* start,int* end);
Task queue_pull(Queue* q);

#endif