/* htab_lookup_add
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/


#include <stdio.h>
#include <stdlib.h>
#include "htab_struct.h"

/**
 * @brief Search for record with key, and if it is found then returns pointer to it.
 * If it is not found, create a new record with the key, and connect it to the hash table.
 * 
 * @param t hash table
 * @param key key of the record
 * @return htab_pair_t* pointer to the record
 * @return NULL if something went wrong
 */
htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key) {
    
    // calculate the position using hash function and modulo
    size_t position = htab_hash_function(key) % t->arr_size;

    htab_item_t *temp = t->ptr[position];
    htab_item_t *previous = NULL;
    while(temp != NULL) {
        if(strcmp(temp->pair.key, key) == 0) {
            temp->pair.value ++;
            return &temp->pair;
        } else {
            previous = temp; // store the current item
            temp = temp->next; // go to next item in table
        }
    }

    htab_item_t *new_item = malloc(sizeof(htab_item_t));
    if(new_item == NULL) {
        fprintf(stderr, "Allocation of new item was not succesfull.\n");
        return NULL;
    }

    // create new variable so hash table does not store one and the same as we pass more keys
    char *new_key = malloc((strlen(key)+1) * sizeof(char));
    if(new_key == NULL) {
        fprintf(stderr, "Allocation of new item was not succesfull.\n");
        free(new_item);
        return NULL;
    }
    strcpy(new_key, key);
    new_item->pair.key = new_key;

    new_item->pair.value = 1; // set the value to 1
    new_item->next = NULL; // set the next pointer to NULL


    if(previous == NULL) {
        // if it is first item on the position, make it a head in the list
        t->ptr[position] = new_item;
    } else {
        previous->next = new_item; // set the previous item to point to new item
    }

    t->size ++; //increment the number of records
    return &new_item->pair;
} // htab_lookup_add