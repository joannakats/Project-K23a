#include <stdio.h>

#include "common.h"
#include "loregression.h"
#include "prediction.h"
#include "spec_hashtable.h"
#include "thread_pool.h"

long min(long num1, long num2) {
	return (num1 > num2 ) ? num2 : num1;
}

logistic_regression *prediction_init(bow *vocabulary) {
	jobsch_init(THREADS);
	return loregression_init(vocabulary->ht.count);
}

int prediction_training(FILE *csv, int training_n, hashtable *specs, logistic_regression *model) {
	char line[512];
	long i, line_n, step;
	char *left_spec, *right_spec, *label, *saveptr;

	Job job;
	job.type = train;

	line_n = 1;
	while (line_n <= training_n) {
		job.start_offset = ftell(csv);
		job.start_line = line_n;

		step = min(BATCH_SIZE, training_n - line_n + 1);
		 for (i = 1; i <= step; i++)
		 	fgets(line, sizeof(line), csv);

		line_n += step;
		job.end_line = line_n - 1;

		submit_job(&job);
	}

	sleep(3);
	exit(0);

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

		prediction = loregression_predict(model, spec1, spec2);

		//DEBUG:
		//printf("%s,%s (%s) => %d\n", left_spec, right_spec, label, prediction);

		if (prediction == atoi(label))
			hits++;
	}

	printf("Prediction accuracy: %5.2f%% (%ld hits)\n",
		hits / (double) set_n * 100.0, hits);

	return 0;
}
