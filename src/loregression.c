#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
     return 1.0/(1.0 + exp(-x));
 }

static void compute_x(double *x, int size, node *spec_left, node *spec_right) {
	for(int i=0;i<size;i++){
		x[i]=fabs(spec_left->tf_idf_factors[i] - spec_right->tf_idf_factors[i]);
	}
}

/* The goal is to find the regression accounting function p(x) ,p(xi) near 0 or 1. */
void loregression_loss(logistic_regression *loregression, struct line *batch, long a, long b) {
	int size = loregression->size;
	long i, j;
	double f;

	/* Handle computation for part of batch, set by job */
	for (i = a; i <= b; i++) {
		batch[i].x = malloc(size * sizeof(double));
		compute_x(batch[i].x, size, batch[i].spec[0], batch[i].spec[1]);

		/* Compute f(x) */
		f = loregression->b;
		for (j = 0; j < size; j++)
			f += loregression->w[j] * batch[i].x[j];

		/* No other thread writing on this specific position */
		//TODO: IS this what the thread writes?
		batch[i].loss = (sigmoid(f) - (double) batch[i].label);
	}
}

/* Update weights after batch is done */
void loregression_update_weights(logistic_regression *loregression, struct line *batch, long batch_size) {
	long i, j;
	double loss = 0, x;

	for (i = 0; i < batch_size; i++)
		loss += batch[i].loss;

	for (j = 0; j < loregression->size; j++) {
		x = 0;
		for (i = 0; i < batch_size; i++)
			x += batch[i].x[j];

		//TODO: IS this what the master thread does? Maybe some average instead or sth?
		loregression->w[j] -= LR * loss * x;
	}
}


int loregression_predict(logistic_regression *loregression,node *spec_left,node *spec_right){
	int size=loregression->size;
	double *x = malloc(size * sizeof(double));
	double pred = loregression->b;

	compute_x(x, size, spec_left,spec_right);

	for(int j=0;j<size; j++){
		pred += loregression->w[j]*x[j];
	}

	free(x);

	return (sigmoid(pred) >= 0.5 ? 1 : 0);
	// //return (sigmoid(pred) >= 0.1 ? 1 : 0);	//alternative threshold
	///////////////////////////////////////////////////////////////////////////////////
}


double loregression_possibility(logistic_regression *loregression,node *spec_left,node *spec_right) {
	int size=loregression->size;
	double *x = malloc(size * sizeof(double));
	double pred = loregression->b;

	compute_x(x, size, spec_left,spec_right);

	for(int j=0;j<size; j++){
		pred += loregression->w[j]*x[j];
	}

	free(x);

	return sigmoid(pred);
}


/* updates weights of a newly merged clique */
void update_weights_of_clique(logistic_regression *model, clique *c) {
	cliqueNode *cn1, *cn2;
	cn1 = c->head;

	while(cn1 != NULL) {
		cn2 = cn1->next;

		while(cn2 != NULL) {
			loregression_update_weights_of_pair(model, cn1->spec, cn2->spec, 1.0);

			cn2 = cn2->next;
		}
		cn1 = cn1->next;
	}
}


void loregression_update_weights_of_pair(logistic_regression *model, node *spec1, node *spec2, double label) {
	int size = model->size;
	double f = model->b;
	double *x = malloc(size * sizeof(double));;
	double loss_j, pred;

	compute_x(x, size, spec1, spec2);

	//compute f=w[i]*x[i]+b
	for(int i=0;i<size;i++){
		f+=model->w[i]*x[i];
	}

	//p=sigmoid(f)
	pred=sigmoid(f);

	for(int k=0;k<size;k++){
		//cost
		loss_j=( pred -label)*x[k];
		//weight
		model->w[k]-= LR*loss_j;
	}

	free(x);
}
