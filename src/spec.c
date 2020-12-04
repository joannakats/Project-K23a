#include "spec.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_list(node *head) {
	node *current = head;
	while(current != NULL) {
		print_fields(current->fields, current->fieldCount);
		current = current->next;
	}
}

void print_fields(field *array, int dim) {
	for (int i = 0; i < dim; i++) {
		printf("%s: ", array[i].property);
		for (int j = 0; j < array[i].cnt; j++) {
			printf("%s ", array[i].values[j]);
		}
		printf("\n");
	}
	printf("\n\n");
}


/* creates the spec node and initializes its attrubutes */
/* requirement: the arrays of fields, properties and values, have been allocated
	 and initialized elsewhere */
/* returns the spec node that has just been inserted */
node *spec_init(char *id, field *fieldsArray, int numOfFields) {
	/* allocate memory of spec */
	node *spec = malloc(sizeof(node));

	spec->id = malloc(sizeof(char)*(strlen(id)+1));
	strcpy(spec->id, id);

	spec->next = NULL;
	spec->clique = clique_init(spec);
	spec->hasListOfClique = true; //initially every new spec node points to a cliqueNode

	spec->fields = fieldsArray; //set fields to the array that is already allocated and initialized
	spec->fieldCount = numOfFields;

	return spec;
}


/* allocates memory for the array of fields */
field *createFieldArray(int dim) {
	field *arr = malloc(sizeof(field)*dim);
	return arr;
}


/* sets property and allocates memory for the array of values */
void setField(field *field, int numOfValues, char *property) {
	field->property = strdup(property);
	field->cnt = numOfValues;
	field->values = malloc(numOfValues * sizeof(char*));
	for (int i = 0; i < numOfValues; i++) {
		field->values[i] = NULL;
	}
}


/* set a certain value of the array f->values to value */
void setValue(field *f, int index, char *value) {
	f->values[index] = strdup(value);
}


void deleteField(field f) {
	free(f.property);
	//free array of strings
	for (int i = 0; i < f.cnt; i++) {
		free(f.values[i]);
	}
	free(f.values);
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


/* inserts a spec into the overflow chain */
node *spec_insert(node *head, char *id, field *fieldsArray, int numOfFields) {
	node *new_node = spec_init(id, fieldsArray, numOfFields);  //create the new node of spec

	new_node->next = head;	// new node will be inserted at the top of the list

	return new_node;
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


/* frees memory allocated for a spec node */
void delete_specNode(node *spec) {
	/* delete array of fields */
	for (int i = 0; i < spec->fieldCount; i++) {
		deleteField(spec->fields[i]);
	}
	if(spec->fields != NULL) {
		free(spec->fields);
	}

	if (spec->hasListOfClique) { //make sure we don't free something we have already freed
		delete_clique(spec->clique);
	}

	free(spec->id);
	free(spec);
}


/* deletes the whole overflow chain */
void delete_specList(node *head) {
	node *cur = head;
	node *temp;
	while(cur != NULL) {
		temp = cur;
		cur = cur->next;
		delete_specNode(temp);
	}
}


/* returns the spec with the given id if it exists in the list,
	 otherwise it returns NULL */
node *search_spec(node *head, char *id, int *pos) {
	node *current = head;
	*pos = 1;
	while (current != NULL) {
		if (strcmp(current->id, id) == 0) {
			return current;
		}
		current = current->next;
		(*pos)++;
	}
	/* at this point there hasn't been found a spec with this id */
	*pos = -1;
	return NULL;
}


/* requirement: spec1 and spec2 are alike */
/* after the execution of this function spec1 will point to its list of clique nodes and spec2
	will point to spec1's list of clique nodes */
void clique_rearrange(node *spec1, node *spec2) {
	cliqueNode *temp = spec1->clique->head;

	if (spec1->clique == spec2->clique)
		return;

	/* clique merging */
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


	/* anti-clique merging */
	anti_clique *ac1 = spec1->clique->NegCorrel;
	anti_clique *ac2 = spec2->clique->NegCorrel;

	if (ac2 != NULL) {

		/* find tail of spec1's anti-clique list */
		if (ac1 != NULL) {
			while (ac1->next != NULL) {
				ac1 = ac1->next;
			}
		}

		/* attach spec2's anti-clique to that tail */
		ac1->next = ac2;

		/* negative correlation is a two-way relation */
		anti_clique *tmp = ac1;
		anti_clique *other, *prev, *prev1;

		/* make sure the cliques that are negatively correlated with spec2->clique
		   also point to spec1->clique */
		while ((tmp = tmp->next)) {
			anti_clique *cur = tmp->diff->NegCorrel;
			prev = cur;

			bool flag = false;

			/* traverse tmp->diff's anti_clique list */
			while(cur != NULL) {

				/* find the anti_clique node that points to spec2->clique */
				if (cur->diff == spec2->clique) {
					other = cur;
					prev1 = prev;
				}

				if (cur->diff == spec1->clique) {	//check if this clique already points to spec1's clique
					flag = true;
				}

				if(cur->next != NULL) {
					prev = cur;
				}
				cur = cur->next;
			}

			if (flag == false) {
				other->diff = spec1->clique;
			}else{  //remove this anti_clique node
				if (other == prev1) //if it's the first anti_clique node that we need to remove
					tmp->diff->NegCorrel = other->next;
				else
					prev1->next = other->next;
				free(other);
			}
		}

	}

	//delete memory pointed by spec2's clique (we don't need it since 2 cliques merged)
	free(spec2->clique);
	spec2->clique = spec1->clique;
}


/* this function is called when spec1 and spec2 are definitely not alike */
void anti_clique_insert(node *spec1, node *spec2) {
	anti_clique *ac1 = spec1->clique->NegCorrel;

	/* search if a negative correlation between spec1 and spec2 already exists */
	while (ac1 != NULL) {
		/* if there is already a negative correlation between spec1's and spec2's cliques
		   there is nothing to be done */
		if (ac1->diff == spec2->clique)	//no need to check for both, negative correlation is a two way relation
			return;

		ac1 = ac1->next;
	}


	/* every new anti_clique node is put at the top of the anti_clique list */
	spec1->clique->NegCorrel = anti_clique_init(spec2->clique, spec1->clique->NegCorrel);
	spec2->clique->NegCorrel = anti_clique_init(spec1->clique, spec2->clique->NegCorrel);
}


/* allocates memory for anti_clique structure and initializes */
anti_clique *anti_clique_init(clique *c, anti_clique *next) {
	anti_clique *ac = malloc(sizeof(anti_clique));
	ac->next = next;
	ac->diff = c;
	return ac;
}
