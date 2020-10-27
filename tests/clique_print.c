#include <stdio.h>

#include "spec.h"

// Working with specs because of insert() convinience, but clique is the same idea (list)

/* This should print the combinations (distinct pairs) of a list
 * Which is the objective of the output csv.
 * The movement of the start pointer prevents the repeating of pairs in swapped order. */
void print_pairs(node *head) {
	node *left, *right, *start;

	left = head;
	start = head;
	while (left->next) {
		right = start;

		while (right) {
			if (left != right)
				printf("%s,%s\n", left->id, right->id);

			right = right->next;
		}

		left = left->next;
		start = start->next;
	}
}

int main() {
	node *head = NULL;

	head = spec_insert(head, "A", 0, NULL, NULL);
	spec_insert(head, "C", 0, NULL, NULL);
	spec_insert(head, "B", 0, NULL, NULL);
	spec_insert(head, "D", 0, NULL, NULL);
	spec_insert(head, "K", 0, NULL, NULL);

	print_pairs(head);

	delete_specList(head);
}
