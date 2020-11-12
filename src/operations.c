#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "operations.h"

int parse_json_field(FILE *json, char *line, field *current_field) {
	char *property, *value, *temp = NULL;

	if (!line)
		return -1;

	/* Find opening quote of property string */
	strtok_r(line, "\"", &property);
	if (!property[0]) {
		fputs("No property field found", stderr);
		return -1;
	}

	/* Find closing quote of property string */
	if (!(temp = strstr(property, "\":"))) {
		fputs("Incomplete property field", stderr);
		return -1;
	}
	temp[0] = '\0';                  /* Cut property at the closing quote */

	/* Set current field property and value count, initially = 1 */
	setField(current_field, 1, property);

	/* Find opening quote of value string */
	strtok_r(temp + 1, "\"", &value);

	/* If we find a quote after the colon, this is a case of "string value"
	 * (e.g. "battery": "Li-ion") */
	if (value[0]) {
		/* Find closing quote of property string */
		if (!(temp = strrchr(value, '"'))) {
			fputs("Incomplete value field", stderr);
			value = "";
		}
		temp[0] = '\0';             /* Cut value at the closing quote */

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
			/* Find opening quote of array member string */
			strtok_r(line, "\"", &value);

			/* A line with a single bracket ] denotes the end of the array.
			 * value will be an empty string because no quote was found */
			if (!value[0])
				break;

			/* Find closing quote of array member string */
			if (!(temp = strrchr(value, '"'))) {
				fputs("Incomplete value field", stderr);
				value = "";
			}
			temp[0] = '\0';     /* Cut value at the closing quote */

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
	int error = 0;

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

		/* Only add new field if previous was filled */
		if (!error) {
			/* New field entry, resize arrays */
			(*spec_field_count)++;
			*spec_fields = realloc(*spec_fields, *spec_field_count * sizeof(**spec_fields));

			if (!*spec_fields) {
				perror("spec_fields");
				return errno;
			}

			current_field = &(*spec_fields)[*spec_field_count - 1];
		}

		error = parse_json_field(json, line, current_field);
	}

	fclose(json);
	free(line);

	return 0;
}

/* Reading Dataset X */
int insert_specs(hashtable *hash_table, char *dataset_x) {
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
			insert_specs(hash_table, path);
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

/* Reading Dataset W */
int join_specs(hashtable *hash_table, char *dataset_w) {
	FILE *csv;

	char line[512];
	char *left_spec, *right_spec, *label, *saveptr;

	if (!(csv = fopen(dataset_w, "r"))) {
		perror(dataset_w);
		return errno;
	}

	/* Skip first line (column titles) */
	if (!fgets(line, sizeof(line), csv)) {
		perror("dataset_w is empty");
		return errno;
	};

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

int begin_operations(int entries, char *output, char *dataset_x, char *dataset_w) {
	int ret = 0;

	/* Part 1 operations */
	hashtable hash_table = hashtable_init(entries);
	if (!hash_table.list)
		return -3;

	fputs("Reading Dataset X...\n", stderr);
	if (!(ret = insert_specs(&hash_table, dataset_x))) {
		fputs("Reading Dataset W...\n", stderr);
		if (!(ret = join_specs(&hash_table, dataset_w))) {
			fputs("Writing output csv...\n", stderr);
			ret = print_pairs_csv(&hash_table, output);
		}
	}

	/* Cleanup */
	delete_hashtable(&hash_table);

	return ret;
}
