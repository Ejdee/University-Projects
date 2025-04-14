/* no-comment.c
 * Řešení IJC-DU1, příklad b)
 * Autor: Adam Běhoun, FIT
 * Datum: 15.3.2024
 * login: xbehoua00
 * Přeloženo: gcc (GCC) 10.5.0
*/

// we need to define posix to use fileno function
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include "error.h"
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    // check if file is passed as command line argument, if not, we read from stdin
    FILE *fp = NULL;
    if(argc < 2) {
        fp = stdin;
    } else {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            error_exit("Soubor %s nelze otevřít.\n", argv[1]);
        }
        // check if the output is not redirected to the input file using fstat function
        int file_fd = fileno(fp);
        int stdout_fd = fileno(stdout);

        struct stat file_stat, stdout_stat;

        if (fstat(file_fd, &file_stat) == -1 || fstat(stdout_fd, &stdout_stat) == -1)
            error_exit("Nepodařilo se zjistit informace o souboru.");

        if (file_stat.st_dev == stdout_stat.st_dev && file_stat.st_ino == stdout_stat.st_ino)
            error_exit("Výstup přesměrován do vstupního souboru - nedefinované chování.");
    }

    int c = 0;
    int state = 0; // set default state to 0
    while((c=getc(fp)) != EOF) {
        // implementation of finite state machine
        switch(state) {
            case 0:
                if(c == '/') {
                    state = 1;
                } else if (c == '"') {
                    state = 5;
                    putchar(c);
                } else if (c == '\'') {
                    state = 8;
                    putchar(c);
                } else {
                    putchar(c);
                }
                break;
            case 1:
                if (c == '/') {
                    state = 4;
                } else if (c == '*') {
                    state = 2;
                } else {
                    putchar('/');
                    putchar(c);
                    state = 0;
                }
                break;
            case 2:
                if (c == '*')
                    state = 3;
                break;
            case 3:
                if(c == '/') {
                    putchar(' ');
                    state = 0;
                } else if (c != '*') {
                    state = 2;
                }
                break;
            case 4:
                if (c == '\\') {
                    state = 7;
                } else if (c == '\n') {
                    putchar(c);
                    state = 0;
                }
                break;
            case 5:
                if (c == '"') {
                    putchar(c);
                    state = 0;
                } else if (c == '\\') {
                    putchar(c);
                    state = 6;
                } else {
                    putchar(c);
                }
                break;
            case 6:
                putchar(c);
                state = 5;
                break;
            case 7:
                state = 4;
                break;
            case 8:
                if (c == '\\') {
                    putchar(c);
                    state = 9;
                } else if (c == '\'') {
                    putchar(c);
                    state = 0;
                } else {
                    putchar(c);
                }
                break;
            case 9:
                putchar(c);
                state = 8;
                break;
        } // end switch
    } // end while

    if(state != 0) {
        error_exit("Aktuální stav je %d, něco se pokazilo.", state);
    }

    if(argc > 1)
        fclose(fp);

    return 0;
}
