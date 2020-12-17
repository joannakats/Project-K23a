#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spec.h"
#include "acutest.h"


void test_field_init(void) {
	/* allocate a fields hashtable for the test */
	hashtable *hs = field_init(2);
	TEST_CHECK(hs != NULL);
	TEST_CHECK(hs->list != NULL);

	char *prop1 = "page title";
	char *prop2 = "card slot";
	char *prop3 = "depth inches";

	/* test initialization of fields */
	field *f = HSfield_insert(hs, prop1);
	TEST_CHECK(f != NULL);
	setValue(f, "value for property 1");
	TEST_CHECK(f->cnt == 1);
	TEST_CHECK(f->values != NULL);
	TEST_CHECK(f->values[0] != NULL);
	TEST_CHECK(strcmp(f->values[0], "value for property 1") == 0);
	setValue(f, "value for property 1 again");
	TEST_CHECK(f->cnt == 2);
	TEST_CHECK(f->values != NULL);
	TEST_CHECK(f->values[1] != NULL);
	TEST_CHECK(strcmp(f->values[1], "value for property 1 again") == 0);

	field *f1 = HSfield_insert(hs, prop2);
	setValue(f1, "value for property 2");
	TEST_CHECK(f1->cnt == 1);
	TEST_CHECK(f1->values != NULL);
	TEST_CHECK(f1->values[0] != NULL);
	TEST_CHECK(strcmp(f1->values[0], "value for property 2") == 0);

	field *f2 = HSfield_insert(hs, prop3);
	TEST_CHECK(f2 != NULL);
	setValue(f2, "value for property 3");
	TEST_CHECK(f2->cnt == 1);
	TEST_CHECK(f2->values != NULL);
	TEST_CHECK(f2->values[0] != NULL);
	TEST_CHECK(strcmp(f2->values[0], "value for property 3") == 0);
	setValue(f2, "value for property 3 again");
	TEST_CHECK(f2->cnt == 2);
	TEST_CHECK(f2->values != NULL);
	TEST_CHECK(f2->values[1] != NULL);
	TEST_CHECK(strcmp(f2->values[1], "value for property 3 again") == 0);

	/* search property in the hashtable */
	field *res = search_field(hs, prop2);
	TEST_CHECK(res != NULL);
	TEST_CHECK(strcmp(res->property, prop2)== 0);

	res = search_field(hs, "jdfnsdkfs");
	TEST_CHECK(res == NULL);

	res = search_field(hs, prop1);
	TEST_CHECK(res != NULL);
	TEST_CHECK(strcmp(res->property, prop1)== 0);

	res = search_field(hs, prop3);
	TEST_CHECK(res != NULL);
	TEST_CHECK(strcmp(res->property, prop3)== 0);

	/* free memory allocated for this test */
	deleteFields(hs);
}

void test_spec_insert(void) {
	char *prop="This is a property.";
	char *val="This a value.";
	char *id="This is an id.";

	hashtable *hs = field_init(2);
	//we will try to insert 3 specs

	/*  1st spec  */
	node *spec1 = spec_insert(NULL, id, hs);
	//test spec_init
	node *head = spec1;
	TEST_CHECK(spec1 != NULL);
	TEST_CHECK(spec1->id != NULL && strcmp(spec1->id, id) == 0);
	TEST_CHECK(hs == spec1->fields);
	TEST_CHECK(spec1->hasListOfClique == 1);
	TEST_CHECK(spec1->next == NULL);
	TEST_CHECK(spec1->fields->count == 0);
	//see if clique_init() works as supposed to
	TEST_CHECK(spec1->clique != NULL);
	TEST_CHECK(spec1->clique->NegCorrel == NULL); //there souldn't exist a negative correlation
	// check if cliqueNode points to the newly inserted spec
	TEST_CHECK(spec1->clique->head->spec == spec1);
	TEST_CHECK(spec1->clique->head->next == NULL);

	/*  2nd spec  */
	node *spec2 = spec_insert(head, id, NULL);
	head = spec2;
	//test spec_init
	TEST_CHECK(spec2 != NULL);
	TEST_CHECK(spec2->id != NULL && strcmp(spec2->id, id) == 0);
	TEST_CHECK(spec2->hasListOfClique == 1);
	TEST_CHECK(spec2->next == spec1);
	TEST_CHECK(spec1->next == NULL);
	TEST_CHECK(spec2->fields == NULL);
	//see if clique_init() works as supposed to
	TEST_CHECK(spec2->clique != NULL);
	TEST_CHECK(spec2->clique->NegCorrel == NULL);
	TEST_CHECK(spec2->clique->head->spec == spec2);
	TEST_CHECK(spec2->clique->head->next == NULL);

	/*  3rd spec  */
	hashtable *hS = field_init(2);
	field *F = HSfield_insert(hS, prop);
	setValue(F, val);
	setValue(F,val);
	node *spec3 = spec_insert(head, id, hS);
	head = spec3;
	//test spec_init
	TEST_CHECK(spec3 != NULL);
	TEST_CHECK(spec3->id != NULL && strcmp(spec3->id, id) == 0);
	TEST_CHECK(hS == spec3->fields);
	TEST_CHECK(spec3->hasListOfClique == 1);
	// get field node
	F = search_field(hS, prop);
	TEST_CHECK(F != NULL);
	TEST_CHECK(F->cnt == 2);
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
	char id[3];

	//create a list of specs
	node *head = NULL;
	node *tmp;
	for (int i=1; i<10; i++) {
		sprintf(id, "%d", i);
		tmp = spec_insert(head, id, NULL);
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


TEST_LIST = {
	{"field_insertion", test_field_init},
	{"spec_insertion", test_spec_insert},
	{"spec_search", test_search_spec},
	{NULL, NULL}
};
