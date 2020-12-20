#include <stdio.h>

#include "common.h"
#include "prediction.h"

logistic_regression *prediction_init(bow *vocabulary) {
	return loregression_init(vocabulary->ht.count);
}

int prediction_validation(FILE *csv, int validation_n, logistic_regression *model) {
	long line_n, hits = 0;
	char line[512];
	char *left_spec, *right_spec, *label, *saveptr;

	int prediction; // TODO: Normally function return

	for (line_n = 1; line_n <= validation_n; line_n++) {
		if (!fgets(line, sizeof(line), csv)) {
			perror("Reading Dataset W for validation");
			return errno;
		}

		left_spec = strtok_r(line, ",", &saveptr);
		right_spec = strtok_r(NULL, ",", &saveptr);
		label = strtok_r(NULL, ",", &saveptr);

		/* TODO: Predict with logistic regression model */
		prediction = 1;

		if (prediction == atoi(label))
			hits++;
	}

	printf("Prediction accuracy for validation set: %5.2f%% (%ld hits)\n",
		hits / (double) validation_n * 100.0, hits);

	return 0;
}

int prediction_test(FILE *csv, int test_n, logistic_regression *model) {
	long line_n, hits = 0;
	char line[512];
	char *left_spec, *right_spec, *label, *saveptr;

	int prediction; // TODO: Normally function return

	for (line_n = 1; line_n <= test_n; line_n++) {
		if (!fgets(line, sizeof(line), csv)) {
			perror("Reading Dataset W for validation");
			return errno;
		}

		left_spec = strtok_r(line, ",", &saveptr);
		right_spec = strtok_r(NULL, ",", &saveptr);
		label = strtok_r(NULL, ",", &saveptr);

		/* TODO: Predict with logistic regression model */
		prediction = 1;

		if (prediction == atoi(label))
			hits++;
	}

	printf("Prediction accuracy for validation set: %5.2f%% (%ld hits)\n",
		hits / (double) test_n * 100.0, hits);

	return 0;
}
