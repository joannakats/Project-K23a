#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "hashtable.h"
#include "spec_hashtable.h"
#include "json.h"
#include "spec.h"
#include "acutest.h"

void test_anti_clique_insert() {
		node *spec1, *spec2, *spec3;
		char *id1 = "This is id1", *id2 = "This is id2", *id3 = "This is id3";

		/* initialize specs*/
		spec1 = spec_init(id1, NULL);
		spec2 = spec_init(id2, NULL);
		spec3 = spec_init(id3, NULL);

		/* create a negative correlation between spec1 & spec2 */
		anti_clique_insert(spec1, spec2);

		//check pointers
		TEST_CHECK(spec1->clique->NegCorrel != NULL);
		TEST_CHECK(spec2->clique->NegCorrel != NULL);
		TEST_CHECK(spec1->clique->NegCorrel->next == NULL);
		TEST_CHECK(spec2->clique->NegCorrel->next == NULL);

		//check this two way relation
		TEST_CHECK(spec1->clique->NegCorrel->diff == spec2->clique);
		TEST_CHECK(spec2->clique->NegCorrel->diff == spec1->clique);

		/* create a negative correlation between spec2 and spec3 */
		anti_clique_insert(spec2, spec3);

		//check pointers
		TEST_CHECK(spec3->clique->NegCorrel != NULL);
		TEST_CHECK(spec2->clique->NegCorrel != NULL);
		TEST_CHECK(spec3->clique->NegCorrel->next == NULL);
		TEST_CHECK(spec2->clique->NegCorrel->next != NULL);
		TEST_CHECK(spec2->clique->NegCorrel->next->diff == spec1->clique);

		//check this two way relation
		TEST_CHECK(spec3->clique->NegCorrel->diff == spec2->clique);
		TEST_CHECK(spec2->clique->NegCorrel->diff == spec3->clique);


		/* free memory allocated for this test */
		delete_specNode(spec1);
		delete_specNode(spec2);
		delete_specNode(spec3);
}


