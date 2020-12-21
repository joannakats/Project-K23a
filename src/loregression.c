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



/* The goal is to find the regression accounting function p(x) ,p(xi) near 0 or 1. */
int logregression_train(logistic_regression *loregression,node *spec_left,node *spec_right,double label){
	int word_size=loregression->size;
	int i,index=0,k;
	double *f=malloc(word_size*sizeof(*f));
	double *x=malloc(word_size*sizeof(*x));
	for(int i=0;i<word_size;i++){
		f[i]=0; //initialize with 0 for the following sum
	}
	//concat spec_idf_factors in x
	for(i=0;i<spec_left->wordCount;i++){
		x[index]=spec_left->tf_idf_factors[i];
		index++;
	}
	for(i=0;i<spec_right->wordCount;i++){
		x[index]=spec_right->tf_idf_factors[i];
		index++;
	}
	for(i=index;i<word_size;i++){
		x[i]=0.0;
	}

	//compute f(i)=w(i)*x[i]+b
	for(i=0;i<word_size;i++){
		f[i]+=loregression->w[i]*x[i]; /* TODO check weights ??*/
	}
	for(i=0;i<word_size;i++){
		f[i]+=loregression->b;
	}

	//p(x)=sigmoid(f(x))
	for(i=0;i<word_size;i++){
			f[i]=sigmoid(f[i]);

	}
	//cost
	double *j=malloc(word_size*sizeof(double));
	for(k=0;k<word_size;k++){
		j[k]=0;
	}
	for( k=0;k<word_size;k++){
		j[k]+=(f[k]-label)*x[k];
	}
	//weigh
	for( k=0;k<word_size;k++){
		loregression->w[k]-=LR*j[k];
	}

	free(f);
	free(x);
	return 0;
}



double loregression_predict(logistic_regression *loregression,node *spec_left,node *spec_right){
	int size=loregression->size;
	double *x=malloc((size )*sizeof(*x));
	int i,index=0;
	double pred = loregression->b;
	//concat spec_idf_factors in x
	for(i=0;i<spec_left->wordCount;i++){
		x[index]=spec_left->tf_idf_factors[i];
		index++;
	}
	for(i=0;i<spec_right->wordCount;i++){
		x[index]=spec_right->tf_idf_factors[i];
		index++;
	}
	for(int j=index;j<size ;j++){
		pred+=loregression->w[j] *x[j] ;
	}


	free(x);
	return sigmoid(pred);
}
