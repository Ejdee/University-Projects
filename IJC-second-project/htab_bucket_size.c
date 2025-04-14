/* htab_bucket_size.c
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/

#include "htab_struct.h"

/**
 * @brief returns the number of buckets in hash table
 * 
 * @param t hash table
 * @return size_t number of buckets
 */
size_t htab_bucket_count(const htab_t * t) {
    return t->arr_size;
}