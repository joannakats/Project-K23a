#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "json.h"

static int parse_json_field(FILE *json, char *line, field *current_field) {
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

	//dictionary spec_dictionary = hashtable_init(100);
	// TODO: Remove Debug: Print json being parsed
	//printf("%s\n", path);

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

		/* bag_of_words(current_field->property, &spec_dictionary);
		for (int i = 0; i < current_field->cnt; ++i)
			bag_of_words(current_field->values[i], &spec_dictionary); */
	}

	/* Print BoW
	word_node *w;
	for (int i = 0; i < spec_dictionary.tableSize; ++i) {
		w = spec_dictionary.list[i];
		while (w) {
			printf("%s\t%d\n", w->word, w->count);
			w = w->next;
		}
	} */

	fclose(json);
	free(line);

	return 0;
}
