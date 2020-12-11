#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

hashtable hashtable_init(int size){
	hashtable hsTable;
	hsTable.tableSize=size;
	hsTable.count=0;
	hsTable.list= malloc(size*sizeof(void*));
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
	result=result%tableSize;
	return result;
}
