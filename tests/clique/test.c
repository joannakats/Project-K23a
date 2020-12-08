#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spec.h"
#include "acutest.h"

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

	// //create a negative correlation between spec_b & spec1
	anti_clique_insert(spec_b, spec1);

	// //merge spec2's clique with the tail's clique
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
  {"anti_clique_insert", test_anti_clique_insert},
  {"clique_rearrange", test_clique_rearrange},
  {NULL, NULL}
};
