#include <ctype.h>
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "preprocessing.h"

static bow *stopwords;

int preprocessing_init(char *stopwords_file) {
	FILE *stahp;
	char word[512];

	/* Initialize stopwords vocabulary (global) */
	stopwords = bow_init(100);

	stahp = fopen(stopwords_file, "r");

	while (fgets(word, sizeof(word), stahp)) {
		*strchr(word, '\n') = '\0';        /* Remove trailing newline */
		bow_new_word(stopwords, word);
	}

	fclose(stahp);

	return 0;
}

/* Get next valid word from string.
 * Words are strings that
 * - are delimited by non-alphanumeric characters (e.g. whitespace, symbols)
 * - are returned in lowercase
 * - must contain letters ("3585043" by itself is discarded)
 * - can contain numbers ("42cm" is ok)
 * - are NOT stopwords
 *
 * Takes arguments in a manner similar to strtok_r() and fgets():
 * - Like strtok_r(), if str is NULL, parsing continues from saveptr
 * - Like fgets(), takes pointer to buffer (word) allocated outside of the
 *   function and returns pointer to it, if word wsas found, otherwise NULL */
static char *get_next_word(char *str, char *word, char **saveptr) {
	char *a, *b, *c;
	int has_letters;
	int len;

	/* If given str, this is the start of the string */
	if (str)
		a = str;
	else
		a = *saveptr;

	/* Run up to the null byte */
	for (; *a; a = b) {
		has_letters = 0;

		/* Find word boundaries [A, B] */
		for (; !isalnum(*a); ++a) {
			/* Exhausted string without finding a valid word ? */
			if (!(*a))
				break;
		}

		for (b = a; isalnum(*b); ++b) {
			if (isalpha(*b))
				has_letters = 1;
		}

		/* IGNORE: pure numbers */
		if (has_letters) {
			/* Write word in given buffer => don't modify spec content */
			len = b - a;
			memcpy(word, a, len);
			word[len] = '\0';

			/* Convert to lowercase */
			for (c = word; *c; ++c) *c = tolower(*c);

			/* IGNORE: stopwords */
			if (bow_word_index(stopwords, word) < 0) {
				*saveptr = b;  /* Save next starting position */
				return word;
			}
		}
	}

	/* We've hit the null byte before finding a word. str has been parsed */
	return NULL;
}

int preprocessing_insert(bow *global, hashtable *fields) {
	int i, v;
	char word[512], *saveptr, *ret;

	field *f;

	/* Temporary array to set existence of words in this spec.
	 * It will grow parallel to the bow arrays, for new words */
	int *exists = calloc(global->ht.count, sizeof(exists[0]));

	/* Add fields to global vocabulary (making a vector) */
	for (i = 0; i < fields->tableSize; ++i) {
		f = fields->list[i];

		while (f) {
			/* 1. Property string */
			ret = get_next_word(f->property, word, &saveptr);
			while (ret) {
				spec_has_word(global, word, &exists);
				ret = get_next_word(NULL, word, &saveptr);
			}

			/* 1. Value string(s) */
			for (v = 0; v < f->cnt; ++v) {
				ret = get_next_word(f->values[v], word, &saveptr);
				while (ret) {
					spec_has_word(global, word, &exists);
					ret = get_next_word(NULL, word, &saveptr);
				}
			}

			f = f->next;
		}
	}

	/* Increment texts where needed */
	for (i = 0; i < global->ht.count; ++i)
		global->texts[i] += exists[i];

	free(exists);

	return 0;
}

static void spec_bag_words(bow *global, node *spec) {
	int i, v;
	char word[512], *saveptr, *ret;

	hashtable *fields = spec->fields;
	field *f;

	for (i = 0; i < fields->tableSize; ++i) {
		f = fields->list[i];

		while (f) {
			/* 1. Property string */
			ret = get_next_word(f->property, word, &saveptr);
			while (ret) {
				spec_word_increment(global, spec, word);
				ret = get_next_word(NULL, word, &saveptr);
			}

			/* 1. Value string(s) */
			for (v = 0; v < f->cnt; ++v) {
				ret = get_next_word(f->values[v], word, &saveptr);
				while (ret) {
					spec_word_increment(global, spec, word);
					ret = get_next_word(NULL, word, &saveptr);
				}
			}

			f = f->next;
		}
	}
}

int preprocessing_specs(hashtable *spec_ht, bow *global, bool trim) {
	int i;
	node *spec;

	/* Finalize global vocabulary */
	bow_compute_idf(global, spec_ht);

	if (trim)
		bow_trim(global);

	for (i = 0; i < spec_ht->tableSize; ++i) {
		spec = spec_ht->list[i];

		while (spec) {
			//DEBUG: current thing we're preprocessing
			//printf("Doing %s\n", spec->id);

			/* BoW model */
			spec_bow_occurences_init(global, spec);
			spec_bag_words(global, spec);

			/* TF-IDF model */
			spec_bow_to_tf(global, spec);
			spec_tf_idf(global, spec);

			spec = spec->next;
		}
	}

	return 0;
}

void preprocessing_destroy() {
	bow_delete(stopwords);
}
