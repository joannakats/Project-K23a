#include "spec.h"
#include "vocabulary.h"
#ifndef VALIDATION_H_
#define VALIDATION_H_

typedef struct conflict{
  node *spec1;
  node *spec2;
  bool type;  // false indicates we got [spec1, spec2, 0] and true indicates we got [spec1, spec2, 1]
  struct conflict *next;
} conflict;

typedef struct conflict_list{
  conflict *head;
} conflict_list;

conflict_list *list_init();
void delete_list(conflict_list *list);
void insert(conflict_list *list, node *spec1, node *spec2, bool type);
int isEmpty(conflict_list *list);
void print_conflict_list(conflict_list *list);

node *insert_into_structs(hashtable *hs, char *specid, hashtable *specs, bow *vocabulary);
void get_specids(char *line, char *spec1, char *spec2, int *label);
int search_for_conflicts(node *spec1, node *spec2, bool type);
void resolve_conflicts(hashtable *hs, conflict_list *list);
void validation(FILE *csv, int set_n, hashtable *specs, bow *vocabulary);

#endif
