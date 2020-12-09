#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _SPEC_H
#define _SPEC_H
#include "clique.h"

typedef struct field {
	char *property;
	char **values;
	int cnt; //counter of values stored for a certain property
} field;

typedef struct node {
	char *id;
	struct node *next;
	clique *clique;
	bool hasListOfClique; // if true then this node "contains" the list of the clique originally
	field *fields; // array of fields
	int fieldCount; // counter of pairs <property - value> (the size of the array)
	int bow_occurences; // parallel arrays to the global bow arrays
	double tf_factors;
} node;


void print_list(node *head);
void print_fields(field *array, int dim);
node *spec_init(char *id, field *fieldsArray, int numOfFields);
field *createFieldArray(int dim);
void setField(field *field, int numOfValues, char *property);
void setValue(field *f, int index, char *value);
void deleteField(field f);
node *spec_insert(node *head, char *id, field *fieldsArray, int numOfFields);
void delete_specNode(node *spec);
void delete_specList(node *head);
node *search_spec(node *head, char *id, int *pos);

#endif
