#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "acutest.h"
#include "spec_hashtable.h"
#include "preprocessing.h"
#include "vocabulary.h"

void test_vocabulary() {
	bow *vocabulary = NULL;
	hashtable ht, *fields1 = NULL, *fields2 = NULL;
	field *f;

	int pos;
	node *spec1, *spec2;

	ht = hashtable_init(3);
	vocabulary = bow_init(13);

	TEST_ASSERT(ht.list != NULL);
	TEST_ASSERT(vocabulary != NULL);


	/* Make spec 1 fields */
	fields1 = field_init(3);
	TEST_ASSERT(fields1 != NULL);

	f = HSfield_insert(fields1, "description");
	setValue(f, "The FitnessGram, PACER TEST! Run for 300m or something of that sort. 3 times.");

	/* Make spec 2 fields */
	fields2 = field_init(3);
	TEST_ASSERT(fields2 != NULL);

	f = HSfield_insert(fields2, "summary");
	setValue(f, "Hello darkness, my old friend, I've come to talk with you again, Because the fitnessGRAM pacer TEST softly creeping...");


	preprocessing_init("../stopwords.txt");


	/* Insert spec 1 */
	insert_entry(&ht, "1", &fields1);
	preprocessing_insert(vocabulary, fields1);

	/* Insert spec 2 */
	insert_entry(&ht, "2", &fields2);
	preprocessing_insert(vocabulary, fields2);

	TEST_CHECK_(bow_word_index(vocabulary, "for") < 0, "Testing if stopword %s is in the vocabulary", "for");


	/* Make local spec preprocessing models */
	preprocessing_specs(&ht, vocabulary, false);

	/* Print */
	spec1 = search_hashTable_spec(&ht, "1", &pos);
	TEST_ASSERT(spec1 != NULL);

	spec2 = search_hashTable_spec(&ht, "2", &pos);
	TEST_ASSERT(spec2 != NULL);

	printf("\nBoW\n%-12s %-10s %-10s\n--------------------------------\n", "WORD", "SPEC1", "SPEC2");
	for (int i = 0; i < vocabulary->ht.count; ++i) {
		printf("%-12s %-10d %-10d\n", vocabulary->words[i], spec1->bow_occurences[i], spec2->bow_occurences[i]);
	}

	printf("\nTF-IDF\n%-12s %-10s %-10s\n--------------------------------\n", "WORD", "SPEC1", "SPEC2");
	for (int i = 0; i < vocabulary->ht.count; ++i) {
		printf("%-12s %-10f %-10f\n", vocabulary->words[i], spec1->tf_idf_factors[i], spec2->tf_idf_factors[i]);
	}

	/* Cleanup */
	preprocessing_destroy();
	bow_delete(vocabulary);
	delete_hashtable(&ht);
}

TEST_LIST = {
	{"vocabulary", test_vocabulary},
	{NULL, NULL}
};
