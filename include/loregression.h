#ifndef LOREGRESSION_H
#define LOREGRESSION_H

#include "vocabulary.h"
#include "job.h"

typedef struct logistic_regression{
	int size;
	double b;
	double *w;

}logistic_regression;

logistic_regression *loregression_init(int voc_size);
void loregression_delete(logistic_regression *loregression);
double sigmoid(double x);
void loregression_loss(logistic_regression *loregression, struct line *batch, long a, long b);
void loregression_update_weights(logistic_regression *loregression, struct line *batch, long batch_size);
int loregression_predict(logistic_regression *loregression,node *spec_left,node *spec_right);
void loregression_pbatch(logistic_regression *loregression,struct line* batch,long a,long b);
#endif /* LOREGRESSION_H aka logistic regression */
