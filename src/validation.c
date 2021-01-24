#include "validation.h"
#include "spec_hashtable.h"
#include "loregression.h"
#include "prediction.h"
#include <unistd.h>
#define HASH_SIZE 10007

// extern logistic_regression *model;

/* allocate and initialize list of conflicts */
conflict_list *list_init() {
  conflict_list *list = malloc(sizeof(conflict_list));
  list->head = NULL;

  return list;
}


/* delete whole list of conflicts */
void delete_list(conflict_list *list) {
  conflict *tmp = list->head;
  conflict *next;

  /* traverse list to delete every conflict node */
  while (tmp != NULL) {
    next = tmp->next;
    free(tmp);
    tmp = next;
  }
  free(list);
}


/* returns 1 if list is empty or 0 if not */
int isEmpty(conflict_list *list) {
  return (list->head == NULL);
}


/* called when we find a relation of specs that causes conflicts */
void insert(conflict_list *list, node *spec1, node *spec2, bool type) {
  conflict *new = malloc(sizeof(conflict)); //allocate new node for list of conflicts
  /* point to these specs */
  new->spec1 = spec1;
  new->spec2 = spec2;
  new->type = type; //indicates type of relation between spec1 and spec2

  /* put new node at the top of the list */
  new->next = list->head;
  list->head = new;
}


void print_conflict_list(conflict_list *list) {
  puts("Conflict list:");
  conflict *c = list->head;

  while(c != NULL) {
    printf("%s, %s, %d\n", c->spec1->id, c->spec2->id, c->type);
    c = c->next;
  }
}


/*LABEL WILL BE REMOVED LATER*/
void get_specids(char *line, char *spec1, char *spec2, int *label) {
	char *token = strtok(line, " ,");
	strcpy(spec1, token);
	token = strtok(NULL , " ,");
	strcpy(spec2, token);
	///////////////////////////////////// DOWN
	token = strtok(NULL, " ,");
	char l[5];
	strcpy(l, token);
	*label = atoi(l);
}



node *insert_into_structs(hashtable *hs, char *specid, hashtable *spec_hs, bow *vocabulary) {
	int pos;
	/* search if this spec already exists */
	node *spec = search_hashTable_spec(hs, specid, &pos);
  node *original;

	if(spec == NULL) {
		spec = insert_entry_and_return(hs, specid);

    /* copy tf-idf array of the original spec to new spec */
    original = search_hashTable_spec(spec_hs, specid, &pos); //find the orignal spec in hashtable
    spec->tf_idf_factors = malloc(sizeof(double) * vocabulary->ht.count);

    /* copy every index of the array */
    for (int i = 0; i < vocabulary->ht.count; i++) {
      spec->tf_idf_factors[i] = original->tf_idf_factors[i];
    }
	}
	/* return an existing spec or a newly inserted spec */
	return spec;
}


int search_for_conflicts(node *spec1, node *spec2, bool type) {
  if (type == true) {
     return find_positive_conflicts(spec1, spec2);
  } else {
     return find_negative_conflicts(spec1, spec2);
  }
}


void resolve_positive_conflict() {

}


void resolve_negative_conflict() {

}


void resolve_conflicts(hashtable *hs, conflict_list *list) {
  /* ΓΙΑ ΚΑΘΕ CONFLICT ΠΟΥ ΕΠΙΛΥΕΙΣ ΑΛΛΑΞΕ ΒΑΡΟΣ */

  conflict *c = list->head;
  while(c != NULL) {
    if (c->type == true) {
      resolve_positive_conflict();
    } else {
      resolve_negative_conflict();
    }

    c = c->next;
  }
}


void validation(FILE *csv, int set_n, hashtable *specs, bow *vocabulary) {
  char line[256], specid1[108], specid2[108];
  int label, prediction;
  long line_n;
  node *spec1, *spec2;
  FILE *fp;

  /* copy validation set to another temporary file */
  if ((fp = fopen("temporary_file", "w+")) == NULL) {  //open temporary file for writing
    return;
  }

  for (line_n = 1; line_n <= set_n; line_n++) {
    if (!fgets(line, sizeof(line), csv)) {
			perror("Reading Dataset W for validation process");
			return;
		}
    fputs(line, fp);  //copy line to temporary file
  }

  /* get model */
  logistic_regression *model = get_model();
  printf("%p\n", model);
  printf("%d  #  %.2f  #  %p\n", model->size, model->b, model->w);
  fflush(stdout);

  // do {

    /* allocate stucts needed for this validation  round */
    hashtable hs = hashtable_init(HASH_SIZE);
    conflict_list *conflictList = list_init();

    /* ας πούμε ότι έχουμε το μοντέλο με τα βάρη και για κάθε σπεκ κάνουμε τα  predictions, και βάσει αυτού βγάζουμε όλα τα conflicts => ΣΤΟ ΤΕΛΟΣ */

    /* read from temporary file and resolve conflicts */
    fseek(fp, 0, SEEK_SET);
    while(fgets(line, sizeof(line), fp)) {
      // puts(line);
      get_specids(line, specid1, specid2, &label);
      // printf("\t %s - %s - %d\n\n", specid1, specid2, label);

      spec1 = insert_into_structs(&hs, specid1, specs, vocabulary);
      spec2 = insert_into_structs(&hs, specid2, specs, vocabulary);
      // printf("%s - %s \n", spec1->id, spec2->id); fflush(stdout);

      /* predict for  pair of specs */
      prediction = loregression_predict(model, spec1, spec2);

      /* if the relation that we are about to insert creates conflicts with the already existing relations
         then do not insert it into the stucts but keep the nodes in conflictList */
      if (prediction == 1) {

        /* if conflicts are found insert into conflictList */
        if (search_for_conflicts(spec1, spec2, true) == 1) {
          insert(conflictList, spec1, spec2, true);
        } else {
          /* else normally merge cliques */
          hash_table_join(&hs, specid1, specid2);
        }

      } else {

        /* if conflicts are found insert into conflictList */
        if (search_for_conflicts(spec1, spec2, false) == 1) {
          insert(conflictList, spec1, spec2, false);
        } else {
          /* else normally set a negative correlation between these specs */
          hash_table_notjoin(&hs, specid1, specid2);
        }

      }
    }
    //
    print_conflict_list(conflictList);
    // puts("//////////////////////////////////////////");
    // // print_relations(&hs, stdout);
    //
    // /* if conflict list is not empty resolve conflicts and backpropagate */
    // if (!isEmpty(conflictList)) {
    //   resolve_conflicts(&hs, conflictList);
    // }

    /* free memory allocated for this validation round */
    delete_hashtable(&hs);
    delete_list(conflictList);

  //} while (/* ???? */);

  /* close temporary file of validation set and unlink it */
  fclose(fp);
  unlink("temporary_file");
}
