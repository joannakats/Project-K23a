#include <stdbool.h>

#ifndef _CLIQUE_H
#define _CLIQUE_H

typedef struct node node;

typedef struct clique {
	struct anti_clique *NegCorrel; // a list of negatively correlated cliques
	struct cliqueNode *head;
} clique;


typedef struct anti_clique {
	bool one_way_relation;	//true for only one of the 2 anti_clique nodes of Negative Correlation of specs (used for printing)
	clique *diff;
	struct anti_clique *next;
} anti_clique;


typedef struct cliqueNode {
	struct cliqueNode *next; //next in the list of this clique
	node *spec;
} cliqueNode;

void remove_duplicates(anti_clique *ac, clique *c1);
void remove_merge_duplicates(anti_clique *ac);
clique *clique_init(node *spec);
void delete_antiClique(anti_clique *head);
void delete_clique(clique *node);
void clique_rearrange(node *spec1, node *spec2);
anti_clique *anti_clique_init(clique *c, anti_clique *head, bool one_way_relation);
void anti_clique_insert(node *spec1, node *spec2);
#endif
