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

void insert_entry(hashtable* hsTable,int index,char* spec,int numOfFields,char **properties,char **values){// 0<=index<tableSize
	node* temp;
	if(hsTable[index].head==NULL){
		hsTable[index].head=spec_insert(hsTable[index].head,spec,numOfFields,properties,values);
	}else{
		temp=spec_insert(hsTable[index].head,spec,numOfFields,properties,values);
	}

}


int delete_hashtable(hashtable *hsTable,int tableSize){
	for(int i=0;i<tableSize;i++){
		delete_specList(hsTable[i].head);
	}
	free(hsTable);
	return 0;
}