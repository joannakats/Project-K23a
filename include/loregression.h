#ifndef LOREGRESSION_H
#define LOREGRESSION_H

#include "vocabulary.h"

typedef struct logistic_regression{
	int size;
	double b;
	double *w;
	
}logistic_regression;

logistic_regression *loregression_init(int voc_size);
void loregression_delete(logistic_regression *loregression);
int logisticregression_train(logistic_regression *loregression,node *spec1,node *spec2,double label);
double sigmoid(double x);
double loregression_predict(logistic_regression *loregression,node *spec_left,node *spec_right);

#endif /* LOREGRESSION_H aka logistic regression */