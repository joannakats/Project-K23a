#include <errno.h>
#include <stdio.h>
#include <sys/sysinfo.h>

#include "common.h"
#include "prediction.h"
#include "spec_hashtable.h"
#include "thread_pool.h"

logistic_regression *model;

logistic_regression *get_model() {
	return model;
}

int prediction_init(bow *vocabulary) {
	model = loregression_init(vocabulary->ht.count);

	if (!model)
		return errno;

	// Chose threads according to amount of cores, because why not
	jobsch_init(model, min(get_nprocs(), 16));

	return 0;
}

void prediction_destroy() {
	jobsch_destroy();
	loregression_delete(model);
}

int prediction_training(FILE *csv, int training_n, hashtable *specs) {
	char line[512];
	long i, line_n, batch_size;
	char *left_spec, *right_spec, *label_str, *saveptr;

	node *spec[2];
	int pos, label;

	// Split training dataset into mini-batches, run training
	for (line_n = 1; line_n <= training_n; line_n += batch_size) {
		batch_size = min(BATCH_SIZE, training_n - line_n + 1);

		// Make batch
		for (i = 0; i < batch_size; i++) {
		 	if (!fgets(line, sizeof(line), csv)) {
				perror("Reading Dataset W for training");
				return errno;
			}

			left_spec = strtok_r(line, ",", &saveptr);
			right_spec = strtok_r(NULL, ",", &saveptr);
			label_str = strtok_r(NULL, ",\n", &saveptr);

			spec[0] = search_hashTable_spec(specs, left_spec, &pos);
			spec[1] = search_hashTable_spec(specs, right_spec, &pos);
			label = atoi(label_str);

			batch_push(spec[0], spec[1], label);
		}

		/* Train with this batch (will be done in parallel) */
		run_batch(train);
	}

	return 0;
}

int prediction_hits(FILE *csv, int set_n, hashtable *specs) {
	char line[512];
	long line_n,batch_size=0,i;
	char *left_spec, *right_spec, *labelptr, *saveptr;

	node *spec1, *spec2;
	int pos,label;

	long hits;

	// Split  dataset into mini-batches same as training
	for (line_n = 1; line_n <= set_n; line_n += batch_size) {
		batch_size = min(BATCH_SIZE, set_n - line_n + 1);

		// Make batch
		for (i = 0; i < batch_size; i++) {
		 	if (!fgets(line, sizeof(line), csv)) {
				perror("Reading Dataset W for accuracy");
				return errno;
			}

			left_spec = strtok_r(line, ",", &saveptr);
			right_spec = strtok_r(NULL, ",", &saveptr);
			labelptr = strtok_r(NULL, ",\n", &saveptr);

			spec1 = search_hashTable_spec(specs, left_spec, &pos);
			spec2 = search_hashTable_spec(specs, right_spec, &pos);
			label = atoi(labelptr);

			//printf("%s,%s (%d) => %ld bsize\n", left_spec, right_spec, label,batch_size );

			batch_push(spec1, spec2, label);
		}

		/* Count hits with this batch (will be done in parallel) */
		run_batch(test);
	}

	hits = get_hits();
	printf("Prediction accuracy: %5.2f%% (%ld hits)\n", hits / (double) set_n * 100.0, hits);

	return 0;
}
