#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

#define BUFFER_SIZE 8192

/* TODO: Spec structure subject to change */
int read_spec_from_json(char *path, int *spec_field_count, char ***spec_properties, char ***spec_values) {
	FILE *json;
	char *buffer, *property, *value, *saveptr;

	/* Initialize as NULL, old arrays are part of spec nodes now */
	*spec_properties = NULL, *spec_values = NULL;
	*spec_field_count = 0;

	if (!(buffer = malloc(BUFFER_SIZE))) {
		fputs("Couldn't allocate space for buffer", stderr);
		return -1;
	};

	if (!(json = fopen(path, "r"))) {
		perror(path);
		return -2;
	}

	while (fgets(buffer, BUFFER_SIZE, json)) {
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

		// Test print
		printf("[%d] %s = %s\n", *spec_field_count, property, value);

		(*spec_properties)[*spec_field_count - 1] = strdup(property);
		(*spec_values)[*spec_field_count - 1] = strdup(value);
	}

	fclose(json);
	free(buffer);

	return 0;
}

/* Reading Dataset X */
int insert_specs(hashtable *hash_table, char *path) {
	DIR *dir;
	struct dirent *dirent;

	char buf[512];

	char *spec_id;
	int spec_field_count;
	char **spec_properties, **spec_values;

	dir = opendir(path);

	while ((dirent = readdir(dir))) {
		/* Skip dot-files/folders, like ".." */
		if (dirent->d_name[0] == '.')
			continue;

		/* buf is the full path to the current dir entry (folder or .json) */
		sprintf(buf, "%s/%s", path, dirent->d_name);

		/* Recurse into site (e.g. www.ebay.com) subdirectories
		 * NOTE: DT_UNKOWN for some filesystems */
		if (dirent->d_type == DT_DIR) {
			insert_specs(hash_table, buf);
		} else {
			read_spec_from_json(buf, &spec_field_count, &spec_properties, &spec_values);

			/* Create spec id (e.g. buy.net//10) */
			sprintf(buf, "%s//%s", basename(path), dirent->d_name);
			*strrchr(buf, '.') = '\0'; /* Remove (.json) extension from id */
			spec_id = buf;

			// Print id
			printf("%s\n\n", spec_id);

			/* Ready for hashtable insertion */
			/* TODO: Enable after fields revamp */
			//insert_entry(hash_table, spec_id, spec_field_count, spec_properties, spec_values);
		}
	}

	closedir(dir);

	return 0;
}

/* Reading Dataset W */
int join_specs(hashtable *hash_table, char *dataset_w) {
	FILE *csv;

	char buffer[512];
	char *left_spec, *right_spec, *label, *saveptr;

	if (!(csv = fopen(dataset_w, "r"))) {
		perror(dataset_w);
		return -1;
	}

	fgets(buffer, sizeof(buffer), csv); /* Skip first line (column titles) */

	while (fgets(buffer, sizeof(buffer), csv)) {
		left_spec = strtok_r(buffer, ",", &saveptr);
		right_spec = strtok_r(NULL, ",", &saveptr);
		label = strtok_r(NULL, ",", &saveptr);

		/* We only care about cliques */
		if (label[0] == '1') {
			// TODO: hash_table_join (left, right)

			// Print
			printf("%s <=> %s\n", left_spec, right_spec);
		}
	}

	fclose(csv);

	return 0;
}

int begin_operations(int entries, char *output, char *dataset_x, char *dataset_w) {
	int ret = 0;

	/* Part 1 operations */
	hashtable hash_table = hashtable_init(entries);
	if (!hash_table.list)
		return -3;

	if (!(ret = insert_specs(&hash_table, dataset_x))) {
		ret = join_specs(&hash_table, dataset_w);
	}

	/* Cleanup */
	delete_hashtable(&hash_table);

	return ret;
}
