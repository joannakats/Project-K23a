/* Functional testing:
 * INSERTION METHOD TEST
 *
 * This tests the insertion code, regarding the correct transfer of the spec
 * fields (e.g. <page_title>) into memory, comparing the original json to one
 * created from the contents of the structures in memory.
 *
 * Included in the dataset are examples of JSON files with arrays, or empty
 * property fields. ("": "value")
 */

#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "acutest.h"
#include "common.h"
#include "hashtable.h"
#include "operations.h"

char *make_json(node *spec) {
	char *tmp_filename;
	int fd;
	FILE *tmp_json;

	tmp_filename = strdup("/tmp/spec.XXXXXX");

	if ((fd = mkstemp(tmp_filename)) == -1) {
		perror("tmp_json");
		return NULL;
	}

	if (!(tmp_json = fdopen(fd, "w"))) {
		perror(tmp_filename);
		return NULL;
	}

	/* Write spec, formatted as JSON */
	fputs("{\n", tmp_json);

	for (int i = 0; i < spec->fieldCount; ++i) {
		fprintf(tmp_json, "    \"%s\": ", spec->fields[i].property);

		if (spec->fields[i].cnt == 1) {
			fprintf(tmp_json, "\"%s\"", spec->fields[i].values[0]);
		} else {
			fputs("[\n", tmp_json);

			for (int j = 0; j < spec->fields[i].cnt; j++) {
				fprintf(tmp_json, "        \"%s\"", spec->fields[i].values[j]);

				if (j < spec->fields[i].cnt - 1)
					fputs(",\n", tmp_json);
				else
					fputs("\n    ]", tmp_json);
			}
		}

		if (i < spec->fieldCount - 1)
			fputs(",\n", tmp_json);
		else
			fputc('\n', tmp_json);
	}

	fputc('}', tmp_json);

	fclose(tmp_json);

	return tmp_filename;
}

int compare_json(char *json_filename, char *tmp_filename) {
	FILE *json, *tmp_json;
	char *line, *tmp_line;

	if (!(line = malloc(LINE_SIZE))) {
		perror("line");
		return errno;
	}

	if (!(tmp_line = malloc(LINE_SIZE))) {
		perror("tmp_line");
		return errno;
	}

	if (!(json = fopen(json_filename, "r"))) {
		perror(json_filename);
		return errno;
	}

	if (!(tmp_json = fopen(tmp_filename, "r"))) {
		perror(tmp_filename);
		return errno;
	}

	/* Line-by-line comparison */
	while (fgets(line, LINE_SIZE, json)) {
		TEST_CHECK(fgets(tmp_line, LINE_SIZE, tmp_json) != NULL);
		TEST_MSG("Unexpected EOF in tmp_json!");

		TEST_CHECK(!strcmp(line, tmp_line));
		TEST_MSG("Expected: %s\nGot: %s", line, tmp_line);
	}

	free(line);
	free(tmp_line);
	fclose(json);
	fclose(tmp_json);

	return 0;
}

void test_json_insertion(void) {
	hashtable hash_table = hashtable_init(5);
	node* spec;
	char *dataset_x = "json_insertion/dataset_x";

	char json_filename[1024], *tmp_filename;

	printf("Dataset X at: %s\n", dataset_x);

	TEST_CHECK(insert_specs(&hash_table, dataset_x) == 0);

	/* For every spec in the hash_table, check against the original JSON */
	for (int i = 0; i < hash_table.tableSize ; ++i) {
		spec = hash_table.list[i];

		while (spec) {
			sprintf(json_filename, "%s/%s.json", dataset_x, spec->id);
			tmp_filename = make_json(spec);

			printf("Comparing %s\n", spec->id);
			compare_json(json_filename, tmp_filename);

			unlink(tmp_filename);
			free(tmp_filename);

			spec = spec->next;
		}
	}

	delete_hashtable(&hash_table);
}

TEST_LIST = {
	{"json_insertion", test_json_insertion},
	{NULL, NULL}
};
