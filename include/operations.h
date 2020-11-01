#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "hashtable.h"

int begin_operations(int entries, char *output, char *dataset_x, char *dataset_w);

/* Used for testing */
int insert_specs(hashtable *hash_table, char *path);
int join_specs(hashtable *hash_table, char *dataset_w);

#endif /* OPERATIONS_H */
