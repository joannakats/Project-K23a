#ifndef _SPEC_H
#define _SPEC_H

typedef struct fields {
  //properties and values are stored accordingly
  char **properties;
  char **values;
	int cnt; //counter of fields stored
} fields;

typedef struct node {
  char *id;
  struct node *next;
  struct cliqueNode *clique;
  fields *fields;
} node;

typedef struct cliqueNode {
  struct cliqueNode *next; //next in the list of this clique
  struct node *spec;
} cliqueNode;


node *spec_init(char *, char *, char **, char **);
cliqueNode *clique_init(node *);
delete_clique(cliqueNode *);
delete_specNode(node *);
delete_specList(node *);

#endif
