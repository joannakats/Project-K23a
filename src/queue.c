#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "thread_pool.h"

Queue queue_init(void){
	Queue q;
	q.qcount=0; //empty
	q.head=NULL;
	q.tail=NULL;
	return q;
}
/* if the queue is empty return 1 else 0 */
int empty(Queue* q){
	return (q->qcount == 0);
}

void queue_push(Queue* q,Job* job) {
	qnode* temp;
	temp=malloc(sizeof(qnode));
	temp->job=*job;
	temp->next=NULL;
	//check if queue is empty
	if (empty(q)) {
		q->head=temp;
		q->tail=temp;
		q->qcount++;
	}else{
		q->tail->next=temp;
		q->tail=temp; // tail points to last element
		q->qcount++;
	}
}

/*we will check before if queue is empty*/
Job queue_pull(Queue* q){
	Job job = {0};
	if(q->head!=NULL){
		job=q->head->job;
		qnode* temp;
		temp=q->head;
		q->head=q->head->next;
		q->qcount--;
		free(temp);
	}

	return job;
}
