#include "spec.h"
typedef struct hashtable {
    int index;
    node* head;

}hashtable;


hashtable *hashtable_init(int tableSize);
//hash function ( djb2 from http://www.cse.yorku.ca/~oz/hash.html)
unsigned  long hash(const char* key,int tableSize);
void insert_entry(hashtable *hsTable,int key,char* data); 
int delete_hashtable(hashtable *hsTable,int tableSize);
