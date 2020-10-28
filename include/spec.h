#include <stdbool.h>
#ifndef _SPEC_H
#define _SPEC_H

typedef struct field {
	char *property;
	char **values;
	int cnt; //counter of values stored for a certain property
} field;

typedef struct node {
	char *id;
	struct node *next;
	struct cliqueNode *clique; // list of clique
	bool hasListOfClique; // if true then this node "contains" the list of the clique originally
	field *fields; // array of fields
	int fieldCount; // counter of pairs <property - value> (the size of the array)
} node;

typedef struct cliqueNode {
	struct cliqueNode *next; //next in the list of this clique
	struct node *spec;
} cliqueNode;

void print_list(node *head);
void print_fields(field *array, int dim);
node *spec_init(char *id, field *fieldsArray, int numOfFields);
field *createFieldArray(int dim);
void setField(field *field, int numOfValues, char *property);
void setValue(field *f, int index, char *value);
void deleteField(field f);
cliqueNode *clique_init(node *spec);
node *spec_insert(node *head, char *id, field *fieldsArray, int numOfFields);
void delete_clique(cliqueNode *node);
void delete_specNode(node *spec);
void delete_specList(node *head);
node *search_spec(node *head, char *id, int *pos);
void clique_rearrange(node *spec1, node *spec2);
#endif
