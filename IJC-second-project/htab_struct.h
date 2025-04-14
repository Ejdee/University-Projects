/* htab_struct.h
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/

#ifndef HTAB_STRUCT_H // prevent multiple includes
#define HTAB_STRUCT_H

#include "htab.h"
#include "htab_item.h"

struct htab {
    int size;
    int arr_size;
    htab_item_t *ptr[];
};

#endif // htab_struct.h