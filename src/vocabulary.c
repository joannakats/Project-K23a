#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "vocabulary.h"

bow *bow_init(int tableSize) {
	bow *new = malloc(sizeof(*new));

	if (!new) {
		perror("bow_init");
		return NULL;
	}

	new->words = NULL;
	new->occurrences = NULL;
	new->texts = NULL;
	new->idf_factors = NULL;
	new->ht = hashtable_init(tableSize);

	return new;
}

int bow_word_index(bow *vocabulary, char *word) {
	int array_index;
	int ht_index = hash(word, vocabulary->ht.tableSize);
	bow_bucket *bucket = vocabulary->ht.list[ht_index];

	while (bucket) {
		array_index = bucket->index;

		/* If word exists already in the array, increment the occurrence counter */
		if (!strcmp(vocabulary->words[array_index], word))
			return array_index;

		bucket = bucket->next;
	}

	return -1;
}

int bow_word_increment(bow *vocabulary, char *word) {
	int array_index, ht_index;
	bow_bucket *bucket;

	/* Search for word associatively */
	array_index = bow_word_index(vocabulary, word);

	/* Increment existing word counter */
	if (array_index >= 0) {
		vocabulary->occurrences[array_index]++;
		return 0;
	}

	/* At this point, the word isn't present our vocabulary yet.
	 * 1: Grow */
	vocabulary->ht.count++;
	vocabulary->words = realloc(vocabulary->words, vocabulary->ht.count * sizeof(vocabulary->words[0]));
	vocabulary->texts = realloc(vocabulary->texts, vocabulary->ht.count * sizeof(vocabulary->texts[0]));
	vocabulary->occurrences = realloc(vocabulary->occurrences, vocabulary->ht.count * sizeof(vocabulary->occurrences[0]));
	vocabulary->idf_factors = realloc(vocabulary->idf_factors, vocabulary->ht.count * sizeof(vocabulary->idf_factors[0]));

	/* 2: Append word to array */
	vocabulary->words[vocabulary->ht.count - 1] = strdup(word);
	vocabulary->occurrences[vocabulary->ht.count - 1] = 1;

	/* These are updated later */
	vocabulary->texts[vocabulary->ht.count - 1] = 0;
	vocabulary->idf_factors[vocabulary->ht.count - 1] = 0.0;

	/* 3: Add array_index to hashtable */
	ht_index = hash(word, vocabulary->ht.tableSize);
	bucket = malloc(sizeof(*bucket));

	/* New bucket as head of list, points to place in indexed array */
	bucket->index = vocabulary->ht.count - 1;
	bucket->next = vocabulary->ht.list[ht_index];
	vocabulary->ht.list[ht_index] = bucket;

	return 0;
}

void bow_delete(bow *vocabulary) {
	int i;
	bow_bucket *current, *next;

	for (i = 0; i < vocabulary->ht.count; ++i)
		free(vocabulary->words[i]);

	free(vocabulary->words);
	free(vocabulary->occurrences);

	/* TODO: Delete hashtable function? */
	for (i = 0; i < vocabulary->ht.tableSize; ++i) {
		current = vocabulary->ht.list[i];

		while (current) {
			next = current->next;
			free(current);
			current = next;
		}
	}
	free(vocabulary->ht.list);

	free(vocabulary);
}

/* LOCAL (spec) functions */

/* TF phase */
int spec_bow_to_tf(bow* global, node *spec) {
	int i;

	spec->tf_idf_factors = malloc(global->ht.count * sizeof(spec->tf_idf_factors[0]));
	if (!spec->tf_idf_factors) {
		perror("tf_idf_factors");
		return errno;
	};

	for (i = 0; i < global->ht.count; ++i)
		spec->tf_idf_factors[i] = spec->bow_occurences[i] / (double) spec->wordCount;

	return 0;
}

/* TF-IDF phase */

int spec_update_texts(bow *global, node *spec) {
	int i;

	for (i = 0; i < global->ht.count; ++i) {
		if (spec->bow_occurences[i] > 0)
			global->texts[i]++;
	}

	return 0;
}

int spec_tf_idf(bow *global, node *spec) {
	int i;

	for (i = 0; i < global->ht.count; ++i)
		spec->tf_idf_factors[i] *= global->idf_factors[i];

	return 0;
}
