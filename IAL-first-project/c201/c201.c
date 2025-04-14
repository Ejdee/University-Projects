/**
 * Implementation author : Adam Běhoun (xbehoua00@fit.vutbr.cz)
 */


/* c201.c **********************************************************************
** Téma: Jednosměrný lineární seznam
**
**                     Návrh a referenční implementace: Petr Přikryl, říjen 1994
**                                          Úpravy: Andrea Němcová listopad 1996
**                                                   Petr Přikryl, listopad 1997
**                                Přepracované zadání: Petr Přikryl, březen 1998
**                                  Přepis do jazyka C: Martin Tuček, říjen 2004
**                                              Úpravy: Kamil Jeřábek, září 2020
**                                                    Daniel Dolejška, září 2021
**                                                    Daniel Dolejška, září 2022
**
** Implementujte abstraktní datový typ jednosměrný lineární seznam.
** Užitečným obsahem prvku seznamu je celé číslo typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou typu List.
** Definici konstant a typů naleznete v hlavičkovém souboru c201.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ List:
**
**      List_Dispose ....... zrušení všech prvků seznamu,
**      List_Init .......... inicializace seznamu před prvním použitím,
**      List_InsertFirst ... vložení prvku na začátek seznamu,
**      List_First ......... nastavení aktivity na první prvek,
**      List_GetFirst ...... vrací hodnotu prvního prvku,
**      List_DeleteFirst ... zruší první prvek seznamu,
**      List_DeleteAfter ... ruší prvek za aktivním prvkem,
**      List_InsertAfter ... vloží nový prvek za aktivní prvek seznamu,
**      List_GetValue ...... vrací hodnotu aktivního prvku,
**      List_SetValue ...... přepíše obsah aktivního prvku novou hodnotou,
**      List_Next .......... posune aktivitu na další prvek seznamu,
**      List_IsActive ...... zjišťuje aktivitu seznamu.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam předá
** někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**/

#include "c201.h"

#include <stdio.h> // printf
#include <stdlib.h> // malloc, free

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě. Nastaví error_flag na logickou 1.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void List_Error(void) {
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným
 * seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
 * že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu jednosměrně vázaného seznamu
 */
void List_Init( List *list ) {
	if(list == NULL) {
		List_Error();
	}

	// set the variables to some value
	list->activeElement = NULL;
	list->firstElement = NULL;
	list->currentLength = 0;
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam list do stavu, v jakém se nacházel
 * po inicializaci. Veškerá paměť používaná prvky seznamu list bude korektně
 * uvolněna voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 **/
void List_Dispose( List *list ) {
	if(list == NULL) {
		return;
	}

	// make two pointers to track one element ahead
	ListElementPtr current = list->firstElement;
	ListElementPtr next = NULL;

	// traverse the linked list
	while(current != NULL) {
		// set the next variable to the next element of element we are freeing
		// like this we wont lose track of elements
		next = current->nextElement;
		free(current);
		current = next;
		(list->currentLength)--;
	}

	// set the linked list to initial state
	list->activeElement = NULL;
	list->firstElement = NULL;
}

/**
 * Vloží prvek s hodnotou data na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void List_InsertFirst( List *list, int data ) {

	// allocate memory for the new element and check if it was successful
	ListElementPtr newElement = malloc(sizeof(struct ListElement));
	if(newElement == NULL) {
		List_Error();
		return;
	}
	
	// take track of the first element in the linked list
	ListElementPtr tmp = list->firstElement;

	// set the values for the new element
	newElement->data = data;
	newElement->nextElement = tmp;

	// set the first element to the new element and increment the current length of the list
	list->firstElement = newElement;
	(list->currentLength)++;
}

/**
 * Nastaví aktivitu seznamu list na jeho první prvek.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
void List_First( List *list ) {
	list->activeElement=list->firstElement;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void List_GetFirst( List *list, int *dataPtr ) {
	// check if the list is empty
	if(list->firstElement == NULL) {
		List_Error();
		return;
	}
	
	// set the value of first element of the list into the pointer to int
	*dataPtr = list->firstElement->data;
}

/**
 * Zruší první prvek seznamu list a uvolní jím používanou paměť.
 * Pokud byl rušený prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
void List_DeleteFirst( List *list ) {

	// if the linked list is empty, nothing happens so we return
	if(list->firstElement == NULL) {
		return;
	}
	
	// make the pointer to the second element that will be first after we cancel
	// it can be some pointer or NULL - both of these values are valid
	ListElementPtr next = list->firstElement->nextElement;

	// if the active element is the first element in the linked list
	// we set it to NULL
	if(list->activeElement == list->firstElement) {
		list->activeElement = NULL;
	}

	// free the first element
	free(list->firstElement);

	// set the first element to the second element that was there in the linked list
	// if the next element was NULL, then the first element is now also NULL, and therefore
	// the linked list is empty
	list->firstElement = next;
	// decrement the current length of the linked list
	(list->currentLength)--;
}

/**
 * Zruší prvek seznamu list za aktivním prvkem a uvolní jím používanou paměť.
 * Pokud není seznam list aktivní nebo pokud je aktivní poslední prvek seznamu list,
 * nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
void List_DeleteAfter( List *list ) {

	// if the active element doesnt exist or it is last element in the linked list then
	// do nothing
	if(list->activeElement == NULL || list->activeElement->nextElement == NULL) {
		return;
	}

	// take track of the element after the element we want to delete
	ListElementPtr next = list->activeElement->nextElement->nextElement;

	// free the memory used by the element
	free(list->activeElement->nextElement);

	// set the next element of the active element to the element we took track of before
	// and decrement the current length by one
	list->activeElement->nextElement = next;
	(list->currentLength)--;
}

/**
 * Vloží prvek s hodnotou data za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje!
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * zavolá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void List_InsertAfter( List *list, int data ) {

	// if the active element doesnt exist, do nothing
	if(list->activeElement == NULL) {
		return;
	}

	// allocate memory for the new element
	ListElementPtr new = malloc(sizeof(struct ListElement));
	if(new == NULL) {
		List_Error();
		return;
	}

	new->data = data;

	// take track of the next element of the active element
	ListElementPtr tmp = list->activeElement->nextElement;

	// the next element of the active element will be the new element 
	list->activeElement->nextElement = new;
	// and the next element of the new element will be the element we took track of before
	new->nextElement = tmp;

	(list->currentLength)++;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam není aktivní, zavolá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void List_GetValue( List *list, int *dataPtr ) {

	// if the active element doesnt exist, return the error message
	if(list->activeElement == NULL) {
		List_Error();
		return;
	}

	*dataPtr = list->activeElement->data;
}

/**
 * Přepíše data aktivního prvku seznamu list hodnotou data.
 * Pokud seznam list není aktivní, nedělá nic!
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void List_SetValue( List *list, int data ) {

	if(list->activeElement == NULL) {
		return;
	}

	list->activeElement->data = data;
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Všimněte si, že touto operací se může aktivní seznam stát neaktivním.
 * Pokud není předaný seznam list aktivní, nedělá funkce nic.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
void List_Next( List *list ) {
	if(list->activeElement == NULL) {
		return;
	}

	list->activeElement = list->activeElement->nextElement;
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Tuto funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
int List_IsActive( List *list ) {
	return (list->activeElement != NULL);
}

/* Konec c201.c */
