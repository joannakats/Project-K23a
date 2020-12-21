#include <stdio.h>

#include "common.h"
#include "loregression.h"
#include "prediction.h"
#include "spec_hashtable.h"

logistic_regression *prediction_init(bow *vocabulary) {
	return loregression_init(vocabulary->ht.count);
}

int prediction_training(FILE *csv, int training_n, hashtable *specs, logistic_regression *model) {
	char line[512];
	long line_n;
	char *left_spec, *right_spec, *label, *saveptr;

	node *spec1, *spec2;
	int pos;

	for (line_n = 1; line_n <= training_n; line_n++) {
		if (!fgets(line, sizeof(line), csv)) {
			perror("Reading Dataset W for training");
			return errno;
		}

		left_spec = strtok_r(line, ",", &saveptr);
		right_spec = strtok_r(NULL, ",", &saveptr);
		label = strtok_r(NULL, ",", &saveptr);

		spec1 = search_hashTable_spec(specs, left_spec, &pos);
		spec2 = search_hashTable_spec(specs, right_spec, &pos);

		logregression_train(model, spec1, spec2, (double) atoi(label));
	}

	return 0;
}

int prediction_validation(FILE *csv, int validation_n, hashtable *specs, logistic_regression *model) {
	char line[512];
	long line_n, hits = 0;
	char *left_spec, *right_spec, *label, *saveptr;

	node *spec1, *spec2;
	int pos;

	int prediction; // TODO: Normally function return

	for (line_n = 1; line_n <= validation_n; line_n++) {
		if (!fgets(line, sizeof(line), csv)) {
			perror("Reading Dataset W for validation");
			return errno;
		}

		left_spec = strtok_r(line, ",", &saveptr);
		right_spec = strtok_r(NULL, ",", &saveptr);
		label = strtok_r(NULL, ",", &saveptr);

		spec1 = search_hashTable_spec(specs, left_spec, &pos);
		spec2 = search_hashTable_spec(specs, right_spec, &pos);

		prediction = (int) loregression_predict(model, spec1, spec2);

		if (prediction == atoi(label))
			hits++;
	}

	printf("Prediction accuracy for validation set: %5.2f%% (%ld hits)\n",
		hits / (double) validation_n * 100.0, hits);

	return 0;
}

int prediction_test(FILE *csv, int test_n, hashtable *specs, logistic_regression *model) {
	char line[512];
	long line_n, hits = 0;
	char *left_spec, *right_spec, *label, *saveptr;

	node *spec1, *spec2;
	int pos;

	int prediction; // TODO: Normally function return

	for (line_n = 1; line_n <= test_n; line_n++) {
		if (!fgets(line, sizeof(line), csv)) {
			perror("Reading Dataset W for testing");
			return errno;
		}

		left_spec = strtok_r(line, ",", &saveptr);
		right_spec = strtok_r(NULL, ",", &saveptr);
		label = strtok_r(NULL, ",", &saveptr);

		spec1 = search_hashTable_spec(specs, left_spec, &pos);
		spec2 = search_hashTable_spec(specs, right_spec, &pos);

		prediction = (int) loregression_predict(model, spec1, spec2);

		if (prediction == atoi(label))
			hits++;
	}

	printf("Prediction accuracy for testing set: %5.2f%% (%ld hits)\n",
		hits / (double) test_n * 100.0, hits);

	return 0;
}
