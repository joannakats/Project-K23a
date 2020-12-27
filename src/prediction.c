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
		label = strtok_r(NULL, ",\n", &saveptr);

		spec1 = search_hashTable_spec(specs, left_spec, &pos);
		spec2 = search_hashTable_spec(specs, right_spec, &pos);

		//TODO:
		//loregression_train(model, spec1, spec2, atoi(label));
	}

	return 0;
}

int prediction_hits(FILE *csv, int set_n, hashtable *specs, logistic_regression *model) {
	char line[512];
	long line_n, hits = 0;
	char *left_spec, *right_spec, *label, *saveptr;

	node *spec1, *spec2;
	int pos;

	int prediction;

	for (line_n = 1; line_n <= set_n; line_n++) {
		if (!fgets(line, sizeof(line), csv)) {
			perror("Reading Dataset W for accuracy");
			return errno;
		}

		left_spec = strtok_r(line, ",", &saveptr);
		right_spec = strtok_r(NULL, ",", &saveptr);
		label = strtok_r(NULL, ",\n", &saveptr);

		spec1 = search_hashTable_spec(specs, left_spec, &pos);
		spec2 = search_hashTable_spec(specs, right_spec, &pos);

		//TODO:
		//prediction = loregression_predict(model, spec1, spec2);
		prediction = 1;

		printf("%s,%s (%s) => %d\n", left_spec, right_spec, label, prediction);

		if (prediction == atoi(label))
			hits++;
	}

	printf("Prediction accuracy: %5.2f%% (%ld hits)\n",
		hits / (double) set_n * 100.0, hits);

	return 0;
}
