#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	int ret, opt;

	/* Zero-initialized to check that they've been given values later */
	*entries = 0;
	*dataset_x = NULL;
	*dataset_w = NULL;

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

	/* TODO: Test That X is a folder and W is a file */

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
