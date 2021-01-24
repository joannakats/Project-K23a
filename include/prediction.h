#ifndef PREDICTION_H
#define PREDICTION_H

#include "vocabulary.h"
#include "loregression.h"

int prediction_init(bow *vocabulary);
void prediction_destroy();
logistic_regression *get_model();

int prediction_training(FILE *csv, int training_n, hashtable *specs);
int prediction_hits(FILE *csv, int set_n, hashtable *specs);

#endif /* PREDICTION_H */
