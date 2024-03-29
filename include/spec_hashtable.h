#include "spec.h"
#include "hashtable.h"
#ifndef _SPEC_HASHTABLE_H
#define _SPEC_HASHTABLE_H

void insert_entry(hashtable* hsTable,char* id,hashtable **fields);
node *insert_entry_and_return(hashtable* hsTable, char* id);
node *search_hashTable_spec(hashtable* hsTable,char* id,int* pos);
void hash_table_join(hashtable* hsTable,char* left_id,char* right_id);
void hash_table_notjoin(hashtable* hsTable,char* left_id,char* right_id);
void print_pairs(const hashtable *hsTable);
int print_relations(const hashtable *hs, FILE *fp);
int delete_hashtable(hashtable *hsTable);

#endif
