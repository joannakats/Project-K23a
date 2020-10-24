#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "spec.h"

//needs testing and to add insert_entry
hashtable hashtable_init(int size){
	hashtable hsTable;
	hsTable.tableSize=size;
	hsTable.list= malloc(size*sizeof(node*));
	for(int i=0;i<size;i++){
		hsTable.list[i]=NULL;
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
	if(hsTable->list[index]==NULL){
		hsTable->list[index]=spec_insert(hsTable->list[index],spec,numOfFields,properties,values);
	}else{
		temp=spec_insert(hsTable->list[index],spec,numOfFields,properties,values);
	}

}



int delete_hashtable(hashtable *hsTable){
	int size=hsTable->tableSize;
	for(int i=0;i<size;i++){
		delete_specList(hsTable->list[i]);
	}
	return 0;
}
