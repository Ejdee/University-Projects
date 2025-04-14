/* eratosthenes.c
 * Řešení IJC-DU1, příklad a)
 * Autor: Adam Běhoun, FIT
 * Datum: 21.3.2024
 * login: xbehoua00
 * Přeloženo: gcc (GCC) 10.5.0
*/

#include <math.h>
#include "eratosthenes.h"

// if it is compiled with -DUSE_INLINE flag, we want to use inline functions
#ifdef USE_INLINE
extern void bitset_free(bitset_t jmeno_pole);
extern bitset_index_t bitset_size(bitset_t jmeno_pole);
extern void bitset_fill(bitset_t jmeno_pole, int bool_vyraz);
extern void bitset_setbit(bitset_t jmeno_pole, bitset_index_t index, int bool_vyraz);
extern int bitset_getbit(bitset_t jmeno_pole, bitset_index_t index);
#endif

/**
 * @brief function finds all prime numbers to N using Eratosthenes sieve
 * @param pole name of the bitset
 */
void Eratosthenes(bitset_t pole) {
    bitset_fill(pole, 1);
    bitset_setbit(pole, 0, 0);
    bitset_setbit(pole, 1, 0);

    bitset_index_t size = bitset_size(pole);

    // multiples of 2 are not prime numbers, so we can set them to 0 in advance
    for(bitset_index_t i = 4; i < size; i+=2) {
        bitset_setbit(pole, i, 0);
    }

    // we start loop from 3 and jump by 2, so we skip all even numbers
    for(bitset_index_t i = 3; i < sqrt(size); i+=2) {
        if(bitset_getbit(pole, i)) {
            // we set all multiples of i to 0
            for(bitset_index_t j = i*i; j < size; j+=2*i) {
                bitset_setbit(pole, j, 0);
            }
        }
    }
}
