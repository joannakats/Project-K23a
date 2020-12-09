#include "spec.h"
#include "hashtable.h"
#ifndef _SPEC_HASHTABLE_H
#define _SPEC_HASHTABLE_H

void insert_entry(hashtable* hsTable,char* id,field *fieldsArray,int numOfFields);
node *search_hashTable_spec(hashtable* hsTable,char* id,int* pos);
void hash_table_join(hashtable* hsTable,char* left_id,char* right_id); 
void print_pairs(const hashtable *hsTable);

#endif