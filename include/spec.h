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
	struct clique *clique;
	bool hasListOfClique; // if true then this node "contains" the list of the clique originally
	field *fields; // array of fields
	int fieldCount; // counter of pairs <property - value> (the size of the array)
} node;


typedef struct clique {
	struct anti_clique *NegCorrel; // a list of negatively correlated cliques
	struct cliqueNode *head;
} clique;


typedef struct anti_clique {
	clique *diff;
	struct anti_clique *next;
} anti_clique;


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
clique *clique_init(node *spec);
node *spec_insert(node *head, char *id, field *fieldsArray, int numOfFields);
void delete_antiClique(anti_clique *head);
void delete_clique(clique *node);
void delete_specNode(node *spec);
void delete_specList(node *head);
node *search_spec(node *head, char *id, int *pos);
void clique_rearrange(node *spec1, node *spec2);
anti_clique *anti_clique_init(clique *c, anti_clique *next);
void anti_clique_insert(node *spec1, node *spec2);
#endif
