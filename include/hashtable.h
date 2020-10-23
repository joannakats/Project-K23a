#include "spec.h"
typedef struct hashtable {
    int index;
    node* head;

}hashtable;


hashtable *hashtable_init(int tableSize);
//hash function ( djb2 from http://www.cse.yorku.ca/~oz/hash.html)
unsigned  long hash(const char* key,int tableSize);
void insert_entry(hashtable *hsTable,int index,char* spec,int numOfFields,char **properties,char **values); 
int delete_hasht