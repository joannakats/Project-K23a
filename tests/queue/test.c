#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "thread_pool.h"
#include "job.h"
#include "acutest.h"

void test_queue(void){
	//check queue_init
	Queue queue=queue_init();
	TEST_CHECK(queue.qcount==0);
	//for insert check
	Job job1;
	job1.type=train;
	job1.start=1; //for testing purposes
	job1.end=2;
	Job job2;
	job2.type=test;
	job2.start=1;
	job2.end=2;
	
	queue_push(&queue,&job1);
	TEST_CHECK(queue.qcount==1);
	TEST_CHECK(queue.head!=NULL);
	TEST_CHECK(queue.tail!=NULL);
	queue_push(&queue,&job2);
	TEST_CHECK(queue.qcount==2);
	TEST_CHECK(queue.head!=NULL);
	TEST_CHECK(queue.tail!=NULL);
	
	//check queue pull and free
	queue_pull(&queue);
	TEST_CHECK(queue.qcount==1);
	queue_pull(&queue);
	TEST_CHECK(queue.qcount==0);
	TEST_CHECK(queue.head==NULL);
	TEST_CHECK(queue.tail==NULL);
}

TEST_LIST = {
	{"test_queue",test_queue},
	{NULL,NULL}
};