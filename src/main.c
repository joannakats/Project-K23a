#include <dirent.h>
#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//#include "hashtable.h"
typedef struct hashtable hashtable;

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

int insert_specs(hashtable *hash_table, char *path) {
	DIR *dir;
	struct dirent *dirent;

	char buf[512];

	// TODO: If no information hiding, node here
	char *spec_id;
	int spec_field_count;
	char **spec_properties, **spec_values;

	dir = opendir(path);

	while ((dirent = readdir(dir))) {
		/* Skip dot-files/folders, like ".." */
		if (dirent->d_name[0] == '.')
			continue;

		/* Recurse into site (e.g. www.ebay.com) subdirectories
		 * Note:Not all filesystems support d_type ! */
		if (dirent->d_type == DT_DIR) {
			sprintf(buf, "%s/%s", path, dirent->d_name);
			insert_specs(hash_table, buf);
		} else {
			/* Remove (.json) extention from filename */
			*strrchr(dirent->d_name, '.') = '\0';

			sprintf(buf, "%s//%s", basename(path), dirent->d_name);

			// Print id (e.g. buy.net//10)
			spec_id = buf;
			puts(spec_id);

			// TODO: Get fields, Insert in hashtable
		}
	}

	closedir(dir);

	return 0;
}

int main(int argc, char *argv[]) {
	int ret = 0;

	/* args: returned from parsing function get_opts */
	int entries = 0;
	char *dataset_x = NULL, *dataset_w = NULL;

	if ((ret = get_opts(argc, argv, &entries, &dataset_x, &dataset_w)))
		return ret;

	// Print arguments
	printf("e: %d, w: %s, x: %s\n", entries, dataset_w, dataset_x);

	/* TODO: ht init goes here */
	insert_specs(NULL, dataset_x);

	free(dataset_w);
	free(dataset_x);
}
