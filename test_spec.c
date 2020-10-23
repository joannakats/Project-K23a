#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "spec.h"

node *head = NULL;

int main() {
	char **prop1, **prop2, **prop3, **v1, **v2, **v3;
	int cnt1=3, cnt2=2, cnt3=1;
	int i;
	char *str="This is a property.";
	char *s="This a value.";
	char *id="This is an id.";
	prop1 = malloc(cnt1*sizeof(char*));
	prop2 = malloc(cnt2*sizeof(char*));
	prop3 = malloc(cnt3*sizeof(char*));
	v1 = malloc(cnt1*sizeof(char*));
	v2 = malloc(cnt2*sizeof(char*));
	v3 = malloc(cnt3*sizeof(char*));
	

	for(i=0; i<cnt1; i++) {
		prop1[i] = malloc(sizeof(char)*(strlen(str)+1));
		v1[i] = malloc(sizeof(char)*(strlen(s)+1));
		strcpy(prop1[i], str);
		strcpy(v1[i], s);
	}
	
	for(i=0; i<cnt2; i++) {
		prop2[i] = malloc(sizeof(char)*(strlen(str)+1));
		v2[i] = malloc(sizeof(char)*(strlen(s)+1));
		strcpy(prop2[i], str);
		strcpy(v2[i], s);
	}
	
	for(i=0; i<cnt3; i++) {
		prop3[i] = malloc(sizeof(char)*(strlen(str)+1));
		v3[i] = malloc(sizeof(char)*(strlen(s)+1));
		strcpy(prop3[i], str);
		strcpy(v3[i], s);
	}
	
	head = insert(head, id,  cnt1, prop1, v1);
	node  *temp = insert(head, id,  cnt2, prop2, v2);
	temp = insert(head, id,  cnt3, prop3, v3);
	
	print_list(head);

	delete_specList(head);

	return 0;
}
