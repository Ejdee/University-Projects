/* htab_find.c
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/

#include "htab.h"
#include "htab_struct.h"
#include "htab_item.h"

/**
 * @brief finds the record in hash table by its key and returns pointer to the record
 * 
 * @param t hash table
 * @param key key of the record
 * @return htab_pair_t* pointer to the records that contains the key
 * @return NULL if the key was not found
 */
htab_pair_t * htab_find(const htab_t * t, htab_key_t key) {

    // calculate the position using hash function and modulo
    size_t position = htab_hash_function(key) % t->arr_size;

    htab_item_t *temp = t->ptr[position];
    // traverse the linked list in the calculated position
    while(temp != NULL) {
        if(strcmp(temp->pair.key, key) == 0) {
            return &temp->pair; // return the pointer 
        } else {
            temp = temp->next;
        }
    }

    // if the key was not found, return NULL
    return NULL;
}

