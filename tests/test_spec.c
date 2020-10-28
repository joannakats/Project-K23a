#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "spec.h"

node *head = NULL;

int main() {
	int i;
	char *str="This is a property.";
	char *s="This a value.";
	char *id="This is an id.";

	field *f = createFieldArray(1);
	setField(f, 2, str);
	for(i=0; i<2; i++) {
		setValue(f, i, s);
	}

	head = spec_insert(head, id, f, 1);

	field *f1 = createFieldArray(2); //2 pairs property-numOfValues
	setField(&f1[0], 1, str);
	setValue(&f1[0], 0, s);
	setField(&f1[1], 2, str);
	setValue(&f1[1], 0, s);
	setValue(&f1[1], 1, s);

	node  *temp = spec_insert(head, id,  f1, 2);

	print_list(head);

	delete_specList(head);

	return 0;
}
