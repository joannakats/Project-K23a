#include <ctype.h>
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "training.h"

static bow *stopwords;

void training_init(char *stopwords_file) {
	FILE *stahp;
	char word[512];

	/* Initialize stopwords vocabulary (global) */
	stopwords = bow_init(100);

	stahp = fopen(stopwords_file, "r");
	// TODO error check;

	while (fgets(word, sizeof(word), stahp)) {
		*strchr(word, '\n') = '\0';        /* Remove trailing newline */
		bow_word_increment(stopwords, word);
	}

	fclose(stahp);
}

/* Vectorize a string, only words, not numbers, not capitalized */
/* TODO: global local version of shite (MOVE to preprocessing.c) */
void bag_of_words(char *str, bow *vocabulary) {
	char *a, *b, *c;
	char word[512];
	int len;

	a = str;
	/* Run up to the null byte */
	while (*a) {
		/* Find word boundaries [A, B] */
		for (; *a && !isalpha(*a); ++a);
		for (b = a; *b && isalpha(*b); ++b);

		/* Temporary buffer for word */
		len = b - a;
		memmove(word, a, len);
		word[len] = '\0';

		/* To lowercase */
		for (c = word; *c; ++c) *c = tolower(*c);

		//puts(word);
		/* Ignore stopwords */
		if (bow_word_index(stopwords, word) < 0)
			bow_word_increment(vocabulary, word);

		a = b;
	}
}

void training_destroy() {
	bow_delete(stopwords);
}
