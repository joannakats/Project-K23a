#ifndef PREDICTION_H
#define PREDICTION_H

#include "loregression.h"
#include "vocabulary.h"

/*TODO: return pointer to log regression model */
logistic_regression *prediction_init(bow *vocabulary);

/* TODO: pass pointer to model as argument */
int prediction_training(FILE *csv, int training_n, hashtable *specs, logistic_regression *model);
int prediction_hits(FILE *csv, int set_n, hashtable *specs, logistic_regression *model);

#endif /* PREDICTION_H */
