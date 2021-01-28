#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include "vocabulary.h"

int preprocessing_init(char *stopwords_file);

/* Put a whole spec in the Bag of Words (Part of creating the global vocabulary) */
int preprocessing_insert(bow *global, hashtable *fields);

/* Prepare and make all local spec structures */
int preprocessing_specs(hashtable *spec_ht, bow *global, bool trim);

void preprocessing_destroy();

#endif /* PREPROCESSING_H */
