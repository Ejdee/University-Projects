/* error.c
 * Řešení IJC-DU1, příklad b)
 * Autor: Adam Běhoun, FIT
 * Datum: 15.3.2024
 * login: xbehoua00
 * Přeloženo: gcc (GCC) 10.5.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "error.h"

/**
 * @brief function prints the warning message with variable number of arguments
 * @param fmt format of the warning
 * @param ... arguments
 */
void warning(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Warning: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);
}

/**
 * @brief function prints error message with variable number of arguments and exit the program
 * @param fmt format of the error
 * @param ... arguments
 */
void error_exit(const char *fmt, ...) {
    va_list args;
    va_start (args, fmt);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);
    exit(1);
}
