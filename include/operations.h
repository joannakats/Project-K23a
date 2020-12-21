#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "spec_hashtable.h"
#include "vocabulary.h"

int begin_operations(int entries, char *dataset_x, char *dataset_w, char *output);

/* Used for testing */
/* Make spec nodes */
int insert_dataset_x(hashtable *hash_table, char *dataset_x, bow *vocabulary);

/* Make cliques */
int insert_dataset_w(hashtable *hash_table, char *dataset_w);

#endif /* OPERATIONS_H */
