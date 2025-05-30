/* htab_size.c
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/

#include "htab_struct.h"

/**
 * @brief returns the current number of records in the hash table
 * 
 * @param t hash table
 * @return size_t number of the records in the hash table
 */
size_t htab_size(const htab_t * t) {
    return t->size;
}