void test_clique_rearrange(void) {
	node *spec1, *spec2, *spec3, *spec_a, *spec_b, *spec_c, *spec_d;
	clique *hc, *s1c, *s2c;

	//make a list of three specs
	spec1 = spec_insert(NULL, "1", NULL);
	spec2 = spec_insert(spec1, "2", NULL);
	spec3 = spec_insert(spec2, "3", NULL);

	//initialize spec_a & spec_b
	spec_a = spec_init("a", NULL);
	spec_b = spec_init("b", NULL);
	spec_c = spec_init("c", NULL);
	spec_d = spec_init("d", NULL);

	//create a negative correlation between spec2 & spec_a
	anti_clique_insert(spec2, spec_a);

	// //merge spec2's clique with head's clique
	clique_rearrange(spec3, spec2);
	hc = spec3->clique;
	s2c = spec2->clique;
	//check pointers of clique list (composed of only 2 clique nodes)
	TEST_CHECK(hc->head->spec == spec3);
	TEST_CHECK(hc->head->next->spec == spec2);
	TEST_CHECK(s2c == hc);
	//check boolean
	TEST_CHECK(spec3->hasListOfClique == true);
	TEST_CHECK(spec2->hasListOfClique == false);
	TEST_CHECK(spec1->hasListOfClique == true);
	//check anti_clique merging
	TEST_CHECK(hc->NegCorrel->diff == spec_a->clique);	//check if spec3 points to spec_a
	TEST_CHECK(spec_a->clique->NegCorrel->diff == hc);	//check if spec_a points to spec3

	//create a negative correlation between spec_b & spec1
	anti_clique_insert(spec_b, spec1);

	//merge spec2's clique with the tail's clique
	clique_rearrange(spec1, spec2);
	hc = spec3->clique;
	s1c = spec1->clique;
	s2c = spec2->clique;
	//check if head, spec1 and spec2 point to the same clique list
	TEST_CHECK(hc == s1c && s1c == s2c);
	//check boolean variable for every spec
	TEST_CHECK(spec3->hasListOfClique == false);
	TEST_CHECK(spec2->hasListOfClique == false);
	TEST_CHECK(spec1->hasListOfClique == true);
	//check pointers of clique list (composed of 3 clique nodes)
	TEST_CHECK(s1c->head->spec == spec1);
	TEST_CHECK(s1c->head->next->spec == spec3);
	TEST_CHECK(s1c->head->next->next->spec == spec2);
	//check anti_clique merging
	TEST_CHECK(s1c->NegCorrel->diff == spec_b->clique);
	TEST_CHECK(spec_b->clique->NegCorrel->diff == s1c);
	TEST_CHECK(s1c->NegCorrel->next->diff == spec_a->clique);
	TEST_CHECK(spec_a->clique->NegCorrel->diff == s1c);

	/////////////////////////////////////////////////////////////////////////////////
	anti_clique_insert(spec_d, spec1);
	TEST_CHECK(spec1->clique->NegCorrel->diff == spec_d->clique);
	TEST_CHECK(spec_d->clique->NegCorrel->diff == spec1->clique);

	anti_clique_insert(spec_c, spec_d);
	TEST_CHECK(spec_d->clique->NegCorrel->diff == spec_c->clique);
	TEST_CHECK(spec_c->clique->NegCorrel->diff == spec_d->clique);


	clique_rearrange(spec1, spec_c);
	TEST_CHECK(spec1->clique == spec_c->clique);

	anti_clique *tmp = spec1->clique->NegCorrel;

	//check for duplicate anti_clique nodes
	TEST_CHECK(tmp != NULL);
	TEST_CHECK(tmp->diff == spec_d->clique);
	tmp = tmp->next;
	TEST_CHECK(tmp->diff == spec_b->clique);
	tmp = tmp->next;
	TEST_CHECK(tmp->diff == spec_a->clique);
	tmp = tmp->next;
	TEST_CHECK(tmp == NULL);

	//free memory allocated for this test
	delete_specList(spec3);
	delete_specNode(spec_a);
	delete_specNode(spec_b);
	delete_specNode(spec_c);
	delete_specNode(spec_d);
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
int relate_specs(hashtable *hash_table, char *dataset_w) {
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

		/* relate specs */
		if (label[0] == '1') { /* positively */
			hash_table_join(hash_table, left_spec, right_spec);
		} else { /* negatively */
			hash_table_notjoin(hash_table, left_spec, right_spec);
		}

		/* find specs and their cliques */
		int x = 0;
		node *right = search_hashTable_spec(hash_table, right_spec, &x);
		clique *c_right = right->clique;
		node *left = search_hashTable_spec(hash_table, left_spec, &x);
		clique *c_left = left->clique;

		anti_clique *a_right = NULL;
		anti_clique *a_left = NULL;

		/* test relation for each case */
		if (label[0] == '0') {
			a_right = c_right->NegCorrel;
			a_left = c_left->NegCorrel;

			/* traverse anti_clique list of right spec to find the anti_clique
			*  node that points to left's clique */
			while(a_right != NULL) {
				if (a_right->diff == c_left) {
					break;
				}
				a_right = a_right->next;
			}

			/* traverse anti_clique list of left spec to find the anti_clique
			*  node that points to right's clique */
			while(a_left != NULL) {
				if (a_left->diff == c_right) {
					break;
				}
				a_left = a_left->next;
			}

			/* check if the cliques of the two specs point to one another */
			TEST_CHECK(a_right != NULL && (a_right->diff == c_left));
			TEST_CHECK(a_left != NULL && (a_left->diff == c_right));
		}
		else {
			TEST_CHECK(c_right == c_left);	// check it two cliques merged

			/* get head of the merged anti_clique node */
			a_left = c_left->NegCorrel;

			/* traverse anti_clique list to check two way negative relation */
			while (a_left != NULL) {
				clique *tmp = a_left->diff;	//get the clique that this anti_clique node points to

				TEST_CHECK(tmp != NULL);
				TEST_MSG("anti_clique node of merged clique does not point to a clique");

				anti_clique *ac = tmp->NegCorrel;	//get the head of the anti_clique list of this clique

				/* traverse this list to see if an anti_clique node points to c_left */
				while (ac != NULL) {
					if (ac->diff == c_left) {
						break;
					}
					ac = ac->next;
				}

				TEST_CHECK(ac != NULL);
				TEST_MSG("not a two way negative relation");

				a_left = a_left->next;
			}

			/* traverse cliqueNode list */
			cliqueNode *c = c_left->head;
			while (c != NULL) {
				node *spec = c->spec;

				TEST_CHECK(spec->clique == c_left);
				TEST_MSG("spec does not point to this clique");

				c = c->next;
			}
		}

	}

	fclose(csv);

	return 0;
}


void test_clique_relation(void) {
	//initialize hashtable
	hashtable hs = hashtable_init(5);
	int ret = 0;
	char *dataset_x = "clique/dataset_x";
	char *dataset_w = "clique/dataset_w.csv";

	if (!(ret = insert_specs(&hs, dataset_x))) {
		if ((ret = relate_specs(&hs, dataset_w))) {
			puts("relate specs");
		}
	} else {
		puts("insert specs");
	}
}


TEST_LIST = {
  {"anti_clique_insert", test_anti_clique_insert},
  {"clique_rearrange", test_clique_rearrange},
	{"clique_relation", test_clique_relation},
  {NULL, NULL}
};
