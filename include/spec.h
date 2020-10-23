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

void print_list(node *);
void print_fields(fields *);
node *spec_init(char *, int, char **, char **);
cliqueNode *clique_init(node *);
node *spec_insert(node *, char *, int, char **, char **);
void delete_clique(cliqueNode *);
void delete_specNode(node *);
void delete_specList(node *);

#endif
