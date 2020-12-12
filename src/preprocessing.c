#include <ctype.h>
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "preprocessing.h"

static bow *stopwords;

void preprocessing_init(char *stopwords_file) {
	FILE *stahp;
	char word[512];

	/* Initialize stopwords vocabulary (global) */
	stopwords = bow_init(100);

	stahp = fopen(stopwords_file, "r");

	while (fgets(word, sizeof(word), stahp)) {
		*strchr(word, '\n') = '\0';        /* Remove trailing newline */
		bow_word_increment(stopwords, word);
	}

	fclose(stahp);
}

/* Vectorize a string, only words, not numbers, not capitalized */

/* Populate the global vocabulary */
void bag_words(bow *vocabulary, char *str) {
	char *a, *b, *c;
	char word[512];
	int len;

	a = str;
	/* Run up to the null byte */
	while (*a) {
		/* Find word boundaries [A, B] */
		for (; *a && !isalnum(*a); ++a);
		for (b = a; *b && isalnum(*b); ++b);

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

/* Local version: Populate the local occurences */
void spec_bag_words(bow *global, node *spec, char *str) {
	char *a, *b, *c;
	char word[512];
	int len;

	a = str;
	/* Run up to the null byte */
	while (*a) {
		/* Find word boundaries [A, B] */
		for (; *a && !isalnum(*a); ++a);
		for (b = a; *b && isalnum(*b); ++b);

		/* Temporary buffer for word */
		len = b - a;
		memmove(word, a, len);
		word[len] = '\0';

		/* To lowercase */
		for (c = word; *c; ++c) *c = tolower(*c);

		//puts(word);
		/* Ignore stopwords */
		/* TODO: enable this when defined
		if (bow_word_index(stopwords, word) < 0)
			spec_word_increment(global, spec, word); */

		a = b;
	}
}

void preprocessing_destroy() {
	bow_delete(stopwords);
}
