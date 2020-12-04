#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spec.h"
#include "acutest.h"

void test_spec_insert(void) {
	char *prop="This is a property.";
	char *val="This a value.";
	char *id="This is an id.";

	//we will try to insert 3 specs

	/*  1st spec  */
	field *arr1 = createFieldArray(1);
	TEST_CHECK(arr1 != NULL);
	setField(arr1, 1, prop);
	//testing setField
	TEST_CHECK(arr1->property != NULL && strcmp(arr1->property, prop) == 0);
	TEST_CHECK(arr1->cnt == 1);
	TEST_CHECK(arr1->values != NULL);
	setValue(arr1, 0, val);
	TEST_CHECK(arr1->values[0] != NULL && strcmp(arr1->values[0], val) == 0);

	node *spec1 = spec_insert(NULL, id, arr1, 1);
	//test spec_init
	node *head = spec1;
	TEST_CHECK(spec1 != NULL);
	TEST_CHECK(spec1->id != NULL && strcmp(spec1->id, id) == 0);
	TEST_CHECK(arr1 == spec1->fields);
	TEST_CHECK(spec1->hasListOfClique == 1);
	TEST_CHECK(spec1->next == NULL);
	TEST_CHECK(spec1->fieldCount == 1);
	//see if clique_init() works as supposed to
	TEST_CHECK(spec1->clique != NULL);
	TEST_CHECK(spec1->clique->NegCorrel == NULL); //there souldn't exist a negative correlation
	// check if cliqueNode points to the newly inserted spec
	TEST_CHECK(spec1->clique->head->spec == spec1);
	TEST_CHECK(spec1->clique->head->next == NULL);

	/*  2nd spec  */
	field *arr2 = createFieldArray(2);
	TEST_CHECK(arr2 != NULL);
	setField(&arr2[0], 2, prop);
	setField(&arr2[1], 1, prop);
	//testing setField
	for (int i=0; i<2; i++) {
		TEST_CHECK(arr2[i].property != NULL && strcmp(arr2[i].property, prop) == 0);
		TEST_CHECK(arr2[i].values != NULL);
	}
	TEST_CHECK(arr2[0].cnt == 2);
	TEST_CHECK(arr2[1].cnt == 1);

	setValue(&arr2[0], 0, val);
	setValue(&arr2[0], 1, val);
	TEST_CHECK(arr2[0].values[0] != NULL && strcmp(arr2[0].values[0], val) == 0);
	TEST_CHECK(arr2[0].values[1] != NULL && strcmp(arr2[0].values[1], val) == 0);
	setValue(&arr2[1], 0, val);
	TEST_CHECK(arr2[1].values[0] != NULL && strcmp(arr2[1].values[0], val) == 0);

	node *spec2 = spec_insert(head, id, arr2, 2);
	head = spec2;
	//test spec_init
	TEST_CHECK(spec2 != NULL);
	TEST_CHECK(spec2->id != NULL && strcmp(spec2->id, id) == 0);
	TEST_CHECK(arr2 == spec2->fields);
	TEST_CHECK(spec2->hasListOfClique == 1);
	TEST_CHECK(spec2->next == spec1);
	TEST_CHECK(spec1->next == NULL);
	TEST_CHECK(spec2->fieldCount == 2);
	//see if clique_init() works as supposed to
	TEST_CHECK(spec2->clique != NULL);
	TEST_CHECK(spec2->clique->NegCorrel == NULL);
	TEST_CHECK(spec2->clique->head->spec == spec2);
	TEST_CHECK(spec2->clique->head->next == NULL);

	/*  3rd spec  */
	field *arr3 = createFieldArray(2);
	TEST_CHECK(arr3 != NULL);
	//testing setField
	for (int i=0; i<2; i++) {
		setField(&arr3[i], 2, prop);
		TEST_CHECK(arr3[i].property != NULL && strcmp(arr3[i].property, prop) == 0);
		TEST_CHECK(arr3[i].values != NULL);
		TEST_CHECK(arr3[i].cnt == 2);
		for (int j=0; j<2; j++) {
			setValue(&arr3[i], j, val);
			TEST_CHECK(arr3[i].values[j] != NULL && strcmp(arr3[i].values[j], val) == 0);
		}
	}

	node *spec3 = spec_insert(head, id, arr3, 2);
	head = spec3;
	//test spec_init
	TEST_CHECK(spec3 != NULL);
	TEST_CHECK(spec3->id != NULL && strcmp(spec3->id, id) == 0);
	TEST_CHECK(arr3 == spec3->fields);
	TEST_CHECK(spec3->hasListOfClique == 1);
	TEST_CHECK(spec3->fieldCount == 2);
	//see if clique_init() works as supposed to
	TEST_CHECK(spec3->clique != NULL);
	TEST_CHECK(spec3->clique->NegCorrel == NULL);
	TEST_CHECK(spec3->clique->head->spec == spec3);
	TEST_CHECK(spec3->clique->head->next == NULL);

	//check spec_insert
	TEST_CHECK(spec3->next == spec2);
	TEST_CHECK(spec2->next == spec1);
	TEST_CHECK(spec1->next == NULL);

	delete_specList(head);
}


