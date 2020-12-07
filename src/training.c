#include <ctype.h>
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "training.h"

dictionary stopwords;

void stopwords_init(char *filename) {
	// TODO: Make dictionary (hashtable) of stopwords
}

/* Vectorize a string, only words, not numbers, not capitalized */
// TODO: probably dictionary.h
// DOEST EXIST YET
// void bag_of_words(char *str, dictionary *dict) {
// 	char *a, *b, *c;
// 	char word[512];
// 	int len;

// 	a = str;
// 	/* Run up to the null byte */
// 	while (*a) {
// 		/* Find word boundaries [A, B] */
// 		for (; *a && !isalpha(*a); ++a);
// 		for (b = a; *b && isalpha(*b); ++b);

// 		/* Temporary buffer for word */
// 		len = b - a;
// 		memmove(word, a, len);
// 		word[len] = '\0';

// 		/* To lowercase */
// 		for (c = word; *c; ++c) *c = tolower(*c);

// 		//puts(word);
// 		/* Ignore stopwords */
// 		if (!dict_word_find(&stopwords, word))
// 			dict_word_add(dict, word);

// 		a = b;
// 	}
// }
