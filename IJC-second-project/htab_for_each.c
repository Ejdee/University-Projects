/* htab_for_each.c
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/

#include "htab_struct.h"

/**
 * @brief Applies a given function to all the records in the hash table
 * 
 * @param t hash table
 * @param f pointer to a function with parameter htab_pair_t
 */
void htab_for_each(const htab_t *t, void (*f)(htab_pair_t *data)) {
    // traverse the entire hash table
    for(int i = 0; i < t->arr_size; i++) {
        htab_item_t *temp = t->ptr[i];
        while(temp != NULL) {
            // create new pair, so the function will not change the original hash table
            htab_pair_t temp_pair = temp->pair;

            f(&temp_pair); // apply the function on the pair
            temp = temp->next;
        }
    }
}