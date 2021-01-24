#include "spec.h"
#include "vocabulary.h"
#include "loregression.h"
#ifndef VALIDATION_H_
#define VALIDATION_H_

typedef struct conflict{
  node *spec1;
  node *spec2;
  bool type;  // false indicates we got [spec1, spec2, 0] and true indicates we got [spec1, spec2, 1]
  double prediction;
  struct conflict *next;
} conflict;

typedef struct conflict_list{
  conflict *head;
  int counter;  //counter of nodes
} conflict_list;

conflict_list *list_init();
void delete_list(conflict_list *list);
void insert(conflict_list *list, node *spec1, node *spec2, double prediction, bool type);
int isEmpty(conflict_list *list);
void print_conflict_list(conflict_list *list);

node *insert_into_structs(hashtable *hs, char *specid, hashtable *specs, bow *vocabulary);
void get_specids(char *line, char *spec1, char *spec2, int *label);
int search_for_conflicts(node *spec1, node *spec2, bool type);
void resolve_conflicts(logistic_regression *model, conflict_list *list);
void resolve_positive_conflict(logistic_regression *model, conflict *conflict);
void resolve_negative_conflict(logistic_regression *model, conflict *conflict);
void validation(FILE *csv, int set_n, hashtable *specs, bow *vocabulary);

#endif
