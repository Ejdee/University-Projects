/**
 * Implementation author : Adam Běhoun (xbehoua00@fit.vutbr.cz)
 */

/*
 * Tabulka s rozptýlenými položkami
 *
 * S využitím datových typů ze souboru hashtable.h a připravených koster
 * funkcí implementujte tabulku s rozptýlenými položkami s explicitně
 * zretězenými synonymy.
 *
 * Při implementaci uvažujte velikost tabulky HT_SIZE.
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptylovací funkce která přidělí zadanému klíči index z intervalu
 * <0,HT_SIZE-1>. Ideální rozptylovací funkce by měla rozprostírat klíče
 * rovnoměrně po všech indexech. Zamyslete sa nad kvalitou zvolené funkce.
 */
int get_hash(char *key) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) {
    result += key[i];
  }
  return (result % HT_SIZE);
}

/*
 * Inicializace tabulky — zavolá sa před prvním použitím tabulky.
 */
void ht_init(ht_table_t *table) {
  // set each element to NULL
  for(int i = 0; i < HT_SIZE; i++) {
    (*table)[i] = NULL;
  }
}

/*
 * Vyhledání prvku v tabulce.
 *
 * V případě úspěchu vrací ukazatel na nalezený prvek; v opačném případě vrací
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key) {

  int hash = get_hash(key);
  ht_item_t *tmp_element = (*table)[hash];

  // traverse the linked list, until we find the correct key
  while(tmp_element != NULL) {
    if(strcmp(tmp_element->key, key) == 0) {
      return tmp_element;
    } else {
      tmp_element = tmp_element->next;
    }
  }

  // return NULL if we haven't found the key
  return NULL;
}

/*
 * Vložení nového prvku do tabulky.
 *
 * Pokud prvek s daným klíčem už v tabulce existuje, nahraďte jeho hodnotu.
 *
 * Při implementaci využijte funkci ht_search. Pri vkládání prvku do seznamu
 * synonym zvolte nejefektivnější možnost a vložte prvek na začátek seznamu.
 */
void ht_insert(ht_table_t *table, char *key, float value) {
  int hash = get_hash(key);
  ht_item_t *searched_element = ht_search(table, key);

  // if there is no element with the key, we allocate a new item with the key,
  // and insert it at the beginning of the list 
  // otherwise we rewrite its value
  if(searched_element == NULL) {

    // allocate memory for the new item
    ht_item_t *new = malloc(sizeof(ht_item_t));
    if(new == NULL) {
      return;
    }

    // allocate memory for the key and copy the key 
    new->key = strdup(key);
    if(new->key == NULL) {
      return;
    }

    new->value = value;
    // set the next element to the first element of the list
    new->next = (*table)[hash];

    // set the first element of the list to the new element
    (*table)[hash] = new;
  } else {
    searched_element->value = value;
  }
}

/*
 * Získání hodnoty z tabulky.
 *
 * V případě úspěchu vrací funkce ukazatel na hodnotu prvku, v opačném
 * případě hodnotu NULL.
 *
 * Při implementaci využijte funkci ht_search.
 */
float *ht_get(ht_table_t *table, char *key) {
  ht_item_t *searched = ht_search(table, key);
  // if we have found an element, return the pointer to the value
  // otherwise return NULL
  if(searched != NULL) {
    return (&searched->value);
  } 
  return NULL;
}

/*
 * Smazání prvku z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje přiřazené k danému prvku.
 * Pokud prvek neexistuje, funkce nedělá nic.
 *
 * Při implementaci NEPOUŽÍVEJTE funkci ht_search.
 */
void ht_delete(ht_table_t *table, char *key) {
  int hash = get_hash(key);

  // take track of two elements, current and one before current
  ht_item_t *prev = NULL;
  ht_item_t *curr = (*table)[hash];
  // traverse the linked list until we find the key to delete
  while(curr != NULL) {
    // if we found the key to delete, then set the previous element to point
    // to the next element of the element being deleted and free the current element
    if(strcmp(curr->key, key) == 0) {
      if(prev != NULL) {
        prev->next = curr->next;
      }
      free(curr->key);
      free(curr);
      return;
    }
    prev = curr;
    curr = curr->next;
  }
  return;
}

/*
 * Smazání všech prvků z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje a uvede tabulku do stavu po 
 * inicializaci.
 */
void ht_delete_all(ht_table_t *table) {
  // traverse the linked list for each item in the table and free it
  for(int i = 0; i < HT_SIZE; i++) {
    ht_item_t *curr = (*table)[i];
    ht_item_t *tmp = NULL;

    while(curr != NULL) {
      tmp = curr;
      curr = curr->next;
      free(tmp->key);
      free(tmp);
    }

    (*table)[i] = NULL;
  }
}
