#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "loregression.h"
#include "vocabulary.h"

#define E 2.71828
//LR is learning rate,in lesson was suggested 0.1
#define LR 0.1


logistic_regression *loregression_init(int voc_size){
	logistic_regression *new =malloc(sizeof(*new));
	if(!new){
		perror("loregression_init");
		return NULL;
	}

	new->b=(double)rand() / (double)((unsigned)RAND_MAX + 1);
	new->size=voc_size;
	new->w=malloc(sizeof(double) * voc_size); //check voc_size the array or sth else
	for(int i=0;i<voc_size;i++){
		new->w[i]= (double)rand() / (double)((unsigned)RAND_MAX + 1);
	}

	return new;

}


void loregression_delete(logistic_regression *loregression){
	free(loregression->w);
	free(loregression); //
}
 double sigmoid(double x){
     return 1.0/(1.0 +exp(-x));
 }

static double *make_x(node *spec_left,node *spec_right,int size){

	double *x=malloc(size *sizeof(double));
	for(int i=0;i<size;i++){
		x[i]=fabs(spec_left->tf_idf_factors[i] - spec_right->tf_idf_factors[i]);
	}
	return x;
}

/* The goal is to find the regression accounting function p(x) ,p(xi) near 0 or 1. */
int loregression_train(logistic_regression *loregression,node *spec_left,node *spec_right,double label){
	int size=loregression->size;
	double f= loregression->b;
	double *x;
	double loss_j,pred;
	
	x=make_x(spec_left,spec_right,size);
	
	//compute f=w[i]*x[i]+b
	for(int i=0;i<size;i++){
		f+=loregression->w[i]*x[i]; 
	}
	
	//p=sigmoid(f)
	pred=sigmoid(f);
	
	for(int k=0;k<size;k++){
		//cost
		loss_j=( pred -label)*x[k];
		//weight
		loregression->w[k]-= LR*loss_j;
	}

	free(x);
	return 0;
}




int loregression_predict(logistic_regression *loregression,node *spec_left,node *spec_right){
	int size=loregression->size;
	double *x;
	double pred = loregression->b;
	
	x=make_x(spec_left,spec_right,size);
	
	for(int j=0;j<size ;j++){
		pred+=loregression->w[j] *x[j] ;
	}


	free(x);
	return (sigmoid(pred) >= 0.5 ? 1 : 0);
	//return (sigmoid(pred) >= 0.1 ? 1 : 0);	//alternative threshold
}
