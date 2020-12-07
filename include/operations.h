#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <stdio.h>
#include "hashtable.h"

int begin_operations(int entries, char *dataset_x, char *dataset_w, char *output);

/* Used for testing */
int insert_dataset_x(hashtable *hash_table, char *dataset_x);

/* Using FILE* argument, because it's only part of the parsing of dataset W.
 * Normally used for 60% of the file, but can be used for all of it, if passed
 * the FILE* pointer */
int relate_specs(hashtable *hash_table, FILE *csv, long training_n);

#endif /* OPERATIONS_H */
