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
	int fieldCount; // counter of pairs <property - value>
} node;

typedef struct cliqueNode {
	struct cliqueNode *next; //next in the list of this clique
	struct node *spec;
} cliqueNode;

/*void print_list(node *);
void print_fields(fields *);*/
node *spec_init(char *, field *, int);
field *createFieldArray(int);
void setField(field *, int numOfValues, char *);
void setValue(field *, int, char *);
void deleteField(field );
cliqueNode *clique_init(node *);
node *spec_insert(node *, char *, field *, int);
void delete_clique(cliqueNode *);
void delete_specNode(node *);
void delete_specList(node *);
node *search_spec(node *, char *);
#endif
