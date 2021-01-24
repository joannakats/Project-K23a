#include "spec.h"


/* allocates memory for the clique node and initializes its attributes */
clique *clique_init(node *spec) {
	clique *c = malloc(sizeof(clique));
	c->NegCorrel = NULL;
	c->head = malloc(sizeof(cliqueNode));

	cliqueNode *cn = c->head;
	cn->next = NULL;
	cn->spec = spec;
	return c;
}


/* this function is called when spec1 and spec2 are definitely not alike */
void anti_clique_insert(node *spec1, node *spec2) {
	anti_clique *ac1 = spec1->clique->NegCorrel;

	/* search if a negative correlation between spec1 and spec2 already exists */
	while (ac1 != NULL) {
		/* if there is already a negative correlation between spec1's and spec2's cliques
		   there is nothing to be done */
		/* no need to check for both anti_clique lists, negative correlation is a two way relation */
		if (ac1->diff == spec2->clique)
			return;

		ac1 = ac1->next;
	}

	/* every new anti_clique node is put at the top of the anti_clique list */
	spec1->clique->NegCorrel = anti_clique_init(spec2->clique, spec1->clique->NegCorrel);
	spec2->clique->NegCorrel = anti_clique_init(spec1->clique, spec2->clique->NegCorrel);
}


/* allocates memory for anti_clique structure and initializes */
anti_clique *anti_clique_init(clique *c, anti_clique *head) {
	anti_clique *ac = malloc(sizeof(anti_clique));
	ac->next = head;
	ac->diff = c;
	ac->one_way_relation = false;
	return ac;
}



void remove_duplicates(anti_clique *ac, clique *c1) {
	anti_clique *temp = ac;
	anti_clique *prev;
	int cnt;

	/* traverse list of anti_clique nodes */
	while (temp != NULL) {
		clique *another = temp->diff;
		anti_clique *ac_node = another->NegCorrel;
		cnt = 0;
		prev = NULL;

		/* traverse list of another's clique anti_clique nodes */
		while(ac_node != NULL) {
			if(ac_node->diff == c1) {
				cnt++;
				if(cnt > 1) {
					/* remove node */
					prev->next = ac_node->next;
					ac_node->next = NULL;
					ac_node->diff = NULL;
					free(ac_node);
					ac_node = prev;
					cnt--;
				}
			}

			prev = ac_node;
			ac_node = ac_node->next;
		}

		temp = temp->next;
	}
}


void remove_merge_duplicates(anti_clique *ac) {
	anti_clique *temp = ac;
	anti_clique *temp1, *prev;

	while (temp != NULL) {
		temp1 = temp->next;
		prev = temp;

		while(temp1 != NULL) {
			if (temp->diff == temp1->diff) {
				/* remove this anti_clique nodes */
				prev->next = temp1->next;
				temp1->next = NULL;
				temp1->diff = NULL;
				free(temp1);
				temp1 = prev;
			}

			prev = temp1;
			temp1 = temp1->next;
		}

		temp = temp->next;
	}
}


