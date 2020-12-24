#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "hashtable.h"
#include "spec_hashtable.h"
#include "json.h"
#include "spec.h"
#include "acutest.h"


void test_insert_hashtable(void){
	char *spec_id1="This is spec_id1";
	char *spec_id2="This is spec_id2";
	char *prop="This is property";
	char *val="This is value";
	int entries=10;
	int index;

 	//first spec
	hashtable *hs1 = field_init(1);
	TEST_CHECK(hs1!=NULL);
	field *spec1 = HSfield_insert(hs1, prop);	//HSfield_insert has been tested in spec/test.c
	setValue(spec1, val);	//setValue has been tested in spec/test.c

	//2nd spec
	hashtable *hs2 = field_init(2);
	TEST_CHECK(hs2!=NULL);
	field *spec2 = HSfield_insert(hs2, prop);
	TEST_CHECK(spec2 != NULL);
	spec2 = HSfield_insert(hs2, prop);
	TEST_CHECK(spec2 != NULL);

	//test hashtable init
	hashtable hsTable=hashtable_init(entries);
	TEST_CHECK(hsTable.tableSize==entries);
	//test hashtable_insert_entries
	insert_entry(&hsTable,spec_id1, &hs1);
	index=hash(spec_id1,entries);
	//test hash
	TEST_CHECK(index>=0);
	TEST_CHECK(hsTable.list[index]!=NULL); //spec_insert list has been tested in spec/test.c

	index=hash(spec_id2,entries);
	TEST_CHECK(index>=0);
	insert_entry(&hsTable,spec_id2,&hs2);
	TEST_CHECK(hsTable.list[index]!=NULL);

	delete_hashtable(&hsTable);
}

void test_join_hashtable(void){
	char spec[5];
	int entries=10,pos,i=0;
	node *temp;
	hashtable hsTable=hashtable_init(entries);

	//insert entries in hashtable.Number 2*entries is just a random number.
	hashtable *fields;
	fields = NULL;

	for(i=0;i<2*entries;i++){
		sprintf(spec,"%d",i);
		insert_entry(&hsTable,spec,&fields);
	}

	//test_hash_table_join and search_hashtable_spec
	for(i=0;i<entries;i++){
		sprintf(spec,"%d",i);
		temp=search_hashTable_spec(&hsTable,spec,&pos);
		TEST_CHECK(temp!=NULL);
		TEST_CHECK(pos>=1);// because we use hash to insert the specs.
		TEST_CHECK(strcmp(temp->id,spec) == 0);
		TEST_MSG("Unexpected search temp");
	}

	delete_hashtable(&hsTable);
}

int insert_specs(hashtable *hash_table, char *dataset_x) {
	DIR *dir;
	struct dirent *dirent;

	char path[1024], *extention;

	char *spec_id;
	hashtable *spec_fields = NULL;

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

			read_spec_from_json(path, &spec_fields);

			/* Create spec id (e.g. buy.net//10) */
			sprintf(spec_id, "%s//%s", basename(dataset_x), dirent->d_name);
			/* Remove extension to get final id */
			*strrchr(spec_id, '.') = '\0';

			/* Ready for hashtable insertion */
			insert_entry(hash_table, spec_id, &spec_fields);
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
		if (label[0] == '1') {
			hash_table_join(hash_table, left_spec, right_spec);
		} else {
			hash_table_notjoin(hash_table, left_spec, right_spec);
		}
	}

	fclose(csv);

	return 0;
}

void test_print_relations(void) {
	int ret = 0;
	FILE *fp, *fp1;
	char *dataset_x = "hashtable/2013_camera_specs";
	// char *dataset_w = "hashtable/sigmod_large_labelled_dataset.csv";
	char *dataset_w = "hashtable/sigmod_medium_labelled_dataset.csv";
	char line1[512], line2[512], left1[203], left2[203], right1[203], right2[203];
	char *token;

	hashtable hash_table;
	hash_table = hashtable_init(10007);
	if (!hash_table.list)
		return;

	if (!(ret = insert_specs(&hash_table, dataset_x))) {
		if (!(fp = fopen("output", "w+"))) {
			perror("output");
			return;
		}

		if (!(ret = join_specs(&hash_table, dataset_w))) {
			int w = print_relations(&hash_table, fp);
			TEST_CHECK(w != 0);

			fseek(fp, 0, SEEK_SET);
			/* read every line of this file */
			while (fgets(line1, sizeof(line1), fp)) {
				token = strtok(line1, ",");
				strcpy(left1, token);
				token = strtok(NULL, ",");
				strcpy(right1, token);

				fp1 = fp;

				/* read lines down from line1 in order to compare */
				while (fgets (line2, sizeof(line2), fp1)) {
					// token = NULL;
					token = strtok(line2, ",");
					strcpy(left2, token);
					token = strtok(NULL, ",");
					strcpy(right2, token);

					if (strcmp(left1, left2) == 0) {
						TEST_CHECK(strcmp(right1, right2) != 0);
						TEST_MSG("DUPLICATES FOUND");
					}

					if (strcmp(left1, right2) == 0) {
						TEST_CHECK(strcmp(left2, right1) != 0);
						TEST_MSG("DUPLICATES FOUND");
					}
				}
			}
			fclose(fp);
		}
	}
	delete_hashtable(&hash_table);
}


TEST_LIST = {
	{"hashtable_insertion",test_insert_hashtable},
	{"hashtable_search",test_join_hashtable},
	{"print_relations", test_print_relations},
	{NULL, NULL}
};
