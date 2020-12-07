#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "json.h"
#include "operations.h"
#include "training.h"

/* Phase 1 - Insert Dataset X in data structures */
int insert_dataset_x(hashtable *hash_table, char *dataset_x) {
	DIR *dir;
	struct dirent *dirent;

	char path[1024], *extention;

	char *spec_id;
	int spec_field_count;
	field *spec_fields;

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
			insert_dataset_x	(hash_table, path);
		} else {
			/* Skip non-json files */
			if (!(extention = strrchr(path, '.')))
				continue;
			else if (strcmp(extention, ".json"))
				continue;

			read_spec_from_json(path, &spec_field_count, &spec_fields);

			/* Create spec id (e.g. buy.net//10) */
			sprintf(spec_id, "%s//%s", basename(dataset_x), dirent->d_name);
			/* Remove extension to get final id */
			*strrchr(spec_id, '.') = '\0';

			/* Ready for hashtable insertion */
			insert_entry(hash_table, spec_id, spec_fields, spec_field_count);
		}
	}

	closedir(dir);

	return 0;
}

/* Phase 2 - Join specs from Dataset W  */
int relate_specs(hashtable *hash_table, FILE *csv, long training_n) {
	long line_n;
	char line[512];
	char *left_spec, *right_spec, *label, *saveptr;

	/* Skip first line (left, right, label) */
	fgets(line, sizeof(line), csv);
	for (line_n = 1; line_n <= training_n; line_n++) {
		fgets(line, sizeof(line), csv);

		left_spec = strtok_r(line, ",", &saveptr);
		right_spec = strtok_r(NULL, ",", &saveptr);
		label = strtok_r(NULL, ",", &saveptr);

		if (label[0] == '1') {
			printf("%s,%s\n", left_spec, right_spec);
			hash_table_join(hash_table, left_spec, right_spec);
		}
		//TODO: Negative relations
		//else
		//	hash_table_NOTjoin(hash_table, left_spec, right_spec);
	}

	return 0;
}

static int parse_dataset_w(hashtable *hash_table, char *dataset_w) {
	FILE *csv;
	char line[512];

	/* Don't count the header line in total */
	long training_n, validation_n, test_n, line_total = -1;

	if (!(csv = fopen(dataset_w, "r"))) {
		perror(dataset_w);
		return errno;
	}

	/* Find total lines, to split 60-20-20 */
	while (fgets(line, sizeof(line), csv))
		line_total++;

	if (line_total <= 0) {
		perror("dataset_w is empty");
		fclose(csv);

		return errno;
	};

	/* Dataset W split in 3 */
	training_n = line_total * 60 / 100;
	validation_n = line_total * 20 / 100;
	test_n = line_total - training_n - validation_n;

	/* Parse Dataset W */
	rewind(csv);

	relate_specs(hash_table, csv, training_n);

	fclose(csv);

	return 0;
}

/* Phase 3 - Output */
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

	/* Part 1 operations */
	hashtable hash_table = hashtable_init(entries);
	if (!hash_table.list)
		return -3;

	fputs("Reading Dataset X...\n", stderr);
	if (!(ret = insert_dataset_x(&hash_table, dataset_x))) {
		fputs("Reading Dataset W...\n", stderr);
		if (!(ret = parse_dataset_w(&hash_table, dataset_w))) {
			fputs("Writing output csv...\n", stderr);
			ret = print_pairs_csv(&hash_table, output);
		}
	}

	/* Cleanup */
	delete_hashtable(&hash_table);

	return ret;
}
