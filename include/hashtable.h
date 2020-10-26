#include "spec.h"
typedef struct hashtable {
    int tableSize;
    node** list;

}hashtable;


hashtable hashtable_init(int size);
//hash function ( djb2 from http://www.cse.yorku.ca/~oz/hash.html)
unsigned  long hash(const char* key,int tableSize);
void insert_entry(hashtable* hsTable,char* spec,int numOfFields,char **properties,char **values);
int delete_hashtable(hashtable *hsTable);