void test_search_spec(void) {
	char *prop="This is a property.";
	char *val="This a value.";
	char id[3];

	//create and initialize a field array
	field *arr = createFieldArray(2);
	for (int i=0; i<2; i++) {
		setField(&arr[i], 2, prop);
		for (int j=0; j<2; j++) {
			setValue(&arr[i], j, val);
		}
	}

	//create a list of specs
	node *head = NULL;
	node *tmp;
	for (int i=1; i<10; i++) {
		sprintf(id, "%d", i);
		tmp = spec_insert(head, id, arr, 2);
		head = tmp;
	}

	//testing search
	int pos;
	int cnt = 1;
	for(int i=9; i>0; i--) {
		sprintf(id, "%d", i);
		tmp = search_spec(head, id, &pos);
		TEST_CHECK(tmp != NULL);
		TEST_CHECK(strcmp(tmp->id, id) == 0);
		TEST_CHECK(pos == cnt);
		cnt++;
	}

	//free memory allocated for this test
	//free field array
	for(int i=0; i<2; i++) {
		deleteField(arr[i]);
	}
	free(arr);
	node *cur = head;
	while(cur != NULL) {
		tmp = cur;
		cur = cur->next;
		if (tmp->hasListOfClique) { //make sure we don't free something we have already freed
			delete_clique(tmp->clique);
		}

		free(tmp->id);
		free(tmp);
	}
}


void test_anti_clique_insert() {
		node *spec1, *spec2, *spec3;
		char *id1 = "This is id1", *id2 = "This is id2", *id3 = "This is id3";

		/* initialize specs*/
		spec1 = spec_init(id1, NULL, 0);
		spec2 = spec_init(id2, NULL, 0);
		spec3 = spec_init(id3, NULL, 0);

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
	node *spec1, *spec2, *spec3, *spec_a, *spec_b;
	clique *hc, *s1c, *s2c;

	//make a list of three specs
	spec1 = spec_insert(NULL, "1", NULL, 0);
	spec2 = spec_insert(spec1, "2", NULL, 0);
	spec3 = spec_insert(spec2, "3", NULL, 0);

	//initialize spec_a & spec_b
	spec_a = spec_init("a", NULL, 0);
	spec_b = spec_init("b", NULL, 0);

	//create a negative correlation between spec2 & spec_a
	anti_clique_insert(spec2, spec_a);

	//merge spec2's clique with head's clique
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

	//free memory allocated for this test
	delete_specList(spec3);
	delete_specNode(spec_a);
	delete_specNode(spec_b);
}


TEST_LIST = {
	{"spec_insertion", test_spec_insert},
	{"spec_search", test_search_spec},
	{"anti_clique_insert", test_anti_clique_insert}, /*
	{"clique_rearrange", test_clique_rearrange},*/
	{NULL, NULL}
};
