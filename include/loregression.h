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
int loregression_train(logistic_regression *loregression,node *spec_left,node *spec_right,double label);
double sigmoid(double x);
static double *make_x(node *spec_left,node *spec_right,int size);
int loregression_predict(logistic_regression *loregression,node *spec_left,node *spec_right);

#endif /* LOREGRESSION_H aka logistic regression */
