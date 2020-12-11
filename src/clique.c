#include "spec.h"

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
	return ac;
}


/* requirement: spec1 and spec2 are alike */
/* after the execution of this function spec1 will point to its list of clique nodes and spec2
	will point to spec1's list of clique nodes */
void clique_rearrange(node *spec1, node *spec2) {
	cliqueNode *temp = spec1->clique->head;
	anti_clique *ac1 = spec1->clique->NegCorrel;
	anti_clique *ac2 = spec2->clique->NegCorrel;
	clique *clique2 = spec2->clique;
	printf("#####################################################################CLIQUE_REARRANGE\n");
	printf("spec1->clique = %p\n", spec1->clique);
	printf("spec2->clique = %p\n", spec2->clique);
	if (spec1->clique == spec2->clique)
		return;

	/* -------------------------------clique merging----------------------------------- */

	/* Find tail of spec1's clique */
	while (temp->next)
		temp = temp->next;

	/* Attach spec2's clique to that tail */
	temp->next = spec2->clique->head;
	//delete memory pointed by spec2's clique (we don't need it since 2 cliques merged)
	//printf("spec2->clique = %p\n", spec2->clique);
	//free(spec2->clique);

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
		anti_clique *other, *prev;
		other = NULL;

		/* make sure the cliques that are negatively correlated with spec2->clique
		   also point to spec1->clique */
		while (tmp != NULL) {
			printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&NEW ANTI_CLIQUE LIST\n" );
			anti_clique *cur = tmp->diff->NegCorrel;
			printf("tmp->diff = %p\n", tmp->diff);
		// 		printf("\nHEAD = %p\n", cur);
			prev = 	NULL;

			bool flag1, flag2;
			flag1 = flag2 = false;

			/* traverse tmp->diff's anti_clique list */
			while(cur != NULL) {
				printf("cur = %p\n", cur);
				/* find the anti_clique node that points to spec2->clique */
	 			if (cur->diff == clique2) {
					//printf("FOUND CLIQUE2\n");
					flag1 = true;
					cur->diff = spec1->clique; //replace pointer with spec1's clique
					prev = cur;
					cur = cur->next;
					continue;
				}

				/* check if this clique already points to spec1's clique, if so then
				 remove this anti_clique node */
				if (cur->diff == spec1->clique) {
					flag2 = true;
					other = cur;	//hold this anti_clique node

					//if it's the head of the anti_clique list that needs to be removed
					if(tmp->diff->NegCorrel == cur) {
						tmp->diff->NegCorrel = cur->next;	//assign as head next node
						prev = NULL;
						cur = cur->next;
						printf("\tDELETE OTHER = %p\n", other);
						free(other);
						// continue;/////////////
					}
					else {
						prev->next = cur->next;
						printf("\tDELETE OTHER = %p\n", other);
						free(other);
						cur = prev->next;
						// continue;
					}
					continue;
				}

				if (flag1 == true && flag2 == true) {	//just in case there is nothing else to be done for this anti_clique list break
					break;
				}
				prev = cur;
				cur = cur->next;
			}

			tmp = tmp->next;
		}

	}

	printf("FREEING CLIQUE2 = %p\n", clique2);
	free(clique2);

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
