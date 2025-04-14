/**
 * Implementation author : Adam Běhoun (xbehoua00@fit.vutbr.cz)
 */

/*
 * Použití binárních vyhledávacích stromů.
 *
 * S využitím Vámi implementovaného binárního vyhledávacího stromu (soubory ../iter/btree.c a ../rec/btree.c)
 * implementujte triviální funkci letter_count. Všimněte si, že výstupní strom může být značně degradovaný 
 * (až na úroveň lineárního seznamu). Jako typ hodnoty v uzlu stromu využijte 'INTEGER'.
 * 
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>


/**
 * Vypočítání frekvence výskytů znaků ve vstupním řetězci.
 * 
 * Funkce inicilializuje strom a následně zjistí počet výskytů znaků a-z (case insensitive), znaku 
 * mezery ' ', a ostatních znaků (ve stromu reprezentováno znakem podtržítka '_'). Výstup je 
 * uložen ve stromu (klíč vždy lowercase).
 * 
 * Například pro vstupní řetězec: "abBccc_ 123 *" bude strom po běhu funkce obsahovat:
 * 
 * key | value
 * 'a'     1
 * 'b'     2
 * 'c'     3
 * ' '     2
 * '_'     5
 * 
 * Pro implementaci si můžete v tomto souboru nadefinovat vlastní pomocné funkce.
*/

/**
 * @brief Convert the uppercase A-Z to lowercase by adding 32 to it. According to ASCII table.
 * If the character is not a-z || A-Z || ' ', we return '_'
 * 
 * @param c character
 * @return lowercased character
 */
char to_lower_or_underscore(char c) {
    if(c >= 65 && c <= 90) {
        return c+32;
    } else if (c == ' ' || (c >= 97 && c <= 122)) {
        return c;
    } else {
        return '_';
    }
}

void letter_count(bst_node_t **tree, char *input) {

    // for every character in the input string
    for(int i = 0; input[i] != '\0'; i++) {

        // create a pointer to store the result of the function bst_search
        bst_node_content_t *searched_content = NULL;
        // if the node with given key was found, we will increment the value of the node
        if(bst_search(*tree, to_lower_or_underscore(input[i]), &searched_content)) {
            *(int*)searched_content->value += 1;
        // if the node with given key doesn't exist, we will create a new one and set
        // the initial value to 1
        } else {
            bst_node_content_t new_content = {
                .type = INTEGER,
                .value = malloc(sizeof(int))
            };
            if(new_content.value == NULL) {
                fprintf(stderr, "Memory allocation error.\n");
                return;
            }
            *((int*)(new_content.value)) = 1;
            bst_insert(tree, to_lower_or_underscore(input[i]), new_content);
        }
    }
}
