/**
 * Implementation author : Adam Běhoun (xbehoua00@fit.vutbr.cz)
 */

/*
 * Binární vyhledávací strom — iterativní varianta
 *
 * S využitím datových typů ze souboru btree.h, zásobníku ze souboru stack.h
 * a připravených koster funkcí implementujte binární vyhledávací
 * strom bez použití rekurze.
 */

#include "../btree.h"
#include "stack.h"
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
 * Funkci implementujte iterativně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, bst_node_content_t **value)
{
  while(tree != NULL) {
    if(tree->key == key) {
      *value = &tree->content;
      return true;
    } else if(key > tree->key) {
      tree = tree->right;
    } else {
      tree = tree->left;
    }
  }
  return false;
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
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_insert(bst_node_t **tree, char key, bst_node_content_t value)
{
  bst_node_t **ptr = tree;

  // traverse the tree until we get to the NULL node or to the node with given key
  // if we find the node with given key, update the content and free the old one
  while(*ptr != NULL) {
    if(key == (*ptr)->key) {
      free((*ptr)->content.value);
      (*ptr)->content = value;
      return;
    } else if(key > (*ptr)->key) {
      ptr = &(*ptr)->right;
    } else {
      ptr = &(*ptr)->left; 
    }
  }

  // create the node 
  *ptr = malloc(sizeof(bst_node_t));
  if(ptr == NULL) {
    fprintf(stderr, "Memory allocation error.\n");
    return;
  }
  // set the values
  (*ptr)->content = value;
  (*ptr)->key = key;
  (*ptr)->left = NULL;
  (*ptr)->right = NULL;
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 *
 * Klíč a hodnota uzlu target budou nahrazené klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 *
 * Tato pomocná funkce bude využita při implementaci funkce bst_delete.
 *
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree)
{
  bst_node_t **ptr = tree;

  // find the rightmost node
  while((*ptr)->right != NULL) {
    ptr = &(*ptr)->right;
  }

  // free the old content value and update the content and key 
  free(target->content.value);
  target->content = (*ptr)->content;
  target->key = (*ptr)->key;

  // if there is left subtree, we have to attach it to the parent node
  if((*ptr)->left != NULL) {
    bst_node_t *tmp = *ptr;
    *ptr = (*ptr)->left;
    free(tmp);
  } else {
    free(*ptr);
    *ptr = NULL;
  }
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
 * Funkci implementujte iterativně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key)
{
  bst_node_t **ptr = tree;

  while(*ptr != NULL) {
    // if we find the key, update the parent's pointers according to number of subtrees
    // of the current node
    if((*ptr)->key == key) {
      // if the current node has both subtrees, update the current node with
      // the rightmost node of the left subtree 
      if((*ptr)->right != NULL && (*ptr)->left != NULL) {
        bst_replace_by_rightmost(*ptr, &(*ptr)->left);
      // if the current node has one subtree, update the current node with
      // that subtree and free the current node and set it to initial state
      } else if((*ptr)->right != NULL) {
        bst_node_t *tmp = *ptr;
        (*ptr) = (*ptr)->right;
        free(tmp->content.value);
        tmp->content.value = NULL;
        free(tmp);
      } else if((*ptr)->left != NULL) {
        bst_node_t *tmp = *ptr;
        (*ptr) = (*ptr)->left;
        free(tmp->content.value);
        tmp->content.value = NULL;
        free(tmp);
      // if it doesn't have any subtree, just free the current node and set it to NULL to get initial state
      } else {
        free((*ptr)->content.value);
        (*ptr)->content.value = NULL;
        free(*ptr);
        *ptr = NULL;
      }
      return;
    } else {
      // if we are not done with deleting, go to the right or left based on the key value
      if(key > (*ptr)-> key) {
        ptr = &(*ptr)->right;
      } else {
        ptr = &(*ptr)->left;
      }
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
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree)
{
  stack_bst_t bst_stack;
  stack_bst_init(&bst_stack);

  // while the tree is not NULL or the stack is not empty
  do {
    // if the current node is NULL, we want to get the last right
    // node that we pushed to the stack
    if(*tree == NULL) {
      if(!stack_bst_empty(&bst_stack)) {
        *tree = stack_bst_top(&bst_stack);
        stack_bst_pop(&bst_stack);
      } 
    // if the current node is not NULL, check if it has right child
    // and if so, push it to the stack
    } else {
      if((*tree)->right != NULL) {
        stack_bst_push(&bst_stack, (*tree)->right);
      }
      // create temporary pointer, move the current node to the left child
      // and free the temporary pointer
      bst_node_t *tmp = *tree;
      *tree = (*tree)->left;
      free(tmp->content.value);
      free(tmp);
    }
  } while ((*tree != NULL) || (!stack_bst_empty(&bst_stack)));
}

/*
 * Pomocná funkce pro iterativní preorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu.
 * Nad zpracovanými uzly zavolá bst_add_node_to_items a uloží je do zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit, bst_items_t *items)
{
  // process the left diagonal of the tree
  while(tree != NULL) {
    stack_bst_push(to_visit, tree);
    bst_add_node_to_items(tree, items);
    tree = tree->left;
  }
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_preorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items)
{
  // stack initialization
  stack_bst_t bst_stack;
  stack_bst_init(&bst_stack);
  // call the helper function to process the left diagonal
  bst_leftmost_preorder(tree, &bst_stack, items);

  // if the stack is not empty, pop the node and call the helper function
  // on its right child - that will process the left diagonal of right child
  while(!stack_bst_empty(&bst_stack)) {
    tree = stack_bst_top(&bst_stack);
    stack_bst_pop(&bst_stack);
    bst_leftmost_preorder(tree->right, &bst_stack, items);
  }
}

/*
 * Pomocná funkce pro iterativní inorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit)
{
  // push the node on the stack without processing it 
  while(tree != NULL) {
    stack_bst_push(to_visit, tree);
    tree = tree->left;
  }
}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_inorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items)
{
  // stack initialization
  stack_bst_t bst_stack;
  stack_bst_init(&bst_stack);

  // load the stack with left diagonal
  bst_leftmost_inorder(tree, &bst_stack);

  // if the stack is not empty, pop the node, process it and call the helper
  // function to its right child. like this we will achieve the behaviour of InOrder (left, root, right)
  while(!stack_bst_empty(&bst_stack)) {
    tree = stack_bst_top(&bst_stack);
    stack_bst_pop(&bst_stack);
    bst_add_node_to_items(tree, items);
    bst_leftmost_inorder(tree->right, &bst_stack);
  }
}

/*
 * Pomocná funkce pro iterativní postorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů. Do zásobníku bool hodnot ukládá informaci, že uzel
 * byl navštíven poprvé.
 *
 * Funkci implementujte iterativně pomocí zásobníku uzlů a bool hodnot a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit)
{
  // load the stack with the left diagonal and for each node, push bool value to the second stack
  while(tree != NULL) {
    stack_bst_push(to_visit, tree);
    stack_bool_push(first_visit, true);
    tree = tree->left;
  }
}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_postorder a
 * zásobníku uzlů a bool hodnot a bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items)
{
  bool fromLeft;
  // stack initializations
  stack_bst_t bst_stack;
  stack_bst_init(&bst_stack);
  stack_bool_t bool_stack;
  stack_bool_init(&bool_stack);

  // load the stacks with left diagonal
  bst_leftmost_postorder(tree, &bst_stack, &bool_stack);

  // while the stack is not empty
  while(!stack_bst_empty(&bst_stack)) {
    // pop the node from the stack of nodes
    tree = stack_bst_top(&bst_stack);
    // pop the bool value from the stack of booleans
    fromLeft = stack_bool_top(&bool_stack);
    stack_bool_pop(&bool_stack);

    // if the bool value is true, we want to change the boolean value in the stack to false
    // and call the helper function on the right child of the current node
    if(fromLeft) {
      stack_bool_push(&bool_stack, false);
      bst_leftmost_postorder(tree->right, &bst_stack, &bool_stack);

    // if the bool value is false, we want to pop the node from the stack of nodes and process it
    } else {
      stack_bst_pop(&bst_stack);
      bst_add_node_to_items(tree, items);
    }
  }
}
