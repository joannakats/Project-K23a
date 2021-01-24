#include "validation.h"
#include "spec_hashtable.h"
#include "prediction.h"
#include <unistd.h>
#include <math.h>
#define HASH_SIZE 10007


/* allocate and initialize list of conflicts */
conflict_list *list_init() {
  conflict_list *list = malloc(sizeof(conflict_list));
  list->head = NULL;
  list->counter = 0;

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
void insert(conflict_list *list, node *spec1, node *spec2, double prediction, bool type) {
  conflict *new = malloc(sizeof(conflict)); //allocate new node for list of conflicts
  /* point to these specs */
  new->spec1 = spec1;
  new->spec2 = spec2;
  new->type = type; //indicates type of relation between spec1 and spec2
  new->prediction = prediction;

  /* put new node at the top of the list */
  new->next = list->head;
  list->head = new;
  list->counter++;
}


void print_conflict_list(conflict_list *list) {
  puts("Conflict list:");
  conflict *c = list->head;

  while(c != NULL) {
    printf("%s, %s, %d\n", c->spec1->id, c->spec2->id, c->type);
    c = c->next;
  }
}


void get_specids(char *line, char *spec1, char *spec2, int *label) {
	char *token = strtok(line, " ,");
	strcpy(spec1, token);
	token = strtok(NULL , " ,");
	strcpy(spec2, token);

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

/////////////////////////////////////////////////////////////////////////////////////////

void resolve_positive_conflict(logistic_regression *model, conflict *conflict) {
  double average_of_cliques = 0.0;
  int cnt = 0; //counter of relations
  node *spec1 = conflict->spec1;
  node *spec2 = conflict->spec2;

  /* check if these cliques have been previously merged during another conflict resolution */
  if (spec1->clique == spec2->clique) {
    return; //if so, ignore, it is no longer a conflict
  }

  cliqueNode *c1 = spec1->clique->head;
  cliqueNode *c2;

  /* -----predict all possible relations of these 2 cliques to compute average prediction -----*/
  while (c1 != NULL) {
    cliqueNode *tmp = c1->next;

    while(tmp != NULL) {
      cnt++;
      average_of_cliques += loregression_possibility(model, c1->spec, tmp->spec);

      tmp = tmp->next;
    }

    c2 = spec2->clique->head;
    while(c2 != NULL) {
      cnt++;
      average_of_cliques += loregression_possibility(model, c1->spec, c2->spec);

      c2 = c2->next;
    }

    c1 = c1->next;
  }

  c2 = spec2->clique->head;
  while(c2 != NULL) {
    cliqueNode *tmp = c2->next;

    while (tmp != NULL) {
      cnt++;
      average_of_cliques += loregression_possibility(model, c2->spec, tmp->spec);

      tmp = tmp->next;
    }

    c2 = c2->next;
  }

  /* get average */
  average_of_cliques /= cnt;

  /* decision */
  double conflict_diff = 1 - conflict->prediction;

  /* check which converges more */
  if (conflict_diff < average_of_cliques) {
    /* update relations of two cliques */
    /* the two cliques will be merged */
    remove_negCorrelation(spec1->clique, spec2->clique);
    remove_negCorrelation(spec2->clique, spec1->clique);
    clique_rearrange(spec1, spec2);

    /* update weights of this clique */
    update_weights_of_clique(model, spec1->clique);

  } else {
    /* change weights of relation that caused the conflict */
    loregression_update_weights_of_pair(model, spec1, spec2, 0.0);
  }
}


void resolve_negative_conflict(logistic_regression *model, conflict *conflict) {
  double average = 0.0;
  int cnt = 0; //counter of relations predicted

  /* check if this clique has been previously split */
  if (conflict->spec1->clique != conflict->spec2->clique) {
    return; //if so, ignore, it is no longer a conflict
  }

  /* traverse cliqueNode list to find the possibility of similarity */
  cliqueNode *cn1, *cn2;
  cn1 = conflict->spec1->clique->head;

  while(cn1 != NULL) {
    cn2 = cn1->next;

    while(cn2 != NULL) {
      average += loregression_possibility(model, cn1->spec, cn2->spec);

      cn2 = cn2->next;
    }
    cn1 = cn1->next;
  }

  /* get average */
  average /= cnt;

  /* decision */
  double average_diff = 1 - average;
  /* check which converges more */
  if (conflict->prediction < average_diff) {
    /* split clique and update weights */
    split_clique(conflict->spec1, conflict->spec2);
    loregression_update_weights_of_pair(model, conflict->spec1, conflict->spec2, 0.0);
  } else {
    /* change the relation that caused the conflict and updates weights for that relation */
    loregression_update_weights_of_pair(model, conflict->spec1, conflict->spec2, 1.0);
  }
}


void resolve_conflicts(logistic_regression *model, conflict_list *list) {
  conflict *c = list->head;
  while(c != NULL) {
    if (c->type == true) {
      resolve_positive_conflict(model, c);
    } else {
      resolve_negative_conflict(model, c);
    }

    c = c->next;
  }
}

////////////////////////////////////////////////////////////////////////////////////

void validation(FILE *csv, int set_n, hashtable *specs, bow *vocabulary) {
  char line[256], specid1[108], specid2[108];
  int label, prediction, counter = 0, initial_numOfConflicts = 0, difference = 0;
  float condition;
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

  do {

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
          insert(conflictList, spec1, spec2, prediction, true);
        } else {
          /* else normally merge cliques */
          hash_table_join(&hs, specid1, specid2);
        }

      } else {

        /* if conflicts are found insert into conflictList */
        if (search_for_conflicts(spec1, spec2, false) == 1) {
          insert(conflictList, spec1, spec2, prediction, false);
        } else {
          /* else normally set a negative correlation between these specs */
          hash_table_notjoin(&hs, specid1, specid2);
        }

      }
    }

/*    puts("");
    print_conflict_list(conflictList); */
    puts("");

    if (initial_numOfConflicts == 0) {
      initial_numOfConflicts = conflictList->counter;
      condition = 0.4 * initial_numOfConflicts;
    }

    /* if conflict list is not empty resolve conflicts and backpropagate */
    if (!isEmpty(conflictList)) {
      puts("\nconflict list not empty");
      resolve_conflicts(model, conflictList);
    }

    /* free memory allocated for this validation round */
    difference = abs(counter - conflictList->counter);
    counter = conflictList->counter;
    delete_hashtable(&hs);
    delete_list(conflictList);
    printf("number of conflicts = %d\n", counter);
  } while (counter > condition && difference > 5);  // continue resolving conflicts until the number of conflicts
  // drops to 40% of the initial number of conflicts or if [|new conflicts - previous conflicts|] > 5

  /* close temporary file of validation set and unlink it */
  fclose(fp);
  unlink("temporary_file");
}
