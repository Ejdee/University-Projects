/* io.c
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/


#include "io.h"

/**
 * @brief read a single word and return the number of successfuly read characters or EOF
 * 
 * @param s buffer to store the word
 * @param max maximum number of characters that can be read
 * @param f stream
 * @return int number of successfuly read characters or EOF if it is end of the file
 */
int read_word(char *s, int max, FILE *f) {
    int c = getc(f);

    while(isspace(c) && c != EOF) {
        c = getc(f); // skip the isspace characters
    }

    if(c == EOF) {
        return -1;
    }

    // read the word
    int position = 0;
    while(!isspace(c) && c != EOF && position < max-1) {
        s[position] = c;
        position++;
        c = getc(f);
    } 

    s[position] = '\0'; // add the null terminator character

    while(!isspace(c) && c != EOF) {
        c = getc(f); // skip the remaining characters
        position ++; // increment the position so we can detect that we cut the word
    }

    return position;
} // read_word