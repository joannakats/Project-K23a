#include "spec.h"

/* creates the spec node and initializes its attrubutes */
/* requirement: the arrays of fields, properties and values, have been allocated
	 and initialized elsewhere */
/* returns the spec node that has just been inserted */
node *spec_init(char *id, hashtable *HSfields) {
	/* allocate memory of spec */
	node *spec = malloc(sizeof(node));

	spec->id = malloc(sizeof(char)*(strlen(id)+1));
	strcpy(spec->id, id);

	spec->next = NULL;
	spec->clique = clique_init(spec);
	spec->hasListOfClique = true; //initially every new spec node points to a cliqueNode

	spec->fields = HSfields; //set fields to the array that is already allocated and initialized

	spec->bow_occurences = NULL;
	spec->tf_idf_factors = NULL;

	return spec;
}


/* procedure to initialize hashtable for fields */
hashtable *field_init(int size) {
	hashtable *hs;
	/* allocate memory for data structure */
	hs = malloc(sizeof(hashtable));

	*hs = hashtable_init(size);

	return hs;
}


/* procedure that adds a field to the hashtable, more specifically
   inserts a field node and initializes property (values are not initialized yet)*/
field *HSfield_insert(hashtable *hs, char *property) {
	unsigned long index = hash(property, hs->tableSize);

	field *f = malloc(sizeof(field)); //allocate memory for a field node
	f->property = strdup(property);
	f->values = malloc(sizeof(char *));
	f->cnt = 0;

	/* insert at the top of the overflow chain */
	f->next = hs->list[index];
	hs->list[index] = f;

	hs->count++;	//increment counter of fields

	return f;
}


void setValue(field *f, char *value) {
	f->cnt++;	//increment counter of values
	f->values = realloc(f->values, f->cnt * sizeof(char *)); //resize array of values

	f->values[f->cnt - 1] = strdup(value);
}



void deleteFieldNode(field *f) {
	free(f->property);
	//free array of strings
	for (int i = 0; i < f->cnt; i++) {
		free(f->values[i]);
	}
	free(f->values);
	free(f);
}


/* procedure to delete hashtable of fields */
void deleteFields(hashtable *fields) {
	field *cur, *temp;
	for(int i = 0; i < fields->tableSize; i++) {
		cur = fields->list[i];

		while (cur != NULL) {
			temp = cur->next;
			deleteFieldNode(cur);
			cur = temp;
		}
	}
	free(fields->list);
	free(fields);
}
//////////////////////////////////////////////////////////////

/* inserts a spec into the overflow chain */
node *spec_insert(node *head, char *id, hashtable *fields) {
	node *new_node = spec_init(id, fields);  //create the new node of spec

	new_node->next = head;	// new node will be inserted at the top of the list

	return new_node;
}


/* frees memory allocated for a spec node */
void delete_specNode(node *spec) {
	/* delete array of fields */
	if (spec->fields != NULL) {
		deleteFields(spec->fields);
	}

	if (spec->hasListOfClique) { //make sure we don't free something we have already freed
		delete_clique(spec->clique);
	}

	free(spec->id);

	if (spec->bow_occurences)
		free(spec->bow_occurences);

	if (spec->bow_occurences)
		free(spec->tf_idf_factors);

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


/* returns field node found in the hashtable  of fields in spec */
field *search_field(hashtable *fields, char *property) {
	unsigned long index = hash(property, fields->tableSize);

	field *cur = fields->list[index];
	while(cur != NULL) {
		if (strcmp(cur->property, property) == 0) {
			return cur;
		}
		cur = cur->next;
	}
	return NULL;
}
