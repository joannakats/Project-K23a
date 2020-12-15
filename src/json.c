#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "json.h"

static int parse_json_field(FILE *json, char *line, hashtable *fields) {
	char *property, *value, *temp = NULL;

	field *current_field;

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
	current_field = HSfield_insert(fields, property);

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

		setValue(current_field, value);
	} else {
		/* We have a JSON array[] in our hands!
		 * e.g. "color" : [
		 *    "red",
		 *    "blue"
		 * ] */

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

			setValue(current_field, value);
		}
	}

	return 0;
}

int read_spec_from_json(char *path, hashtable **spec_fields) {
	FILE *json;
	char *line;
	//int error = 0;

	// TODO: Remove Debug: Print json being parsed
	//printf("%s\n", path);

	/* Initialize hashtable of fields and counter */
	*spec_fields = field_init(FIELD_TABLE_SIZE);

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

		// /* Only add new field if previous was filled */
		// if (!error) {
		// 	/* New field entry, resize arrays */
		// 	(*spec_field_count)++;
		// }

		parse_json_field(json, line, *spec_fields);

		/* Add field to global vocabulary (making a vector) */
		// bag_of_words(current_field->property, vocabulary);
		// for (int i = 0; i < current_field->cnt; ++i)
		// 	bag_of_words(current_field->values[i], vocabulary);
	}

	fclose(json);
	free(line);

	return 0;
}
