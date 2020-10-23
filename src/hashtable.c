#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "spec.h"

//needs testing and to add insert_entry
hashtable *hashtable_init(int tableSize){
	hashtable *hsTable= malloc(tableSize*sizeof(hashtable));
	for(int i=0;i<tableSize;i++){
		hsTable[i].index=i;
		hsTable[i].head=NULL;
    }
    return hsTable;
}

unsigned  long  hash(const char* key,int tableSize){
	unsigned long  result=5381;
	char *p;
	p=(char*)key;
	while(*p !='\0'){
		result=(result<<5)+result+*p;
		++p;
	}
	result=result%tableSize; //
	return result;
}

void insert_entry(hashtable *hsTable,int key,char* data){//where data is the spec and key>=0 && key<tableSize
	//to use node*insert(node *head, char *id, char *numOfFields, char **properties, char **values) 
	//we need to read first info from specs and search( list) if spec already exists.

}


int delete_hashtable(hashtable *hsTable,int tableSize){
	for(int i=0;i<tableSize;i++){
		delete_specList(hsTable[i].head);
	}
	free(hsTable);
	return 0;
}