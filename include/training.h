#ifndef TRAINING_H
#define TRAINING_H

#include "vocabulary.h"

/* TODO: if only stopwords, make this title more speicific */
void training_init(char *stopwords_file);
void training_destroy();

/* Adds tokens of string to the Bag of Words
 * (only words with letters, not stopwords, not capitalized) */
void bag_of_words(char *str, bow *vocabulary);

#endif /* TRAINING_H */
