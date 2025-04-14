/* io.h
 * Solution IJC-DU2, task b)
 * Author: Adam Běhoun, FIT
 * Date: 17.4.2024
 * login: xbehoua00
 * Compiled: gcc (GCC) 10.5.0
*/

#ifndef IO_H__ // prevent multiple includes
#define IO_H__

// add needed includes 
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

int read_word(char *s, int max, FILE *f);

#endif // IO_H__