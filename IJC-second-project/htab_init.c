/* htab_init.c
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/


#include <stdio.h>
#include <stdlib.h>

#include "htab_struct.h"
#include "htab_item.h"

/**
 * @brief initialize hash table of size n
 * 
 * @param n number of buckets
 * @return htab_t* pointer to the initialized hash table
 */
htab_t *htab_init(const size_t n) {    
    htab_t *table = malloc(sizeof(htab_t) + n * sizeof(htab_item_t*));
    if(table == NULL) { // check of successful allocation
        fprintf(stderr, "Allocation was not successful.\n");
        return NULL;
    }

    table->size = 0;
    table->arr_size = n;
    
    for(size_t i = 0; i < n; i++) {
        table->ptr[i] = NULL;
    } 

    return table;
}