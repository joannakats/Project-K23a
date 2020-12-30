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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "acutest.h"
#include "common.h"
#include "hashtable.h"
#include "operations.h"
#include "preprocessing.h"

int make_tmp_json(node *spec, char *tmp_template, int *lines) {
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
	*lines = 2;

	int counter = 0;	//counter of fields written

	for (int i = 0; i < spec->fields->tableSize; i++) {
		field *f = spec->fields->list[i];

		while(f != NULL) {
			counter++;
			(*lines)++;
			fprintf(tmp_json, "    \"%s\": ", f->property);

			if (f->cnt == 1) {
				fprintf(tmp_json, "\"%s\"", f->values[0]);
			} else {
				fputs("[\n", tmp_json);

				for (int j = 0; j < f->cnt; j++) {
					fprintf(tmp_json, "        \"%s\"", f->values[j]);
					(*lines)++;
					if (j < f->cnt - 1){
						fputs(",\n", tmp_json);
					}else {
						fputs("\n    ]", tmp_json);
						(*lines)++;
					}
				}
			}

			fputs(",\n", tmp_json);

			f = f->next;
		}

	}

	fputc('}', tmp_json);

	fclose(tmp_json);
	return 0;
}

int compare_json(char *json_filename, char *tmp_filename, int n_lines) {
	FILE *json, *tmp_json;
	char *line, *tmp_line;

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

	int cnt=0;	//counter of lines checked
	/* Line-by-line comparison */
	while (fgets(line, LINE_SIZE, json)) {
		int flag = 0;
		cnt++;

		//in our temporary json file we expect ",\n" (for the sake of this test) on line (n_lines - 1)
		if (cnt == n_lines - 1) {
			int len = strlen(line);
			line[len-1] = ',';
			line[len] = '\n';
			line[len+1] = '\0';
		}

		fseek(tmp_json, 0, SEEK_SET);
		while (fgets(tmp_line, LINE_SIZE, tmp_json)) {
			if(strcmp(line, tmp_line) == 0) {
				flag = 1;
				break;
			}
		}

		TEST_CHECK(flag == 1);
		TEST_MSG("Data corrupted");

	}

	free(line);
	free(tmp_line);

	fclose(json);
	fclose(tmp_json);

	return 0;
}

void test_json_insertion(void) {
	hashtable hash_table = {0};
	bow *vocabulary = NULL;

	node* spec;
	char *dataset_x = "json_insertion/dataset_x";
	//char *dataset_x = "../../Datasets/2013_camera_specs";

	char json_filename[1024];
	char tmp_template[] = "/tmp/spec.XXXXXX";

	hash_table = hashtable_init(5);
	TEST_ASSERT(hash_table.list != NULL);

	vocabulary = bow_init(13);
	TEST_ASSERT(vocabulary != NULL);

	TEST_ASSERT(tmp_template != NULL);
	TEST_MSG("tmp_template: %s", strerror(errno));

	printf("Dataset X at: %s\n", dataset_x);

	TEST_ASSERT(!preprocessing_init("../stopwords.txt"));

	TEST_ASSERT(!insert_dataset_x(&hash_table, dataset_x, vocabulary));

	/* For every spec in the hash_table, check against the original JSON */
	for (int i = 0; i < hash_table.tableSize; ++i) {
		spec = hash_table.list[i];

		while (spec) {
			sprintf(json_filename, "%s/%s.json", dataset_x, spec->id);
			strcpy(tmp_template + 10, "XXXXXX");

			int n_lines;
			if (!make_tmp_json(spec, tmp_template, &n_lines)) {
				printf("Comparing %s\n", spec->id);
				compare_json(json_filename, tmp_template, n_lines);

				unlink(tmp_template);
			}

			spec = spec->next;
		}
	}

	preprocessing_destroy();
	delete_hashtable(&hash_table);
	bow_delete(vocabulary);
}

TEST_LIST = {
	{"json_insertion", test_json_insertion},
	{NULL, NULL}
};
