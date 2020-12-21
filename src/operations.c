#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "json.h"
#include "loregression.h"
#include "operations.h"
#include "prediction.h"
#include "preprocessing.h"
#include "vocabulary.h"

/* Phase 1 - Insert Dataset X in data structures */
int insert_dataset_x(hashtable *hash_table, char *dataset_x, bow *vocabulary) {
	DIR *dir;
	struct dirent *dirent;

	char path[1024], *extention;

	char *spec_id;
	hashtable *spec_fields = NULL;

	if (!(dir = opendir(dataset_x))) {
		perror(dataset_x);
		return errno;
	}

	/* The path buffer will be modified for each spec, to get the ID format */
	spec_id = path;

	while ((dirent = readdir(dir))) {
		/* Skip dot-files/folders, like ".." */
		if (dirent->d_name[0] == '.')
			continue;

		/* Get the full path to the current dir entry (folder or .json) */
		sprintf(path, "%s/%s", dataset_x, dirent->d_name);

		/* Recurse into site (e.g. www.ebay.com) subdirectories
		 * NOTE: DT_UNKNOWN for some filesystems */
		if (dirent->d_type == DT_DIR) {
			insert_dataset_x(hash_table, path, vocabulary);
		} else {
			/* Skip non-json files */
			if (!(extention = strrchr(path, '.')))
				continue;
			else if (strcmp(extention, ".json"))
				continue;

			read_spec_from_json(path, &spec_fields);

			/* Create spec id (e.g. buy.net//10) */
			sprintf(spec_id, "%s//%s", basename(dataset_x), dirent->d_name);
			/* Remove extension to get final id */
			*strrchr(spec_id, '.') = '\0';

			/* Ready for hashtable insertion */

			/* GAME TIME: Ready for insertion in structs hashtable, and Bag of Words */
			insert_entry(hash_table, spec_id, &spec_fields);
			preprocessing_insert(vocabulary, spec_fields);
		}
	}

	closedir(dir);

	return 0;
}

/* Phase 2 - Join specs from Dataset W  */
int insert_dataset_w(hashtable *hash_table, char *dataset_w) {
	char line[512];
	char *left_spec, *right_spec, *label, *saveptr;

	FILE *csv;

	if (!(csv = fopen(dataset_w, "r"))) {
		perror(dataset_w);
		return errno;
	}

	if (!fgets(line, sizeof(line), csv)) {
		perror("Dataset W is empty");
		return errno;
	}

	while (fgets(line, sizeof(line), csv)) {
		//DEBUG:
		//puts(line);

		left_spec = strtok_r(line, ",", &saveptr);
		right_spec = strtok_r(NULL, ",", &saveptr);
		label = strtok_r(NULL, ",", &saveptr);

		if (label[0] == '1')
			hash_table_join(hash_table, left_spec, right_spec);
		else /* label is 0: anti_clique time */
			hash_table_notjoin(hash_table, left_spec, right_spec);
	}

	fclose(csv);
	return 0;
}

/* Phase 3 - Expand the dataset with all the derived relations
 * and partition for training */
static int partition_dataset_w(hashtable *hash_table, char *dataset_w, bow *vocabulary) {
	FILE *expanded;

	/* Partitioning lines */
	long training_n, validation_n, test_n;
	long line_total = 0;

	logistic_regression *model;

	// TODO: REMOVE THIS AFTER print implemented
	return 0;

	if (!(expanded = fopen("expanded.csv", "w"))) {
		perror(dataset_w);
		return errno;
	}

	//TODO: Make expanded set with all cliques - anticliques (function)
	/* Count data relation lines, to split 60-20-20
	while (fgets(line, sizeof(line), csv))
		line_total++;

	if (line_total <= 0) {
		perror("dataset_w is empty");
		fclose(csv);

		return errno;
	}; */

	/* Expanded Dataset W split in 3 */
	training_n = line_total * 60 / 100;
	validation_n = line_total * 20 / 100;
	test_n = line_total - training_n - validation_n;

	/* Partition expanded dataset */
	/* TODO: init expanded dataset here? */
	model = prediction_init(vocabulary);

	/* 1: Training */
	fputs("Training model...\n", stderr);
	prediction_training(expanded, training_n, hash_table, model);

	/* 2: Validation set */
	fputs("Validation set...\n", stderr);
	prediction_validation(expanded, validation_n, hash_table, model);

	/* 3: Test set (We're not going for epochs right now) */
	fputs("Testing set...\n", stderr);
	prediction_test(expanded, test_n, hash_table, model);

	/* TODO: maybe prediction destroy? */
	loregression_delete(model);

	fclose(expanded);
	return 0;
}

/* Phase 4 - Output */
static int print_pairs_csv(hashtable *hash_table, char *output) {
	FILE *output_csv;

	/* Print to stdout by default */
	if (output) {
		if (!(output_csv = freopen(output, "w", stdout))) {
			perror(output);
			return errno;
		}
	}

	puts("left_spec_id,right_spec_id");
	print_pairs(hash_table);

	if (output) {
		if (!freopen("/dev/tty", "w", stdout)) {
			perror("freopen() for stdout");
			return errno;
		}
	}

	return 0;
}

int begin_operations(int entries, char *dataset_x, char *dataset_w, char *output) {
	int ret = 0;
	/* Project Data Structures */
	hashtable hash_table;
	bow *vocabulary;

	hash_table = hashtable_init(entries);
	if (!hash_table.list)
		return errno;

	if (!(vocabulary = bow_init(entries))) {
		fputs("error in bow_init!\n", stderr);
		return errno;
	}

	preprocessing_init("stopwords.txt");

	fputs("[Reading Dataset X]\n", stderr);
	if (!(ret = insert_dataset_x(&hash_table, dataset_x, vocabulary))) {
		// DEBUG: print global vocabulary size
		printf("Distinct words: %d\n", vocabulary->ht.count);

		fputs("[Preprocessing specs]\n", stderr);
		if (!(ret = preprocessing_specs(&hash_table, vocabulary, true))) {
			printf("Distinct words after trim: %d\n", vocabulary->ht.count);

			fputs("[Reading Dataset W]\n", stderr);
			if (!(ret = insert_dataset_w(&hash_table, dataset_w))) {

				fputs("[Expanding Dataset W]\n", stderr);
				if (!(ret = partition_dataset_w(&hash_table, dataset_w, vocabulary))) {

					fputs("[Writing output csv (cliques)]\n", stderr);
					ret = print_pairs_csv(&hash_table, output);
				}
			}
		}
	}

	/* Cleanup */
	preprocessing_destroy();
	bow_delete(vocabulary);
	delete_hashtable(&hash_table);

	return ret;
}
