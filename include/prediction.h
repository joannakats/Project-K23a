#ifndef PREDICTION_H
#define PREDICTION_H

#include "loregression.h"
#include "vocabulary.h"

/*TODO: return pointer to log regression model */
logistic_regression *prediction_init(bow *vocabulary);

/* TODO: pass pointer to model as argument */
int prediction_validation(FILE *csv, int validation_n, logistic_regression *model);
int prediction_test(FILE *csv, int test_n, logistic_regression *model);

#endif /* PREDICTION_H */
