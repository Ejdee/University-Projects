#include <stdio.h>

// This is a single-line comment

#define KOTESENI /* Block comment in the middle of macro definition */ 42

/*
        Multi
                line
    block

            comment
*/

char *comment = "/* This is not block comment /*";
char *string = "//This is not a line comments";

int main() {
    // This is another single-line comment
    printf("Hello, world!\n"); //the line below should be treated as a comment \
    char d = 'd';
    return 0;
}

/*
aha
*/

// This \
comment \
that \
continues\
on \
different \
lines

char d = '\\';
char e = '\'';

char *string2 = "/***/";
char *string3 = "text\"//text\'\"\\";

// This comment contains escape sequences: \n, \t, \", \

// This comment is very long and exceeds the recommended line length of 80 characters. It is used to demonstrate how to break long comments into multiple lines for better readability. This is especially useful when providing detailed explanations or documenting complex code.

