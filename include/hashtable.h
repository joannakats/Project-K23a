#include "spec.h"
#ifndef _HASHTABLE_H
#define _HASHTABLE_H

typedef struct hashtable {
    int tableSize;
    node** list;

}hashtable;


hashtable hashtable_init(int size);
//hash function ( djb2 from http://www.cse.yorku.ca/~oz/hash.html)
unsigned  long hash(const char* key,int tableSize);
void insert_entry(hashtable* hsTable,char* id,field *fieldsArray,int numOfFields);
node *search_hashTable_spec(hashtable* hsTable,char* id,int* pos);
void hash_table_join(hashtable* hsTable,char* left_id,char* right_id); 
int delete_hashtable(hashtable *hsTable);

#endif