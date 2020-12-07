#ifndef TRAINING_H
#define TRAINING_H

// TODO: probably dictionary.h
#include "hashtable.h"

void training_init(char *stopwords_file);
void training_destroy(char *filename);

void bag_of_words(char *str, dictionary *dict);

#endif /* TRAINING_H */
