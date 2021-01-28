#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "vocabulary.h"

static double trim_threshold;

bow *bow_init(int tableSize) {
	bow *new = malloc(sizeof(*new));

	if (!new) {
		perror("bow_init");
		return NULL;
	}

	new->words = NULL;
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

int bow_new_word(bow *vocabulary, char *word) {
	int ht_index;
	bow_bucket *bucket;

	/* 1: Grow */
	vocabulary->ht.count++;
	vocabulary->words = realloc(vocabulary->words, vocabulary->ht.count * sizeof(vocabulary->words[0]));
	vocabulary->texts = realloc(vocabulary->texts, vocabulary->ht.count * sizeof(vocabulary->texts[0]));

	/* 2: Append word to array */
	vocabulary->words[vocabulary->ht.count - 1] = strdup(word);
	vocabulary->texts[vocabulary->ht.count - 1] = 0;

	/* 3: Add array_index to hashtable */
	ht_index = hash(word, vocabulary->ht.tableSize);
	bucket = malloc(sizeof(*bucket));

	/* New bucket as head of list, points to place in indexed array */
	bucket->index = vocabulary->ht.count - 1;
	bucket->next = vocabulary->ht.list[ht_index];
	vocabulary->ht.list[ht_index] = bucket;

	return 0;
}

/* Record existence of word in spec (1) */
int spec_has_word(bow *vocabulary, char *word, int **exists) {
	int index = bow_word_index(vocabulary, word);

	if (index >= 0) {
		(*exists)[index] = 1;
	} else {
		bow_new_word(vocabulary, word);

		*exists = realloc(*exists, vocabulary->ht.count * sizeof((*exists)[0]));
		(*exists)[vocabulary->ht.count - 1] = 1;
	}

	return 0;
}

int bow_compute_idf(bow* global, hashtable *spec_ht) {
	global->idf_factors=malloc(global->ht.count *sizeof(double));
	int size=global->ht.count;
	//for every word in global compute and write idf_factor
	for(int i=0;i<size;i++){
		global->idf_factors[i]=log10(((double)spec_ht->count)/global->texts[i]);
	}

	/* Ceiling for trimming words: being present in at least 75% of specs */
	trim_threshold = log10(1.0 / 0.70);

	return 0;
}

/* Judge whether this word is necessary */
static int keep_word(bow *vocabulary, int index) {
	return (vocabulary->idf_factors[index] > trim_threshold && vocabulary->texts[index] > 20);
}

/* Change the array index a bow->ht entry points to */
static int bow_ht_update(bow *vocabulary, int old_index, int new_index) {
	int ht_index;
	bow_bucket *bucket;

	ht_index = hash(vocabulary->words[old_index], vocabulary->ht.tableSize);

	bucket = vocabulary->ht.list[ht_index];
	while (bucket) {
		if (bucket->index == old_index) {
			bucket->index = new_index;
			return 0;
		}

		bucket = bucket->next;
	}

	return -1;
}

/* Delete an entry from the bow->ht */
static int bow_ht_delete(bow *vocabulary, int array_index) {
	int ht_index;
	bow_bucket *prev, *current;

	/* Remove from hashtable */
	ht_index = hash(vocabulary->words[array_index], vocabulary->ht.tableSize);

	prev = NULL;
	current = vocabulary->ht.list[ht_index];
	while (current) {
		if (current->index == array_index) {
			/* If head of list, update ht->list */
			if (prev)
				prev->next = current->next;
			else
				vocabulary->ht.list[ht_index] = current->next;

			free(current);
			return 0;
		}

		prev = current;
		current = current->next;
	}

	return -1;
}

int bow_trim(bow *vocabulary) {
	int i;

	/* Move desired elements from old to vocabulary, then free old */
	bow old = *vocabulary;

	vocabulary->ht.count = 0;
	vocabulary->words = NULL;
	vocabulary->texts = NULL;
	vocabulary->idf_factors = NULL;

	for (i = 0; i < old.ht.count; ++i) {
		if (keep_word(&old, i)) {
			/* Put word in new arrays */
			vocabulary->ht.count++;
			vocabulary->words = realloc(vocabulary->words, vocabulary->ht.count * sizeof(vocabulary->words[0]));
			vocabulary->texts = realloc(vocabulary->texts, vocabulary->ht.count * sizeof(vocabulary->texts[0]));
			vocabulary->idf_factors = realloc(vocabulary->idf_factors, vocabulary->ht.count * sizeof(vocabulary->idf_factors[0]));

			vocabulary->words[vocabulary->ht.count - 1] = old.words[i];
			vocabulary->texts[vocabulary->ht.count - 1] = old.texts[i];
			vocabulary->idf_factors[vocabulary->ht.count - 1] = old.idf_factors[i];

			/* Update old hashtable entry
			 * (ht will be copied to vocabulary later) */
			bow_ht_update(&old, i, vocabulary->ht.count - 1);
		} else {
			/* Remove old hashtable entry */
			bow_ht_delete(&old, i);
			free(old.words[i]);
		}
	}

	/* ht list already updated in old */
	vocabulary->ht.list = old.ht.list;

	free(old.words);
	free(old.texts);
	free(old.idf_factors);

	return 0;
}

void bow_delete(bow *vocabulary) {
	int i;
	bow_bucket *current, *next;

	for (i = 0; i < vocabulary->ht.count; ++i)
		free(vocabulary->words[i]);

	free(vocabulary->words);
	free(vocabulary->texts);
	free(vocabulary->idf_factors);

	/* Delete hashtable */
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

int *spec_bow_occurences_init(bow *global, node *spec){
	int size=global->ht.count;
	spec->bow_occurences=malloc(size *sizeof(int));
	spec->wordCount = 0;
	for(int i=0;i<size;i++){
		spec->bow_occurences[i]=0;
	}
	return 0;
}

int spec_word_increment(bow *global, node *spec, char* word){//not sure about that
	//find the index of the word in global->words
	int array_index=bow_word_index(global,word);
	//found
	if(array_index>=0){
		spec->bow_occurences[array_index]++;
		spec->wordCount++;
	}
	return 0;
}

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

int spec_tf_idf(bow *global, node *spec) {
	int i;

	/* Multiply with idf factors */
	for (i = 0; i < global->ht.count; ++i)
		spec->tf_idf_factors[i] *= global->idf_factors[i];

	return 0;
}
