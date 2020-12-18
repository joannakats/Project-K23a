#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _SPEC_H
#define _SPEC_H
#include "clique.h"
#include "hashtable.h"

#define FIELD_TABLE_SIZE 40

typedef struct field {
	char *property;
	char **values;
	int cnt; //counter of values stored for a certain property
	struct field *next;
} field;

typedef struct node {
	char *id;
	struct node *next;
	clique *clique;
	bool hasListOfClique; // if true then this node "contains" the list of the clique originally
	int *bow_occurences; // parallel arrays to the global bow arrays
	double *tf_idf_factors;
	int wordCount;
	hashtable *fields;	//hashtable for properties
} node;


node *spec_init(char *id, hashtable *HSfields);
hashtable *field_init(int size);
field *HSfield_insert(hashtable *hs, char *property);
void setValue(field *f, char *value);
void deleteFieldNode(field *f);
void deleteFields(hashtable *hs);
node *spec_insert(node *head, char *id, hashtable *fields);
void delete_specNode(node *spec);
void delete_specList(node *head);
node *search_spec(node *head, char *id, int *pos);
field *search_field(hashtable *fields, char *property);
#endif
