#include <stdio.h>

#include "spec.h"

// Working with specs because of insert() convinience, but clique is the same idea (list)

/* This should print the combinations (distinct pairs) of a list,
 * which is the objective of the output csv.
 * Since we have iterated though left on the previous pass, the right node for
 * the next pass will start from just AFTER <left>.
 * e.g. we have A,B,C. Then AB AD AC (BA) BD BC (CA CB) CD
 * -> the pairs in parentheses are handled by the previous pass */
void print_pairs(node *head) {
	node *left, *right;

	left = head;
	while (left->next) {
		right = left->next;
		while (right) {
			printf("%s,%s\n", left->id, right->id);
			right = right->next;
		}

		left = left->next;
	}
}

int main() {
	node *head = NULL;

	head = spec_insert(head, "A", 0, NULL, NULL);
	spec_insert(head, "C", 0, NULL, NULL);
	spec_insert(head, "B", 0, NULL, NULL);
	//spec_insert(head, "D", 0, NULL, NULL);
	//spec_insert(head, "K", 0, NULL, NULL);

	print_pairs(head);

	delete_specList(head);
}