/* requirement: spec1 and spec2 are alike */
/* after the execution of this function spec1 will point to its clique and spec2 to spec1's clique */
/* lists consisted of cliqueNode type nodes and anti_clique type nodes will be merged into spec1's clique lists */
void clique_rearrange(node *spec1, node *spec2) {
	cliqueNode *temp = spec1->clique->head;
	anti_clique *ac1 = spec1->clique->NegCorrel;
	anti_clique *ac2 = spec2->clique->NegCorrel;
	clique *clique1 = spec1->clique;
	clique *clique2 = spec2->clique;

	if (spec1->clique == spec2->clique)
		return;

	/* -------------------------------clique merging----------------------------------- */

	/* Find tail of spec1's clique */
	while (temp->next)
		temp = temp->next;

	/* Attach spec2's clique to that tail */
	temp->next = spec2->clique->head;

	/* Update the specs in that clique to point to the
	 * newly unified spec1 clique */
	while ((temp = temp->next)) {
		temp->spec->clique = spec1->clique;
		temp->spec->hasListOfClique = false;
	}


	/* ----------------------------anti-clique merging-------------------------------- */
	anti_clique *tmp = NULL;
	if (ac2 != NULL) {
		if (ac1 != NULL) {

			/* find tail of spec1's anti-clique list */
			while (ac1->next != NULL) {
				ac1 = ac1->next;
			}

			/* attach spec2's anti-clique to that tail */
			ac1->next = ac2;
			tmp = ac1->next;
		} else {
			/* if spec1's anti_clique list doesn't exist then just take spec2's anti_clique list */
			tmp = spec1->clique->NegCorrel = ac2;
		}

		/* negative correlation is a two-way relation */

		/* make sure the cliques that are negatively correlated with spec2->clique
		   also point to spec1->clique */
		while (tmp != NULL) {
			anti_clique *cur = tmp->diff->NegCorrel;

			/* traverse tmp->diff's anti_clique list */
			while(cur != NULL) {
				/* find the anti_clique node that points to spec2->clique */
	 			if (cur->diff == clique2) {
					cur->diff = spec1->clique; //replace pointer with spec1's clique
				}

				cur = cur->next;
			}

			tmp = tmp->next;
		}


		/* make sure that the anti_clique nodes of the cliques, that the nodes of spec2's clique point to,
		   point only once to clique1  */
		remove_duplicates(ac2, clique1);

		/* make sure that the new anti_clique list does not point to the same clique more than once */
		remove_merge_duplicates(spec1->clique->NegCorrel);
	}

	/* two cliques merged we don't need spec2's clique, free memory */
	free(clique2);
}


void print_negativeCorrelation(clique *host, node *spec, anti_clique *head, int *cnt, FILE *fp) {
	anti_clique *tmp = head;

	/* traverse list of anti_clique nodes of clique host */
	while (tmp != NULL) {
		if (tmp->one_way_relation == false) {
			clique *c = tmp->diff;
			cliqueNode *other = c->head;	//get head of cliqueNode list of the other clique

			/* traverse list of cliqueNodes that belong to a clique pointed by tmp */
			while (other != NULL) {
				/* print pairs of specs that do not belong to a clique */
				 fprintf(fp, "%s,%s,0\n", spec->id, other->spec->id);
				(*cnt)++;
				other = other->next;
			}

			/* search the anti_clique node that points back to host clique and set boolean as
			   true to avoid duplicates */
			anti_clique *ac = c->NegCorrel;
			while (ac != NULL) {
				if (ac->diff == host) {
					ac->one_way_relation = true;
					break;
				}

				ac = ac->next;
			}
		}

		tmp = tmp->next;
	}
}


/* deletes the whole clique that a spec node points to */
void delete_clique(clique *clique) {
	cliqueNode *temp;
	cliqueNode *cur = clique->head;
	/* delete list of clique */
	while (cur != NULL) {
		temp = cur;
		cur = cur->next;
		free(temp);
	}
	/* delete anti_clique list */
	delete_antiClique(clique->NegCorrel);
	/* free memory allocated for structure clique */
	free(clique);
}


/* deletes the whole anti_clique list */
void delete_antiClique(anti_clique *head) {
	anti_clique *temp;
	anti_clique *cur = head;
	while (cur != NULL) {
		temp = cur;
		cur = cur->next;
		free(temp);
	}
}


/* case: spec1, spec2, 1 */
/* check if their cliques definitely are not alike */
/* no need to check both anti_clique lists since negative correlation is a two way relation */
int find_positive_conflicts(node *spec1, node *spec2) {
	clique *clique1 = spec1->clique;
	clique *clique2 = spec2->clique;
	anti_clique *ac1 = clique1->NegCorrel;

	/* traverse anti_clique list to see if there is a node that points to spec2's clique
	   if so, then we've just found a conflict */
	while (ac1 != NULL) {
		if (ac1->diff == clique2) {	//conflict
			return 1;
		}

		ac1 = ac1->next;	//move to the next anti_clique node
	}

	return 0; //no conflict found
}


/* case: spec1, spec2, 0 */
/* check if spec1 and spec2 belong to the same clique */
int find_negative_conflicts(node *spec1, node *spec2) {
	if (spec1->clique == spec2->clique) {
		return 1;	//conflict
	} else {
		return 0;	//no conflict
	}
}
