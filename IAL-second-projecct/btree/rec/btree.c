/**
 * Implementation author : Adam Běhoun (xbehoua00@fit.vutbr.cz)
 */

/*
 * Binární vyhledávací strom — rekurzivní varianta
 *
 * S využitím datových typů ze souboru btree.h a připravených koster funkcí
 * implementujte binární vyhledávací strom pomocí rekurze.
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializace stromu.
 *
 * Uživatel musí zajistit, že inicializace se nebude opakovaně volat nad
 * inicializovaným stromem. V opačném případě může dojít k úniku paměti (memory
 * leak). Protože neinicializovaný ukazatel má nedefinovanou hodnotu, není
 * možné toto detekovat ve funkci.
 */
void bst_init(bst_node_t **tree)
{
  *tree = NULL;
}

/*
 * Vyhledání uzlu v stromu.
 *
 * V případě úspěchu vrátí funkce hodnotu true a do proměnné value zapíše
 * ukazatel na obsah daného uzlu. V opačném případě funkce vrátí hodnotu false a proměnná
 * value zůstává nezměněná.
 *
 * Funkci implementujte rekurzivně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, bst_node_content_t **value)
{
  // if the tree node is NULL, we haven't found the given key
  if(tree == NULL) {
    return false;
  }

  // if we found the node with the key, set the value and return true
  if(tree->key == key) {
    *value = &tree->content;
    return true;
  }

  // if the key is larger, we want to check the right subtree next
  // otherwise the left one
  if(key > tree->key) {
    return bst_search(tree->right, key, value);
  } else {
    return bst_search(tree->left, key, value);
  }
}

/*
 * Vložení uzlu do stromu.
 *
 * Pokud uzel se zadaným klíče už ve stromu existuje, nahraďte jeho hodnotu.
 * Jinak vložte nový listový uzel.
 *
 * Výsledný strom musí splňovat podmínku vyhledávacího stromu — levý podstrom
 * uzlu obsahuje jenom menší klíče, pravý větší.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_insert(bst_node_t **tree, char key, bst_node_content_t value)
{
  // if we got to the NULL node, we want to insert
  if(*tree == NULL) {
    // allocate the memory
    *tree = malloc(sizeof(bst_node_t));
    if(tree == NULL) {
      fprintf(stderr, "Memory allocation error.\n");
      return;
    }
    // update the values
    (*tree)->content = value;
    (*tree)->key = key;
    (*tree)->left = NULL;
    (*tree)->right = NULL;
    return;
  }

  // if the key already exists, update the value and free the old one
  if((*tree)->key == key) {
    free((*tree)->content.value);
    (*tree)->content = value;
    return;
  }

  // if the key is larger than the key of the current node, 
  // call the function again on the right subtree
  // otherwise call it on the left subtree
  if(key > (*tree)->key) {
    bst_insert(&(*tree)->right, key, value);
  } else {
    bst_insert(&(*tree)->left, key, value);
  }
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 *
 * Klíč a hodnota uzlu target budou nahrazeny klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 *
 * Tato pomocná funkce bude využitá při implementaci funkce bst_delete.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree)
{
  // if we got to the rightmost node, update the target values,
  // and if the node being deleted has a left child, make it new root
  // of the subtree now or free the rightmost node and set it to NULL to get initial state
  if((*tree)->right == NULL) {
    free(target->content.value);
    target->content = (*tree)->content;
    target->key = (*tree)->key;
    if((*tree)->left != NULL) {
      bst_node_t *tmp = *tree;
      *tree = (*tree)->left;
      free(tmp);
    } else {
      free(*tree);
      *tree = NULL;
    }
    return;
  }

  bst_replace_by_rightmost(target, &(*tree)->right);
}

/*
 * Odstranění uzlu ze stromu.
 *
 * Pokud uzel se zadaným klíčem neexistuje, funkce nic nedělá.
 * Pokud má odstraněný uzel jeden podstrom, zdědí ho rodič odstraněného uzlu.
 * Pokud má odstraněný uzel oba podstromy, je nahrazený nejpravějším uzlem
 * levého podstromu. Nejpravější uzel nemusí být listem.
 *
 * Funkce korektně uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkci implementujte rekurzivně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key)
{
  // if we haven't found the key, do nothing
  if(*tree == NULL) {
    return;
  }
  // if we find the key, update the parent's pointers according to number of subtrees
  // of the current node
  if((*tree)->key == key) {
    // if the current node has both subtrees, update the current node with
    // the rightmost node of the left subtree 
    if((*tree)->right != NULL && (*tree)->left != NULL) {
      bst_replace_by_rightmost(*tree, &(*tree)->left);
    // if the current node has one subtree, update the current node with
    // that subtree and free the current node and set it to initial state
    } else if((*tree)->right != NULL) {
      bst_node_t *tmp = *tree;
      (*tree) = (*tree)->right;
      free(tmp->content.value);
      tmp->content.value = NULL;
      free(tmp);
    } else if((*tree)->left != NULL) {
      bst_node_t *tmp = *tree;
      (*tree) = (*tree)->left;
      free(tmp->content.value);
      tmp->content.value = NULL;
      free(tmp);
    // if it doesn't have any subtree, just free the current node and set it to NULL to get initial state
    } else {
      free((*tree)->content.value);
      (*tree)->content.value = NULL;
      free(*tree);
      *tree = NULL;
    }
    return;
  } else {
    // if we are not done with deleting, go to the right or left based on the key value
    if(key > (*tree)->key) {
      bst_delete(&(*tree)->right, key);
    } else {
      bst_delete(&(*tree)->left, key);
    }
  }
}

/*
 * Zrušení celého stromu.
 *
 * Po zrušení se celý strom bude nacházet ve stejném stavu jako po
 * inicializaci. Funkce korektně uvolní všechny alokované zdroje rušených
 * uzlů.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree)
{
  // free each tree node and update it to NULL to get initial state
  if(*tree != NULL) {
    bst_dispose(&(*tree)->left);
    bst_dispose(&(*tree)->right);

    if((*tree)->content.value != NULL) {
      free((*tree)->content.value);
      (*tree)->content.value = NULL;
    }

    free(*tree);
    *tree = NULL;
  }
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items)
{
  // root, left subtree, right subtree
  if(tree != NULL) {
    bst_add_node_to_items(tree, items);
    bst_preorder(tree->left, items);
    bst_preorder(tree->right, items);
  }
}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items)
{
  // left subtree, root, right subtree
  if(tree != NULL) {
    bst_inorder(tree->left, items);
    bst_add_node_to_items(tree, items);
    bst_inorder(tree->right, items);
  }
}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items)
{
  // left subtree, right subtree, root 
  if(tree != NULL) {
    bst_postorder(tree->left, items);
    bst_postorder(tree->right, items);
    bst_add_node_to_items(tree, items);
  }
}
