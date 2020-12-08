#include "spec.h"

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


/* inserts a spec into the overflow chain */
node *spec_insert(node *head, char *id, field *fieldsArray, int numOfFields) {
	node *new_node = spec_init(id, fieldsArray, numOfFields);  //create the new node of spec

	new_node->next = head;	// new node will be inserted at the top of the list

	return new_node;
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
