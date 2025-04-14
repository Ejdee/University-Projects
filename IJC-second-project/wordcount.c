/* wordcount.c
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/


#include <stdio.h>
#include <stdlib.h>
#include "htab.h"
#include "io.h"

// The hash table size should be a prime number to reduce collisions. 
// Since we don't know the exact size of the input, we have to choose a size that
// will be compromise between memory usage and efficiency. After testing inputs with
// one million unique words and small input with ~100 words, table size of 30089 worked
// well.
#define TABLE_SIZE 30089

// maximum length of read word
#define MAX_LENGTH_WORD 256


/**
 * @brief prints out pair of hash table in format "[key]    [value]"
 * 
 * @param data pair 
 */
void print_pair(htab_pair_t *data) {
    printf("%s\t%d\n", data->key, data->value);
}

int main() {
    htab_t *table = htab_init(TABLE_SIZE);

    bool warning = false;
    char string[MAX_LENGTH_WORD];
    int length = 0;

    while((length = read_word(string, MAX_LENGTH_WORD, stdin)) != EOF) {

        // if a word is longer that MAX_LENGTH_WORD and a warning has not beed printed out yet,
        // we print it out and set flag so we dont print it out anymore.
        if(length > MAX_LENGTH_WORD-1 && !warning) {
            fprintf(stderr, "A word is longer than 255 characters, cutting.\n");
            warning = true;
        }

        // add or increment value of record in the hash table
        htab_pair_t *new_item = htab_lookup_add(table, string);
        (void)new_item;
        if(new_item == NULL) {
            fprintf(stderr, "Error: new_item allocation.\n");
            htab_free(table);
            return 1;
        }
    }

    // print out each pair
    htab_for_each(table, &print_pair);

    // if the program was compiled with -DSTATISTICS, we print out statistics
    #ifdef STATISTICS
        htab_statistics(table);
    #endif

    // free hash table
    htab_free(table);
    table=NULL;

    return 0;
} //main
