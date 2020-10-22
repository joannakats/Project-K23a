#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "hashtable.h"

int print_usage(const char *program) {
	fprintf(stderr,
		"Usage: %s "
		"-e hash_table_entries "
		"-w path_to_dataset_w_(csv) "
		"-x path_to_dataset_x_(directory)\n",
		program);
	return -1;
}

int get_opts(int argc, char *argv[], int *entries, char **dataset_x, char **dataset_w) {
	int opt;
	struct stat statbuf;

	/* Zero-initialized to check if <= 0 later */
	*entries = 0;

	if (argc < 7)
		return print_usage(argv[0]);

	while ((opt = getopt(argc, argv, "e:x:w:")) != -1) {
		switch (opt) {
		case 'e':
			*entries = atoi(optarg);
			break;
		case 'w':
			*dataset_w = strdup(optarg);
			break;
		case 'x':
			*dataset_x = strdup(optarg);
			break;
		default:
			return print_usage(argv[0]);
		}
	}

	/* Check values */
	if (*entries <= 0) {
		fputs("entries must be > 0!\n", stderr);
		return -1;
	}

	if (access(*dataset_x, F_OK) == -1) {
		perror(*dataset_x);
		return -2;
	}

	stat(*dataset_x, &statbuf);
	if (!S_ISDIR(statbuf.st_mode)) {
		fputs("Dataset X must be a directory!\n", stderr);
		return -2;
	}

	if (access(*dataset_w, F_OK) == -1) {
		perror(*dataset_w);
		return -2;
	}

	stat(*dataset_w, &statbuf);
	if (!S_ISREG(statbuf.st_mode)) {
		fputs("Dataset W must be a (csv) file!\n", stderr);
		return -2;
	}

	return 0;
}

int main(int argc, char *argv[]) {
	int ret = 0;

	/* args: returned from parsing function get_opts */
	int entries = 0;
	char *dataset_x = NULL, *dataset_w = NULL;

	if ((ret = get_opts(argc, argv, &entries, &dataset_w, &dataset_x)))
		return ret;

	// Test
	printf("e: %d, w: %s, x: %s\n", entries, dataset_w, dataset_x);

	// ht init goes here

	free(dataset_w);
	free(dataset_x);
}
