#ifndef VOCABULARY_H
#define VOCABULARY_H

#include "hashtable.h"
#include "spec.h"

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
	char **words;
	int *occurrences;                                /* global occurences */
	int *texts;   /* number of texts containing word (used in idf factor) */
	double *idf_factors;                               /* unique per word */
	hashtable ht;                                /* Points to bow_buckets */
} bow;

/* GLOBAL BoW functions */

bow *bow_init(int tableSize);
int bow_word_increment(bow *vocabulary, char *word);

/* Returns -1 if word doesn't exist, otherwise, the index of it in the arrays */
int bow_word_index(bow *vocabulary, char *word);
void bow_delete(bow *vocabulary);

/* Computes and writes global->idf_factors, unique to each word (Uses texts and spec_ht->count) */
int compute_idf(bow* global, hashtable *spec_ht);


/* LOCAL (spec) functions */

/* BoW phase */
/* Makes a zeroed "occurences" array parallel to the global bow array, for use in one spec */
int *spec_bow_occurences_init(bow *global, node *spec);

/* Increment occurence counter of a word in a spec.
 * This uses the hashtable from the global bow */
int spec_word_increment(bow *global, node *spec, char* word);


/* TF phase */
/* Generate TF vector from BoW vector (bow_occurences) (Uses global bow for array size) */
int spec_bow_to_tf(bow* global, node *spec);


/* TF-IDF phase */
/* Scan though completed local bow_occurences array, texts[i]++ for every occurence[i] > 0 */
int spec_update_texts(bow *global, node *spec);

/* Updates tf_idf_factors: Multiply global idf factors with existing local tf vector */
int spec_tf_idf(bow* global, node *spec);

/* TODO: delete this = free the arrays in spec_delete */

#endif /* VOCABULARY_H */
