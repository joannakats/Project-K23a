#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
#include "operations.h"

int print_usage(const char *path) {
	char *program = strdup(path);

	if (program) {
		fprintf(stderr,
			"Usage: %s [-e buckets (default: 10007)] -x dataset_x -w dataset_w [-o FILE (default: stdout)]\n",
			basename(program));
	}

	free(program);
	return -1337;
}

int get_opts(int argc, char *argv[], int *entries, char **output, char **dataset_x, char **dataset_w) {
	int opt;
	struct stat statbuf;

	*entries = 10007;    /* Default value for hashtable entries (buckets) */
	/* Zero-initialized to check if given values later */
	*output = NULL;
	*dataset_x = NULL;
	*dataset_w = NULL;

	while ((opt = getopt(argc, argv, "e:o:x:w:")) != -1) {
		switch (opt) {
		case 'e':
			*entries = atoi(optarg);
			break;
		case 'o':
			*output = strdup(optarg);
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

	/* Check that values have been given */
	if (*entries <= 0) {
		fputs("entries must be > 0!\n", stderr);
		return print_usage(argv[0]);
	} else if (!*dataset_w || !*dataset_x) {
		return print_usage(argv[0]);
	}

	/* Dataset W */
	if (access(*dataset_w, R_OK) == -1) {
		perror(*dataset_w);
		return errno;
	}

	stat(*dataset_w, &statbuf);
	if (!S_ISREG(statbuf.st_mode)) {
		fputs("Dataset W must be a (csv) file!\n", stderr);
		return print_usage(argv[0]);
	}

	/* Dataset X */
	if (access(*dataset_x, R_OK) == -1) {
		perror(*dataset_x);
		return errno;
	}

	stat(*dataset_x, &statbuf);
	if (!S_ISDIR(statbuf.st_mode)) {
		fputs("Dataset X must be a directory!\n", stderr);
		return print_usage(argv[0]);
	}

	return 0;
}

int main(int argc, char *argv[]) {
	int ret = 0;

	/* args: returned from parsing function get_opts */
	int entries;
	char *output, *dataset_x, *dataset_w;

	if (!(ret = get_opts(argc, argv, &entries, &output, &dataset_x, &dataset_w)))
		ret = begin_operations(entries, output, dataset_x, dataset_w);

	free(output);
	free(dataset_w);
	free(dataset_x);

	return ret;
}
