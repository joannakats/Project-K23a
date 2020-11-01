#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "operations.h"

int parse_json_field(FILE *json, char *line, field *current_field) {
	char *property, *value, *saveptr;

	/* Parse field line */
	strtok_r(line, "\"", &saveptr);    /* 1. Whitespace before "property" */

	/* 2. Property
	 * Quirk: If the property field is empty (""), such as in
	 * www.camerafarm.com.au/758.json, we can detect that because saveptr
	 * will start from the closing quote of the empty property field */
	if (saveptr[0] == '"')
		property = "";
	else
		property = strtok_r(NULL, "\"", &saveptr);

	strtok_r(NULL, "\"", &saveptr);                            /* 3. ": " */

	/* At this point if saveptr still points to remaining data, there exists
	 * a single quoted string value. Otherwise the remainder of the line was
	 * a starting bracket for a JSON array and saveptr will be empty, after
	 * exhausting the search for the quotes around "value" */

	/* Set current field property and value count, initially = 1 */
	setField(current_field, 1, property);

	if (saveptr[0]) {
		/* Single quoted "value" string
		 * e.g. "battery": "Li-ion" */
		*strrchr(saveptr, '"') = '\0';
		value = saveptr;                                  /* 4. Value */

		setValue(current_field, 0, value);
	} else {
		/* We have a JSON array[] in our hands!
		 * e.g. "color" : [
		 *    "red",
		 *    "blue"
		 * ] */

		/* Set to zero, incremented to 1 in the loop */
		current_field->cnt = 0;
		while (fgets(line, LINE_SIZE, json)) {
			/* Similar strategy to single string value:
			 * 1. Whitespace before "array member" */
			strtok_r(line, "\"", &saveptr);

			/* A line with a single bracket ] denotes the end of the array.
			 * saveptr will be an empty string because no quote was found */
			if (!saveptr[0])
				break;

			*strrchr(saveptr, '"') = '\0';
			value = saveptr;                   /* 2. Array member */

			current_field->cnt++;
			current_field->values = realloc(current_field->values, current_field->cnt * sizeof(current_field->values[0]));

			setValue(current_field, current_field->cnt - 1, value);
		}
	}

	return 0;
}

int read_spec_from_json(char *path, int *spec_field_count, field **spec_fields) {
	FILE *json;
	char *line;

	field *current_field;

	/* Initialize as NULL, old field structs are part of spec nodes now */
	*spec_fields = NULL;
	*spec_field_count = 0;

	if (!(line = malloc(LINE_SIZE))) {
		perror("line buffer");
		return errno;
	};

	if (!(json = fopen(path, "r"))) {
		perror(path);
		return errno;
	}

	while (fgets(line, LINE_SIZE, json)) {
		if (line[0] == '{' || line[0] == '}')
			continue;

		/* New field entry, resize arrays */
		(*spec_field_count)++;
		*spec_fields = realloc(*spec_fields, *spec_field_count * sizeof(**spec_fields));

		if (!*spec_fields) {
			perror("spec_fields");
			return errno;
		}

		current_field = &(*spec_fields)[*spec_field_count - 1];
		parse_json_field(json, line, current_field);
	}

	fclose(json);
	free(line);

	return 0;
}

/* Reading Dataset X */
int insert_specs(hashtable *hash_table, char *path) {
	DIR *dir;
	struct dirent *dirent;

	char buf[512];

	char *spec_id;
	int spec_field_count;
	field *spec_fields;

	if (!(dir = opendir(path))) {
		perror(path);
		return errno;
	}

	while ((dirent = readdir(dir))) {
		/* Skip dot-files/folders, like ".." */
		if (dirent->d_name[0] == '.')
			continue;

		/* buf is the full path to the current dir entry (folder or .json) */
		sprintf(buf, "%s/%s", path, dirent->d_name);

		/* Recurse into site (e.g. www.ebay.com) subdirectories
		 * NOTE: DT_UNKNOWN for some filesystems */
		if (dirent->d_type == DT_DIR) {
			insert_specs(hash_table, buf);
		} else {
			read_spec_from_json(buf, &spec_field_count, &spec_fields);

			/* Create spec id (e.g. buy.net//10) */
			sprintf(buf, "%s//%s", basename(path), dirent->d_name);
			*strrchr(buf, '.') = '\0'; /* Remove (.json) extension from id */
			spec_id = buf;

			/* Ready for hashtable insertion */
			insert_entry(hash_table, spec_id, spec_fields, spec_field_count);
		}
	}

	closedir(dir);

	return 0;
}

/* Reading Dataset W */
int join_specs(hashtable *hash_table, char *dataset_w) {
	FILE *csv;

	char line[512];
	char *left_spec, *right_spec, *label, *saveptr;

	if (!(csv = fopen(dataset_w, "r"))) {
		perror(dataset_w);
		return errno;
	}

	fgets(line, sizeof(line), csv); /* Skip first line (column titles) */

	while (fgets(line, sizeof(line), csv)) {
		left_spec = strtok_r(line, ",", &saveptr);
		right_spec = strtok_r(NULL, ",", &saveptr);
		label = strtok_r(NULL, ",", &saveptr);

		/* We only care about cliques */
		if (label[0] == '1')
			hash_table_join(hash_table, left_spec, right_spec);
	}

	fclose(csv);

	return 0;
}

int print_pairs_csv(hashtable *hash_table, char *output) {
	FILE *output_csv;

	if (!(output_csv = freopen(output, "w", stdout))) {
		perror(output);
		return errno;
	}

	puts("left_spec_id,right_spec_id");
	print_pairs(hash_table);

	freopen("/dev/tty", "w", stdout);

	return 0;
}

int begin_operations(int entries, char *output, char *dataset_x, char *dataset_w) {
	int ret = 0;

	/* Part 1 operations */
	hashtable hash_table = hashtable_init(entries);
	if (!hash_table.list)
		return -3;

	puts("Reading Dataset X...");
	if (!(ret = insert_specs(&hash_table, dataset_x))) {
		puts("Reading Dataset W...");
		if (!(ret = join_specs(&hash_table, dataset_w))) {
			puts("Writing output csv...");
			ret = print_pairs_csv(&hash_table, output);
		}
	}

	/* Cleanup */
	delete_hashtable(&hash_table);

	return ret;
}
