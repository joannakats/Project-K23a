#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vocabulary.h"

bow *bow_init(int tableSize) {
	bow *new = malloc(sizeof(*new));

	if (!new) {
		perror("bow_init");
		return NULL;
	}

	new->size = 0;
	new->words = NULL;
	new->occurrences = NULL;
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
	vocabulary->size++;
	vocabulary->words = realloc(vocabulary->words, vocabulary->size * sizeof(vocabulary->words[0]));
	vocabulary->occurrences = realloc(vocabulary->occurrences, vocabulary->size * sizeof(vocabulary->occurrences[0]));

	// TODO: error check

	/* 2: Append word to array */
	vocabulary->words[vocabulary->size - 1] = strdup(word);
	vocabulary->occurrences[vocabulary->size - 1] = 1;

	/* 3: Add array_index to hashtable */
	ht_index = hash(word, vocabulary->ht.tableSize);
	bucket = malloc(sizeof(*bucket));

	/* New bucket as head of list, points to place in indexed array */
	bucket->index = vocabulary->size - 1;
	bucket->next = vocabulary->ht.list[ht_index];
	vocabulary->ht.list[ht_index] = bucket;

	return 0;
}

void bow_delete(bow *vocabulary) {
	int i;
	bow_bucket *current, *next;

	for (i = 0; i < vocabulary->size; ++i)
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
