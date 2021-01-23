#include "validation.h"
#include "spec_hashtable.h"

/* allocate and initialize list of conflicts */
conflict_list *list_init() {
  conflict_list *list = malloc(sizeof(conflict_list));
  list->head = NULL;

  return list;
}


/* delete whole list of conflicts */
void delete_list(conflict_list *list) {
  conflict *tmp = list->head;
  conflict *next;

  /* traverse list to delete every conflict node */
  while (tmp != NULL) {
    next = tmp->next;
    free(tmp);
    tmp = next;
  }
  free(list);
}


/* returns 1 if list is empty or 0 if not */
int isEmpty(conflict_list *list) {
  return (list->head == NULL);
}


/* called when we find a relation of specs that causes conflicts */
void insert(conflict_list *list, node *spec1, node *spec2, bool type) {
  conflict *new = malloc(sizeof(conflict)); //allocate new node for list of conflicts
  /* point to these specs */
  new->spec1 = spec1;
  new->spec2 = spec2;
  new->type = type; //indicates type of relation between spec1 and spec2

  /* put new node at the top of the list */
  new->next = list->head;
  list->head = new;
}


void print_conflict_list(conflict_list *list) {
  puts("Conflict list:");
  conflict *c = list->head;

  while(c != NULL) {
    printf("%s, %s, %d\n", c->spec1->id, c->spec2->id, c->type);
    c = c->next;
  }
}


/*LABEL WILL BE REMOVED LATER*/
void get_specids(char *line, char *spec1, char *spec2, int *label) {
	char *token = strtok(line, " ,");
	strcpy(spec1, token);
	token = strtok(NULL , " ,");
	strcpy(spec2, token);
	///////////////////////////////////// DOWN
	token = strtok(NULL, " ,");
	char l[5];
	strcpy(l, token);
	*label = atoi(l);
}



node *insert_into_structs(hashtable *hs, char *specid) {
	int pos;
	/* search if this spec already exists */
	node *spec = search_hashTable_spec(hs, specid, &pos);

	if(spec == NULL) {
		spec = insert_entry_and_return(hs, specid);
	}
	/* return an existing spec or a newly inserted spec */
	return spec;
}


int search_for_conflicts(node *spec1, node *spec2, bool type) {
  if (type == true) {
     return find_positive_conflicts(spec1, spec2);
  } else {
     return find_negative_conflicts(spec1, spec2);
  }
}
