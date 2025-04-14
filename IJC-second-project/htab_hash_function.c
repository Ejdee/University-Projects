/* htab_free.c
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/

#include <stdint.h>
#include "htab.h"

/**
 * @brief calculated the hash of the given string
 * 
 * @param str string
 * @return size_t hash
 */
size_t htab_hash_function(const char *str) {
    uint32_t h=0;     // has to be 32 bits
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h;
}