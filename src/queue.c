#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

Queue queue_init(void){
	Queue q;
	q.qcount=0; //empty
	q.head=NULL;
	q.tail=NULL;
	return q;
}
/* if the queue is empty return 1 else 0 */
int empty(Queue* q){
	return (q->qcount);
}

void queue_push(Queue* q,int* start,int* end){
	qnode* temp;
	temp=malloc(sizeof(qnode));
	temp->task.start=start;
	temp->task.end=end;
	temp->next=NULL;
	//check if queue is empty
	if(empty(q)==0){
		q->head=temp;
		q->tail=temp;
		q->qcount++;
	}else{
		q->tail->next=temp;
		q->tail=temp; // tail points to last element
		q->qcount++;
	}
}

Task queue_pull(Queue* q){
	Task task;
	if(q->head!=NULL){
		task=q->head->task;
		qnode* temp;
		temp=q->head;
		q->head=q->head->next;
		q->qcount--;
		free(temp);
	return task;
	}else{
		task.start=NULL;
		task.end=NULL;
		printf("not a task in the queue\n");
		return task;
	}
}
