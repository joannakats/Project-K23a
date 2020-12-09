/* TODO: Maybe BoW and TF in separate header ? */

#ifndef VOCABULARY_H
#define VOCABULARY_H

#include "hashtable.h"

typedef struct bow_bucket {
	struct bow_bucket *next;
	int index; /* Quick access to the index of a word in the (parallel) arrays */
} bow_bucket;

/* The "Bag of Words" has every distict word, and every occurence of this
 * word on all the specs. Conceptually, it's on the same level as the spec hashtable:
 * Project-wide.
 * It has parallel indexed arrays of
 * - the distinct words in all the specs
 * - the occurence of these words globally
 * and a hashtable that can be used to find the index of a word in said arrays,
 * in an associative manner.
 *
 * Locally, each spec has 2 arrays parallel to the aformentioned ones:
 * - BoW model: the occurences of the words in this specific spec
 * - TF-IDF model: the tf-idf factors, originating from the BoW model */
typedef struct bow {
	int count;
	char **words;
	int *occurrences;
	hashtable ht;                                /* Points to bow_buckets */
} bow;

bow *bow_init(int tableSize);
int bow_word_increment(bow *vocabulary, char *word);

/* Returns -1 if word doesn't exist, otherwise, the index of it in the arrays */
int bow_word_index(bow *vocabulary, char *word);
void bow_delete(bow *vocabulary);


/* Local-spec functions */

/* Makes a zeroed "occurences" array parallel to the global bow array, for use in one spec */
int *spec_bow_occurences_init(bow *global);
/* TODO: Piazza hmm? int *spec_tf_factors_init(bow *global); */

/* Increment occurence counter of a word in a spec.
 * This uses the hashtable from the global bow */
int spec_word_increment(bow *global, int *occurences, char* word);

/* TODO: TF-IDF */

#endif /* VOCABULARY_H */
