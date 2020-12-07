#ifndef TRAINING_H
#define TRAINING_H

#include "hashtable.h"
// TODO: probably dictionary.h
// DOEST EXIST YET JUST DEFINE IT IN SOME WAY FOR NOW
typedef hashtable dictionary;

void training_init(char *stopwords_file);
void training_destroy(char *filename);

void bag_of_words(char *str, dictionary *dict);

#endif /* TRAINING_H */
