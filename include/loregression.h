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
int logisticregression_train(logistic_regression *loregression,node *spec1,node *spec2,int label);
int loregression_weight(logistic_regression *loregression,node *spec1,node *spec2);//
void loregression_predict(logistic_regression *loregression,double *j,double *y);



#endif /* LOREGRESSION_H aka logistic regression */