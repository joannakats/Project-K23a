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
cliqueNode *clique_init(node *spec) {
	cliqueNode *cn = malloc(sizeof(cliqueNode));

	cn->next = NULL;
	cn->spec = spec;
	return cn;
}


/* inserts a spec into the overflow chain */
node *spec_insert(node *head, char *id, field *fieldsArray, int numOfFields) {
	node *new_node = spec_init(id, fieldsArray, numOfFields);  //create the new node of spec

	/* if the list is empty */
	if (head == NULL) {
		return new_node;
	}

	/* if there is only one element */
	if (head->next == NULL) {
		head->next = new_node;
		return new_node;
	}

	/* traverse the list  */
	node *cur = head->next;
	while(cur->next != NULL) {
		cur = cur->next;
	}
	/*add at the end of the list*/
	cur->next = new_node;
	return new_node;
}


/* deletes the whole clique that a spec node points to */
void delete_clique(cliqueNode *node) {
	cliqueNode *temp;
	cliqueNode *cur = node;
	while (cur != NULL) {
		temp = cur;
		cur = cur->next;
		free(temp);
	}
}


/* frees memory allocated for a spec node */
void delete_specNode(node *spec) {
	/* delete array of fields */
	for (int i = 0; i < spec->fieldCount; i++) {
		deleteField(spec->fields[i]);
	}
	free(spec->fields);

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

	/* to avoid wrong values for boolean hasListOfClique in struct node */
	if (spec1->hasListOfClique == true && spec2->hasListOfClique == false) {
		node *tmp = spec1;
		spec1 = spec2;
		spec2 = tmp;
	}

	node *other_spec;
	/* in this case two lists of cliques are merging that none of the arguments holds */
	if (spec1->hasListOfClique == false && spec2->hasListOfClique == false) {
		cliqueNode *head = spec2->clique;
		other_spec = search_HostSpec(head);
		other_spec->clique = spec1->clique;
		other_spec->hasListOfClique = false;
	}

	cliqueNode * temp = spec2->clique; //head of the list of clique nodes that spec2 points to
	spec2->clique = spec1->clique;
	spec2->hasListOfClique = false;

	/* traverse spec1 clique to insert spec2's clique */
	cliqueNode *clN = spec1->clique;
	while(clN->next != NULL) {
		clN = clN->next;
	}

	clN->next = temp;
}


/* searches in a list of clique nodes for the node spec that "keeps" this list */
node *search_HostSpec(cliqueNode *head) {
	cliqueNode *cur = head;
	while(cur != NULL) {
		if (cur->spec->hasListOfClique == true) {
			return cur->spec;
		}
		cur = cur->next;
	}
	return NULL; // something is wrong
}
