#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include "vocabulary.h"

/* TODO: if only stopwords, make this title more speicific */
void preprocessing_init(char *stopwords_file);
void preprocessing_destroy();

/* Adds tokens of string to the Bag of Words
 * (only words with letters, not stopwords, not capitalized) */
void bag_words(bow *vocabulary, char *str);
void spec_bag_words(bow *global, node *spec, char *str);

#endif /* PREPROCESSING_H */
