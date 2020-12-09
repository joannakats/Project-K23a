#include "spec.h"
#ifndef _HASHTABLE_H
#define _HASHTABLE_H

typedef struct hashtable {
    int tableSize;
    void** list;

}hashtable;


hashtable hashtable_init(int size);
//hash function ( djb2 from http://www.cse.yorku.ca/~oz/hash.html)
unsigned  long hash(const char* key,int tableSize);
int delete_hashtable(hashtable *hsTable);

#endif
