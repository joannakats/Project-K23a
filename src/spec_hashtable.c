#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spec_hashtable.h"

void insert_entry(hashtable* hsTable,char* id,hashtable **fields) {
	int index = hash(id, hsTable->tableSize);
	hsTable->list[index] = spec_insert(hsTable->list[index],id,*fields);
	hsTable->count++;
}

node *search_hashTable_spec(hashtable* hsTable,char* id,int *pos){
	int index=hash(id,hsTable->tableSize);
	node *found=search_spec(hsTable->list[index],id,pos);
	return found;
}

/*The following function search for left and right spec.If both specs are found
left spec will point to its list of clique nodes and right spec
will point to left_spec's list of clique nodes */
void hash_table_join(hashtable* hsTable,char* left_id,char* right_id){
	int left_pos,right_pos;
	node *left_spec=search_hashTable_spec(hsTable,left_id,&left_pos);
	node *right_spec=search_hashTable_spec(hsTable,right_id,&right_pos);
	if(left_pos!=-1 && right_pos!=-1)
				clique_rearrange(left_spec,right_spec);

}

void hash_table_notjoin(hashtable* hsTable,char* left_id,char* right_id){
	int left_pos,right_pos;
	node *left_spec=search_hashTable_spec(hsTable,left_id,&left_pos);
	node *right_spec=search_hashTable_spec(hsTable,right_id,&right_pos);
	if(left_pos!=-1 && right_pos!=-1)
		anti_clique_insert(left_spec,right_spec);

}

/* Τhe following function prints the first node of the clique with the following nodes,
the second with its next ones and so on for every clique list. */
void print_pairs(const hashtable *hsTable){
	if(hsTable!=NULL){
		node* current;
		cliqueNode *cliqueLeft,*cliqueRight;
		for(int i=0;i<hsTable->tableSize;i++){
			//printf("\nPosition in the hashtable : %d \n",i);
			current=hsTable->list[i];
			while(current!=NULL){
				//printf("\nSpec: %s \n",current->id);
				if(current->hasListOfClique==true){
				//	printf("Clique: ");
					cliqueLeft=current->clique->head;
					while(cliqueLeft->next!=NULL){
						cliqueRight=cliqueLeft->next;
						while(cliqueRight!=NULL){
							printf("%s,%s\n",cliqueLeft->spec->id,cliqueRight->spec->id);
							cliqueRight=cliqueRight->next;
						}
						cliqueLeft=cliqueLeft->next;
					}
				}
				current=current->next;
			}

		}
	}
}


/* procedure to print every pair of specs */
int print_relations(const hashtable *hs, FILE *fp) {
	if (hs == NULL)
		return -2;

	if (fp == NULL)
		return -3;

	node *cur;
	cliqueNode *left, *right;
	anti_clique *tmp;
	int counter = 0;	//counter of pairs printed

	for (int i = 0; i < hs->tableSize; i++) {
		cur = hs->list[i];

		/* traverse overflow chain */
		while (cur != NULL) {
			if (cur->hasListOfClique == true) {
				left = cur->clique->head;

				/* traverse list of cliqueNodes */
				while(left->next != NULL) {
					tmp = cur->clique->NegCorrel;//////////////
					right = left->next;

					while(right != NULL) {
						/* print pairs of specs that belong to a clique */
						fprintf(fp, "%s,%s,1\n", left->spec->id, right->spec->id);
						counter++;
						right = right->next;
					}

					/* for cliqueNode left traverse list of anti_clique nodes */
					while (tmp != NULL) {
						if (tmp->one_way_relation == true) {
							clique *c = tmp->diff;
							cliqueNode *other = c->head;	//get head of cliqueNode list of the other clique

							/* traverse list of cliqueNodes of this clique*/
							while (other != NULL) {
								/* print pairs of specs that do not belong to a clique */
								fprintf(fp, "%s,%s,0\n", left->spec->id, other->spec->id);
								counter++;
								other = other->next;
							}
						}

						tmp = tmp->next;
					}

					left = left->next;
				}

				/* last cliqueNode - print negative correlation */
				/* for the last cliqueNode traverse list of anti_clique nodes */
				tmp = cur->clique->NegCorrel;
				while (tmp != NULL) {
					if (tmp->one_way_relation == true) {
						clique *c = tmp->diff;
						cliqueNode *other = c->head;

						/* traverse list of cliqueNodes of this clique*/
						while (other != NULL) {
							/* print pairs of specs that do not belong to a clique */
							fprintf(fp, "%s,%s,0\n", left->spec->id, other->spec->id);
							counter++;
							other = other->next;
						}
					}

					tmp = tmp->next;
				}
			}

			cur = cur->next;
		}
	}
	return counter;
}


int delete_hashtable(hashtable *hsTable){
	int size=hsTable->tableSize;
	for(int i=0;i<size;i++){
		delete_specList(hsTable->list[i]);
	}
	free(hsTable->list);
	return 0;
}
