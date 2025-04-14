/* bitset.h
 * Řešení IJC-DU1, příklad a)
 * Autor: Adam Běhoun, FIT
 * Datum: 21.3.2024
 * login: xbehoua00
 * Přeloženo: gcc (GCC) 10.5.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "error.h"
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#define UL_BITS (sizeof(unsigned long) * CHAR_BIT) // number of bits in unsigned long

typedef unsigned long * bitset_t;
typedef unsigned long bitset_index_t;

/**
 * @brief macro creates an array of bits size. Macro calculate the number of needed elements
 * in the array and add one more to store size of the array in the first element. We also check if
 * the value of size is correct.
 *
 * @param jmeno_pole the name of array we process
 * @param velikost the size in bits
 */
#define bitset_create(jmeno_pole,velikost) \
    static_assert(velikost > 0, "Velikost pole musí být větší než 0."); \
    bitset_index_t jmeno_pole[velikost/UL_BITS + (velikost%UL_BITS != 0) + 1] = {velikost}; \

/**
 * @brief macro dynamically allocates memory for array. Macro calculates the number of needed
 * elements in the array and add one more to store size of the array in the first element.
 * We also check if the value of size is correct
 *
 * @param jmeno_pole the name of array we process
 * @param velikost the size in bits to allocate
 */
#define bitset_alloc(jmeno_pole,velikost) \
    static_assert(velikost > 0, "Velikost pole musí být větší než 0."); \
    bitset_t jmeno_pole = calloc(velikost/UL_BITS + (velikost%UL_BITS != 0) + 1, sizeof(bitset_index_t)); \
    if(jmeno_pole == NULL) {error_exit("bitset_alloc: Chyba alokace paměti\n");} \
    jmeno_pole[0] = velikost; \

#ifndef USE_INLINE
    /**
     * @brief macro frees allocated memory of the bitset
     * 
     * @param jmeno_pole the name of array we process
     */
    #define bitset_free(jmeno_pole) free(jmeno_pole);

    /**
     * @brief macro returns a size of array in bits
     * 
     * @param jmeno_pole the name of array we process
     */
    #define bitset_size(jmeno_pole) jmeno_pole[0]

    /**
     * @brief macro fills bitset with either 0 or 1. We create a bit mask with all zeros or ones (~0) and apply
     * OR operation or AND operation depending on bool_vyraz with each element of the bitset
     * 
     * @param jmeno_pole the name of array we process
     * @param bool_vyraz 1 to fill array with ones or 0 to fill array with zeros
     */
    #define bitset_fill(jmeno_pole,bool_vyraz) do{ \
        bitset_index_t mask = bool_vyraz ? ~0 : 0; \
        for(bitset_index_t i = 1; i <= bitset_size(jmeno_pole)/UL_BITS + (bitset_size(jmeno_pole) % UL_BITS != 0); i++) {\
            jmeno_pole[i] = bool_vyraz ? (jmeno_pole[i] | mask) : (jmeno_pole[i] & mask);\
        }\
    } while(0);

    /**
     * @brief macro sets a bit to 0 or 1 at specific location. We create a bit mask with 1 at the MSB and set other
     * bits to 0 applying shift left operation (or 0 at the MSB and set other bits to 1 applying NOT operation),
     * depending on the value of bool_vyraz, we apply AND with 0 at the MSB (to set bit to 0 and keep value of others)
     * or OR with 1 at the MSB (to set bit to 1 and keep value of others)
     *
     * @param jmeno_pole tha name of array we process
     * @param index location to process the bit
     * @param bool_vyraz 1 or 0
     */
    #define bitset_setbit(jmeno_pole,index,bool_vyraz) \
        if(index >= bitset_size(jmeno_pole)) \
            error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu", (unsigned long)index, (unsigned long)bitset_size(jmeno_pole) - 1); \
        if(bool_vyraz) \
            jmeno_pole[(index/UL_BITS)+1] |= (1UL << (index % UL_BITS)); \
        else \
            jmeno_pole[(index/UL_BITS)+1] &= ~(1UL << (index % UL_BITS));

    /**
     * @brief macro returns value of bit at specific location. We create a bit mask with 1 at the MSB and set others
     * bits to 0 applying shift left operation, then we apply AND operation with the bit at the index location,
     * then we shift it back and the expression is equal 1 (if the bit at index was 1) or 0 (if the bit at index was 0)
     *
     * @param jmeno_pole the name of array we process
     * @param index location to process the bit
     */
    #define bitset_getbit(jmeno_pole,index) \
        (index > jmeno_pole[0]) ? \
        (error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu", (unsigned long)index, (unsigned long)jmeno_pole[0] - 1), 2) : \
        ((jmeno_pole[(index/UL_BITS)+1] & (1UL << (index%UL_BITS))) >> (index%UL_BITS)) \

#else
    // ----------------- IMPLEMENTATION OF INLINE FUNCTIONS INSPIRED BY MACROS -----------------

    inline void bitset_free(bitset_t jmeno_pole) {
        free(jmeno_pole);
    }

    inline bitset_index_t bitset_size(bitset_t jmeno_pole) {
        return jmeno_pole[0];
    }

    inline void bitset_fill(bitset_t jmeno_pole, int bool_vyraz) {
        bitset_index_t mask = bool_vyraz ? ~0 : 0;
        for(bitset_index_t i = 1; i <= bitset_size(jmeno_pole)/UL_BITS + (bitset_size(jmeno_pole) % UL_BITS != 0); i++) {
            jmeno_pole[i] = bool_vyraz ? (jmeno_pole[i] | mask) : (jmeno_pole[i] & mask);
        }
    }

    inline void bitset_setbit(bitset_t jmeno_pole, bitset_index_t index, int bool_vyraz) {
        if(index > bitset_size(jmeno_pole))
            error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu", (unsigned long)index, (unsigned long)bitset_size(jmeno_pole)-1);
        if(bool_vyraz)
            jmeno_pole[(index/UL_BITS)+1] |= (1UL << (index % UL_BITS));
        else
            jmeno_pole[(index/UL_BITS)+1] &= ~(1UL << (index % UL_BITS));
    }

    inline int bitset_getbit(bitset_t jmeno_pole, bitset_index_t index) {
        if(index > bitset_size(jmeno_pole))
            error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu", (unsigned long)index, (unsigned long)bitset_size(jmeno_pole)-1);
        return ((jmeno_pole[(index/UL_BITS)+1] & (1UL << (index%UL_BITS))) >> (index%UL_BITS));
    }

#endif
