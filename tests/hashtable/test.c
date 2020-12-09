#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "spec_hashtable.h"
#include "spec.h"
#include "acutest.h"


void test_insert_hashtable(void){

	char *spec_id1="This is spec_id1";
	char *spec_id2="This is spec_id2";
	char *prop="This is property";
	char *val="This is value";
	int entries=10;
	int index;

 //first spec 
	field *spec1=createFieldArray(1);
	TEST_CHECK(spec1!=NULL);
	setField(spec1,1,prop); //setField has been tested in spec/test.c
	setValue(spec1,0,val); //setValue has been tested in spec/test.c
//2nd spec
	field *spec2=createFieldArray(2);
	TEST_CHECK(spec2!=NULL);
	setField(&spec2[0], 2, prop);
	setField(&spec2[1], 1, prop);

//test hashtable init
 	hashtable hsTable=hashtable_init(entries);
 	TEST_CHECK(hsTable.tableSize==entries);
//test hashtable_insert_entries
  	insert_entry(&hsTable,spec_id1,spec1,1);
  	index=hash(spec_id1,entries); 
//test hash
  	TEST_CHECK(index>=0);
  	TEST_CHECK(hsTable.list[index]!=NULL); //spec_insert list has been tested in spec/test.c

	index=hash(spec_id2,entries);
	TEST_CHECK(index>=0);
  	insert_entry(&hsTable,spec_id2,spec2,2);
  	TEST_CHECK(hsTable.list[index]!=NULL); 
	
	delete_hashtable(&hsTable);

}

void test_join_hashtable(void){

	char *prop="This is property";
	char *val="This is value";
	char spec[5];
	int entries=10,pos,i=0;
	node *temp;
	node *current;

	hashtable hsTable=hashtable_init(entries);

 //first spec 
	field *data=createFieldArray(1);
	TEST_CHECK(data!=NULL);
	setField(data,1,prop); 
	setValue(data,0,val); 

//insert entries in hashtable.Number 2*entries is just a random number.
	for(i=0;i<2*entries;i++){
		sprintf(spec,"%d",i);
		insert_entry(&hsTable,spec,data,1);
	}

//test_hash_table_join and search_hashtable_spec
	for(i=0;i<entries;i++){
		sprintf(spec,"%d",i);
		temp=search_hashTable_spec(&hsTable,spec,&pos);
		TEST_CHECK(temp!=NULL);
		TEST_CHECK(pos>=1);// because we use hash to insert the specs.
		TEST_CHECK(strcmp(temp->id,spec) == 0);  
		TEST_MSG("Unexpected search temp");
	}
//free memory
	deleteField(*data);
	free(data);
	for(i=0;i<entries;i++){
		current=hsTable.list[i];
		while(current !=NULL){
			temp=current;
			current=current->next;
			if(temp->hasListOfClique){
				delete_clique(temp->clique);
			}
			free(temp->id);
			free(temp);
			
		}
	}
	free(hsTable.list);



} 



TEST_LIST = {
	{"hashtable_insertion",test_insert_hashtable}, 
	{"hashtable_search",test_join_hashtable},
	{NULL, NULL}
};
