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

int loregression_train(logistic_regression *loregression,node *spec1,node *spec2,double label){
//TODO:train


	return 0;

}

int loregression_weight(logistic_regression *loregression,node *spec1,node *spec2){

	//TODO:lore_weight

	return 0;

}

void loregression_predict(logistic_regression *loregression,double *j,double *y){

//TODO:predict

}