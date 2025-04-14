#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LINE_SIZE 2048
#define DEFAULT_N_SIZE 10

/**
 * @brief cyclic buffer structure, that stores N elements of lines, read index, write index,
 * and size of buffer
 *
 */
typedef struct tcbuf {
    char (*lines)[MAX_LINE_SIZE];
    int windex;
    int rindex;
    int size;
    int count;
} cbuf;


/**
 * @brief check if the passed parameter is only digits
 * 
 * @param parameter string to check
 * @return true if it is only digits
 * @return false if some characters is not digit
 */
bool n_valid(char* parameter) {
    for(size_t i = 0; i < strlen(parameter); i++) {
        if(!isdigit(parameter[i])) {
            return false;
        }
    } 
    return true;
}

/**
 * @brief handle passed arguments of the program and return the number of
 * lines, that user wants to print out. If the -n option was specified, we
 * calculate the new number, otherwise we return default size of 10.
 * 
 * @param argc number of arguments
 * @param argv array of arguments
 * @param fp stream
 * @return int number of lines to print out
 */
int parser(int argc, char *argv[], FILE **fp) {
    int size = -1;

    if(argc > 4) {
        fprintf(stderr, "Too many arguments");
        exit(1);
    }

    for(int i = 1; i < argc; i++) {
        if(strcmp("-n", argv[i]) == 0) {
            if(i+1 < argc) { // check if i+1 argc exists
                if(n_valid(argv[i+1])) {
                    size = strtol(argv[i+1], NULL, 10); // convert the string to number and store it in size
                } else {
                    fprintf(stderr, "Invalid argument for -n option.\n");
                    exit(1);
                }

                i++; // increment the i since we processed it 
            } else {
                fprintf(stderr, "Option -n requires argument.\n");
                exit(1);
            }
            
        } else {
            if(*fp != stdin) { // if we are supposed to change file name, but it is not stdin.. it was already changed and we throw error
                fprintf(stderr, "You cannot type in two files.\n");
                exit(2);
            }
            *fp = fopen(argv[i], "r"); // open the file
            if(*fp == NULL) { // check for success
                fprintf(stderr, "Failed to open file %s.\n", argv[i]);
                exit(2);
            }
        }
    }

    return (size > -1 ? size : DEFAULT_N_SIZE);
}

/**
 * @brief function checks if buffer of cbuf structure is empty or not
 * 
 * @param cb cyclic buffer structure
 * @return true if write index is 0
 * @return false if write index is not 0
 */
bool cbuf_empty(cbuf *cb) {
    return cb->count == 0; // if count of lines in the buffer is zero, then the buffer is empty
}

/**
 * @brief function creates and allocates space for cbuf and return pointer to it
 * 
 * @param n number of lines, we want to track
 * @return cbuf* created cbuf structure
 */
cbuf* cbuf_create(int n) {
    cbuf *cb = malloc(sizeof(cbuf)); // allocate memory for structure

    if (cb != NULL) {
        cb->lines = malloc(n * sizeof(char[MAX_LINE_SIZE])); // allocate memory for each line depending on variable 'n'
        if(cb->lines == NULL) {
            free(cb);
            cb = NULL;
        } else {
            cb->count = 0;
            cb->rindex = 0;
            cb->windex = 0;
            cb->size = n;
        }
    } else {
        fprintf(stderr, "Failed to allocate cbuf structure.\n");
        exit(1);
    }

    return cb;
}

/**
 * @brief function puts read line to position where write_index is pointing
 * 
 * @param cb cyclic buffer structure
 * @param line read line
 */
void cbuf_put(cbuf *cb, char *line) {

    // copy the line in the buffer at the current write index. to ensure that the line does not exceed the maximum length of buffer size,
    // we copy at most MAX_LINE_SIZE - 1 characters, and we add null terminator character at the end.
    strncpy(cb->lines[cb->windex], line, MAX_LINE_SIZE-1);
    cb->lines[cb->windex][MAX_LINE_SIZE-1] = '\0';

    // if the buffer is not empty and write index is equal to read index, we want to increment read index too
    if(!cbuf_empty(cb) && cb->windex == cb->rindex)
        cb->rindex = (cb->rindex + 1) % cb->size;

    cb->windex = (cb->windex + 1) % cb->size; // increment write index
    cb->count ++; // increment number of lines in the buffer
}

/**
 * @brief function gets a line written in read_index position and returns it
 * 
 * @param cb cyclic buffer structure
 * @return char* line that is stored in read_index position
 */
char* cbuf_get(cbuf *cb) {
    if(cbuf_empty(cb)){
        fprintf(stderr, "Buffer is empty.\n");
        exit(1);
    }

    char *to_return = cb->lines[cb->rindex]; // store line at the read index in variable
    cb->rindex = (cb->rindex + 1) % cb->size; // increment read index
    cb->count --; // decrement number of lines in the buffer

    return to_return;
}

/**
 * @brief function frees allocated cyclic buffer structure
 * 
 * @param cb cyclic buffer structure
 */
void cbuf_free(cbuf* cb) {
    free(cb->lines);
    free(cb);
}

/**
 * @brief load lines in the cyclic buffer
 * 
 * @param dest buffer
 * @param fp stream
 * @param cb cyclic buffer
 */
void load_lines(char* dest, FILE *fp, cbuf *cb) {
    // reading lines to dest
    while(fgets(dest, MAX_LINE_SIZE, fp) != NULL) {

        bool newline_read = false;
        // checking, if the loaded line contains newline character
        for(size_t i = 0; i < strlen(dest); i++) {
            if(dest[i] == '\n') {
                newline_read = true;
                break;
            } 
        }

        bool warning = false;
        if(!newline_read) {
            // if the new line character is not in the string, we will insert it before the EOF
            //dest[MAX_LINE_SIZE-2] = '\n';

            if(!warning) {
                fprintf(stderr, "Some lines are too long, it is being cut to 2047 characters.\n");
                warning = true;
            }

            // throw away all the characters that exceeded the length limit
            int c = 0;
            while((c = getc(fp)) != '\n') {
                if(c == EOF) {
                    break;
                }
            }
        }

        cbuf_put(cb, dest);
    }
}


int main(int argc, char *argv[]) {
    FILE *fp = stdin; // default stream

    int size = parser(argc, argv, &fp); // wee need to pass pointer to fp

    if(size == 0) { // if number of lines to print out is 0, end the program with return code 0
        if(fp!=stdin){
            fclose(fp);
        }
        return 0;
    }

    cbuf* cb = cbuf_create(size); // create the buffer

    // load the lines from file or stdin to buffer
    char line[MAX_LINE_SIZE];
    load_lines(line, fp, cb);

    // if 'size' is less than the number of lines in the buffer, print 'size' lines. otherwise print the maximum number of lines.
    int cb_lines_count = size < cb->count ? size : cb->count;

    // print out the lines
    for(int i = 0; i < cb_lines_count; i++) {
        printf("%s", cbuf_get(cb));
    }

    // free the structure
    cbuf_free(cb);

    // if the file is opened, close it
    if(fp != stdin)
        fclose(fp);

    return 0;
}
