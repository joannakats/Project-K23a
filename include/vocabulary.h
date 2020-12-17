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
 * - the distinct words[0...count] in all the specs
 * - the amount of texts[0...count] that contain each word
 * - the idf factor of each word
 * and a hashtable that can be used to find the index of a word in said arrays,
 * in an associative manner.
 *
 * Locally, each spec has 2 arrays parallel to the aformentioned ones:
 * - BoW model: the occurences of the words in this specific spec
 * - TF-IDF model: the tf-idf factors, originating from the BoW model */
typedef struct bow {
	char **words;
	int *texts;                        /* number of texts containing word */
	double *idf_factors;        /* generated from texts - unique per word */
	hashtable ht;                                /* Points to bow_buckets */
} bow;

/* GLOBAL BoW functions */

bow *bow_init(int tableSize);

/* Returns -1 if word doesn't exist, otherwise, the index of it in the arrays */
int bow_word_index(bow *vocabulary, char *word);

/* Called for new words that definitely do not have an existing entry */
int bow_new_word(bow *vocabulary, char *word);

/* Record existence of word in a spec. Useful for idf later */
int spec_has_word(bow *vocabulary, char *word, int **exists);

/* Computes and writes global->idf_factors, unique to each word (Uses texts and spec_ht->count) */
int bow_compute_idf(bow* global, hashtable *spec_ht);

/* Discard words that don't meet a certain "popularity" threshold.
 * Trimming works as follows:
 * Make new parallel arrays that only incorporate the word entries that pass
 * though the keep_word() filter. The objective is to achieve a smaller memory
 * footprint for the vocabulary */
int bow_trim(bow *vocabulary);

void bow_delete(bow *vocabulary);


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
/* Updates tf_idf_factors: Multiply global idf factors with existing local tf vector */
int spec_tf_idf(bow* global, node *spec);

#endif /* VOCABULARY_H */
