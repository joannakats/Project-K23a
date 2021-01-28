#ifndef LOREGRESSION_H
#define LOREGRESSION_H

#include "vocabulary.h"
#include "job.h"
#include "spec.h"

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

double loregression_possibility(logistic_regression *loregression,node *spec_left,node *spec_right);
void update_weights_of_clique(logistic_regression *model, clique *c);
void loregression_update_weights_of_pair(logistic_regression *model, node *spec1, node *spec2, double label);
void loregression_predict_cliques(logistic_regression *model, node *spec1, node *spec2);

/* Calculate hits for batch */
long loregression_pbatch(logistic_regression *loregression,struct line* batch,long a,long b);

#endif /* LOREGRESSION_H aka logistic regression */
