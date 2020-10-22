#include "spec.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//---->function to make properties array

//---->function to make values array

/* creates the spec node and initializes its attrubutes */
/* requirement: the arrays of strings, properties and values, have been allocated
   and initialized elsewhere */
node *spec_init(char *id, char *numOfFields, char **properties, char **values) {
  /* allocate memory of spec */
  node *spec = malloc(sizeof(node));

  spec->id = malloc(sizeof(char)*(strlen(id)+1));
  strcpy(spec->id, id);

  spec->next = NULL;
  spec->clique = NULL;

  spec->fields = malloc(sizeof(fields));
  spec->fields->properties = properties;
  spec->fields->values = values;
  spec->fields->cnt = numOfFields;

  return spec;
}


/* allocates memory for the clique node and initializes its attributes */
cliqueNode *clique_init(node *spec) {
  cliqueNode *cn = malloc(sizeof(cliqueNode));

  cn->next = NULL;
  cn->spec = spec;
  return cn;
}


/* deletes the whole clique that a spec node points to */
delete_clique(cliqueNode *node) {
  cliqueNode *temp;
  cur = node;
  while (cur != NULL) {
    temp = cur;
    cur = cur->next;
    free(temp);
  }
}


/* frees memory allocated for a spec node */
delete_specNode(node *spec) {
  fields *ptr = spec->fields;
  int numOfFields = ptr->cnt;

  /* delete the arrays of strings */
  for (int i=0; i<numOfFields; i++) {
    free(ptr->properties[i]);
    free(ptr->values[i]);
  }
  free(ptr->properties);
  free(ptr->values);

  free(spec->fields);

  delete_clique(spec->clique); //AN DEN EXEI DIKIA TOY KLIKA??

  free(spec->id);
  free(spec);
}

/* deletes the whole overflow chain */
delete_specList(node *head) {
  node *cur = head;
  node *temp;
  while(cur != NUll) {
    temp = cur;
    cur = cur->next;
    delete_specNode(temp);
  }
}
