/* htab_free.c
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/

#include <stdlib.h>
#include "htab_struct.h"

/**
 * @brief frees the entire hash table structure with calling htab_clear
 * 
 * @param t hash table
 */
void htab_free(htab_t * t) {
    htab_clear(t);
    free(t);
}