/* htab_statistics.c
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/

#include <stdio.h>
#include "htab_struct.h"

/**
 * @brief print out statistics of the hash table in the stderr output.
 *  - min: the minimum number of records in any bucket of the hash table
 *  - max: the maximum number of records in any bucket of the hash table
 *  - avg: the average number of records of all the bucketsof the hash table
 * 
 * @param t hash table
 */
void htab_statistics(const htab_t * t) {
    int min = 0;
    int max = 0;

    for(int i = 0; i < t->arr_size; i++) {
        htab_item_t *temp = t->ptr[i];
        int length = 0;
        while(temp != NULL) {
            length ++;
            temp = temp->next;
        }
        max = max > length ? max : length; // update the maximum number

        if(i == 0) { // set the first number to minimum
            min = length;
        } else {
            min = min > length ? length : min; // update the minimum number if the current is less
        }
    }


    // calculate average number of records as float number
    // we have size and arr_size as integers, so we need to convert int to float
    float avg = (float)t->size/(float)t->arr_size;
    fprintf(stderr, "min: %d\n", min);
    fprintf(stderr, "max: %d\n", max);
    fprintf(stderr, "avg: %.2f\n", avg);
}