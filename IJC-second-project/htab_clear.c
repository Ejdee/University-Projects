/* htab_clear.c
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/

#include <stdlib.h>
#include "htab_struct.h"

/**
 * @brief clears all the records in hash table
 * 
 * @param t hash table
 */
void htab_clear(htab_t * t) {

    for(int i = 0; i < t->arr_size; i++) {
        htab_item_t *temp = t->ptr[i];
        // we need to track next item, so we can fully free the current item and not lose next pointer
        htab_item_t *next_item = NULL;

        // traverse the linked list and free when we see not-NULL item
        while(temp != NULL) {
            next_item = temp->next;
            free((char*)temp->pair.key); // free the allocated key
            free(temp);
            temp = next_item;
        }
        t->ptr[i]=NULL;
    }
    t->size = 0; // set number of records to 0
    
}