#ifndef JOB_H
#define JOB_H

#include "spec_hashtable.h"

enum job_type {
	train,
	test
};

typedef struct Job {
	enum job_type type;
	long start; // indexes in batch array
	long end;
} Job;

struct line {
	node *spec[2];
	int label;
	double loss; /* Computed by the threads */
	double *x;
};

#endif /* JOB_H */
