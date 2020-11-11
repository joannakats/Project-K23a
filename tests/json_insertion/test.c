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

int make_tmp_json(node *spec, char *tmp_template) {
	int fd;
	FILE *tmp_json;

	fd = mkstemp(tmp_template);
	if (!TEST_CHECK(fd != -1)) {
		TEST_MSG("mkstemp: %s", strerror(errno));
		return errno;
	}

	tmp_json = fdopen(fd, "w");
	if (!TEST_CHECK(tmp_json != NULL)) {
		TEST_MSG("tmp_json: %s", strerror(errno));
		return errno;
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
	return 0;
}

int compare_json(char *json_filename, char *tmp_filename) {
	FILE *json, *tmp_json;
	char *line, *tmp_line;
	unsigned long line_n = 0;

	line = malloc(LINE_SIZE);
	if (!TEST_CHECK(line != NULL)) {
		TEST_MSG("line: %s", strerror(errno));
		return errno;
	}

	tmp_line = malloc(LINE_SIZE);
	if (!TEST_CHECK(tmp_line != NULL)) {
		TEST_MSG("tmp_line: %s", strerror(errno));
		return errno;
	}

	json = fopen(json_filename, "r");
	if (!TEST_CHECK(json != NULL)) {
		TEST_MSG("%s: %s", json_filename, strerror(errno));
		return errno;
	}

	tmp_json = fopen(tmp_filename, "r");
	if (!TEST_CHECK(tmp_json != NULL)) {
		TEST_MSG("%s: %s", tmp_filename, strerror(errno));
		return errno;
	}

	/* Line-by-line comparison */
	while (fgets(line, LINE_SIZE, json)) {
		line_n++;

		TEST_CHECK(fgets(tmp_line, LINE_SIZE, tmp_json) != NULL);
		TEST_MSG("%s:%lu:\nUnexpected EOF in tmp_json", json_filename, line_n);

		TEST_CHECK(!strcmp(line, tmp_line));
		TEST_MSG("%s:%lu:\nExpected: '%s'\nGot: '%s'", json_filename, line_n, line, tmp_line);
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
	//char *dataset_x = "../../Datasets/2013_camera_specs";

	char json_filename[1024];
	char tmp_template[] = "/tmp/spec.XXXXXX";

	TEST_ASSERT(tmp_template != NULL);
	TEST_MSG("tmp_template: %s", strerror(errno));

	printf("Dataset X at: %s\n", dataset_x);

	TEST_CHECK(!insert_specs(&hash_table, dataset_x));

	/* For every spec in the hash_table, check against the original JSON */
	for (int i = 0; i < hash_table.tableSize; ++i) {
		spec = hash_table.list[i];

		while (spec) {
			sprintf(json_filename, "%s/%s.json", dataset_x, spec->id);
			strcpy(tmp_template + 10, "XXXXXX");

			if (!make_tmp_json(spec, tmp_template)) {
				printf("Comparing %s\n", spec->id);
				compare_json(json_filename, tmp_template);

				unlink(tmp_template);
			}

			spec = spec->next;
		}
	}

	delete_hashtable(&hash_table);
}

TEST_LIST = {
	{"json_insertion", test_json_insertion},
	{NULL, NULL}
};
