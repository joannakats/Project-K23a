#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hashtable.h"
#include "operations.h"

int compare_json(node *spec, char *json) {
	return 0;
}

int main() {
	hashtable hash_table = hashtable_init(5);

	insert_specs(&hash_table, "insertion/dataset_x");

	/* Go through .json's in dataset_x, compare with specs in memory */

	delete_hashtable(&hash_table);
}
