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

void insert_entry(hashtable* hsTable,char* id,field *fieldsArray,int numOfFields) {
	int index = hash(id, hsTable->tableSize);
	node *temp = spec_insert(hsTable->list[index],id,fieldsArray,numOfFields);
	if(hsTable->list[index]==NULL)
		hsTable->list[index] = temp;
}

node *search_hashTable_spec(hashtable* hsTable,char* id,int *pos){
	int index=hash(id,hsTable->tableSize);
	node *found=search_spec(hsTable->list[index],id,pos);
	return found;
}



void hash_table_join(hashtable* hsTable,char* left_id,char* right_id){
	int left_pos,right_pos;
	node *left_spec=search_hashTable_spec(hsTable,left_id,&left_pos);
	node *right_spec=search_hashTable_spec(hsTable,right_id,&right_pos);
	//both spec_ids where found
	if(left_pos!=-1 && right_pos!=-1)
				clique_rearrange(left_spec,right_spec);
	

}		
	



int delete_hashtable(hashtable *hsTable){
	int size=hsTable->tableSize;
	for(int i=0;i<size;i++){
		delete_specList(hsTable->list[i]);
	}
	free(hsTable->list);
	return 0;
}
