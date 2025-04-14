/* primes.c
 * Řešení IJC-DU1, příklad a)
 * Autor: Adam Běhoun, FIT
 * Datum: 22.3.2024
 * login: xbehoua00
 * Přeloženo: gcc (GCC) 10.5.0
*/

#include "eratosthenes.h"
#include <stdio.h>
#include <time.h>

#define size 666000001 // size of the bitset
#define primes_count 10 // number of prime numbers we want to print

/**
 * @brief functions prints the last 10 elements that are set to 1 in the bitset
 * @param array the name of the bitset
 */
void print_primes(bitset_t array) {
    int count = 0;
    bitset_index_t numbers[primes_count];
    bitset_index_t arr_size = bitset_size(array);
    for(bitset_index_t i = arr_size-1; i > 0 && count < primes_count; i--) {
        if(bitset_getbit(array, i) == 1) {
            numbers[count] = i;
            count++;
        }
    }
    for(int i = primes_count-1; i >= 0; i--) {
        printf("%lu\n", (unsigned long)numbers[i]);
    }
}

int main () {
    clock_t start = clock();

    bitset_create(array, size);
    Eratosthenes(array);
    print_primes(array);

    // print the runtime of the program
    fprintf(stderr, "Time=%.3g\n", (double)(clock()-start)/CLOCKS_PER_SEC);

    return 0;
}
