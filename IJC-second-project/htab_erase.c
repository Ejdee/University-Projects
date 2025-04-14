/* htab_erase.c
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/

#include <stdlib.h>
#include "htab_struct.h"

/**
 * @brief erase record in hash table by given key
 * 
 * @param t hash table
 * @param key key of record
 * @return true if erase was successful
 * @return false if something went wrong
 */
bool htab_erase(htab_t * t, htab_key_t key) {
    size_t position = htab_hash_function(key) % t->arr_size;
    
    htab_item_t *temp = t->ptr[position];
    htab_item_t *prev = NULL;
    while(temp != NULL) {
        if(strcmp(temp->pair.key, key) == 0) {
            if(prev == NULL) { // if the key is on the first position
                t->ptr[position] = temp->next; // update the first item of the list
            } else {
                prev->next = temp->next; // else, reconnect the linked list after free
            }
            temp->next = NULL;
            
            free((char*)temp->pair.key); // free allocated char for new key
            free(temp);

            t->size --; // decrement the number of records in hash table
            return true;
        } else {
            prev = temp; // update pointers
            temp = temp->next;
        }
    }
    return false;
}