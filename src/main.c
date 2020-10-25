#include <dirent.h>
#include <getopt.h>
#include <libgen.h>
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

int get_json(char *path, int *spec_field_count, char ***spec_properties, char ***spec_values) {
	FILE *json;
	char buffer[2048], *property, *value, *saveptr;

	/* Initialize as NULL, old arrays are part of spec nodes now */
	*spec_properties = NULL, *spec_values = NULL;
	*spec_field_count = 0;

	if (!(json = fopen(path, "r"))) {
		perror(path);
		return -1;
	}

	while (fgets(buffer, sizeof(buffer), json)) {
		if (buffer[0] == '{' || buffer[0] == '}')
			continue;

		/* New field entry, resize arrays */
		(*spec_field_count)++;
		*spec_properties = realloc(*spec_properties, *spec_field_count * sizeof(**spec_properties));
		*spec_values = realloc(*spec_values, *spec_field_count * sizeof(**spec_values));

		/* Save field line [property, value] in parallel arrays */
		strtok_r(buffer, "\"", &saveptr);            /* 1. Whitespace */
		property = strtok_r(NULL, "\"", &saveptr);     /* 2. Property */
		strtok_r(NULL, "\"", &saveptr);               /* 3. Semicolon */
		value = strtok_r(NULL, "\"", &saveptr);           /* 4. Value */

		(*spec_properties)[*spec_field_count - 1] = strdup(property);
		(*spec_values)[*spec_field_count - 1] = strdup(value);
	}

	// Test print
	/* for (int i = 0; i < *spec_field_count; ++i)
		printf("%s | %s\n", (*spec_properties)[i], (*spec_values)[i]); */

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

		/* buf is the full path to the current dir entry (folder or .json) */
		sprintf(buf, "%s/%s", path, dirent->d_name);

		if (dirent->d_type == DT_DIR) {
			insert_specs(hash_table, buf);
		} else {
			/* Read spec properties from json */
			get_json(buf, &spec_field_count, &spec_properties, &spec_values);

			/* Get spec id (e.g. buy.net//10) */
			sprintf(buf, "%s//%s", basename(path), dirent->d_name);
			*strrchr(buf, '.') = '\0'; /* Remove (.json) extension from id */
			spec_id = strdup(buf);

			// Print id
			puts(spec_id);

			/* Ready for hashtable insertion */
			//insert_entry(hash_table, spec_id, spec_field_count, spec_properties, spec_values);
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

	hashtable hash_table;

	if ((ret = get_opts(argc, argv, &entries, &dataset_x, &dataset_w)))
		return ret;

	// Print arguments
	printf("e: %d, w: %s, x: %s\n", entries, dataset_w, dataset_x);

	hash_table = hashtable_init(entries);

	// The (spec) fun begins:
	insert_specs(&hash_table, dataset_x);
	/* TODO: Update with join data from Dataset W */

	delete_hashtable(&hash_table);

	free(dataset_w);
	free(dataset_x);
}
