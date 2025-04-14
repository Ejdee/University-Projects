/**
 * Implementation author : Adam Běhoun (xbehoua00@fit.vutbr.cz)
 */

/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/*           Daniel Dolejška, září 2022                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
** uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c206.h"

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error(void) {
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init( DLList *list ) {
	list->firstElement = NULL;
	list->activeElement = NULL;
	list->lastElement = NULL;
	list->currentLength = 0;
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose( DLList *list ) {

	// if the list is empty, do nothing
	if(list->firstElement == NULL) {
		return;
	}

	// set the pointers
	DLLElementPtr current = list->firstElement;
	DLLElementPtr next;

	// traverse the list and free the elements one by one
	while(current != NULL) {
		next = current->nextElement;
		free(current);
		current = next;
		(list->currentLength)--;
	}

	// initialize the variables to default values
	list->firstElement = NULL;
	list->activeElement = NULL;
	list->lastElement = NULL;
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst( DLList *list, long data ) {

	// allocate memory for the new element and check if it was successful
	DLLElementPtr new = malloc(sizeof(struct DLLElement));
	if(new == NULL) {
		DLL_Error();
		return;
	}

	// set the values for the new element
	new->data = data;
	new->nextElement = list->firstElement;
	new->previousElement = NULL;

	if(list->firstElement != NULL) {
		list->firstElement->previousElement = new;
	}

	// update the first element in the list
	list->firstElement= new;

	// if it is the first element in the list, we add the pointer of the last element to it as well
	if(list->currentLength == 0) {
		list->lastElement = new;
	}

	// increment the length of the list
	(list->currentLength)++;
}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k DLL_InsertFirst).
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast( DLList *list, long data ) {

	// allocate memory for the new element and check if it was successful
	DLLElementPtr new = malloc(sizeof(struct DLLElement));
	if(new == NULL) {
		DLL_Error();
		return;
	}

	// set the values for the new element
	new->data = data;
	new->nextElement = NULL;
	new->previousElement = list->lastElement;

	if(list->lastElement != NULL) {
		list->lastElement->nextElement = new;
	}

	// update the last element in the list
	list->lastElement = new;

	// if it is the first element in the list, we add pointer of first element to it
	if(list->currentLength == 0) {
		list->firstElement = new;
	}

	// increment the length of the list
	(list->currentLength)++;
}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First( DLList *list ) {
	list->activeElement = list->firstElement;
}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last( DLList *list ) {
	list->activeElement = list->lastElement;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst( DLList *list, long *dataPtr ) {

	// if  the list is empty, then the current length is 0
	if(list->currentLength == 0) {
		DLL_Error();
		return;
	}

	*dataPtr = list->firstElement->data;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast( DLList *list, long *dataPtr ) {

	// if  the list is empty, then the current length is 0
	if(list->currentLength == 0) {
		DLL_Error();
		return;
	}

	*dataPtr = list->lastElement->data;
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst( DLList *list ) {

	// if  the list is empty, then the current length is 0
	if(list->currentLength == 0) {
		return;
	}

	// if the active element is the first one in the list, then we inactive it
	if(list->activeElement == list->firstElement) {
		list->activeElement = NULL;
	}

	// take track of the second element in the list
	DLLElementPtr next = list->firstElement->nextElement;

	// free the first element
	free(list->firstElement);

	// set the first element to the second one we took track of
	list->firstElement = next;
	// if the element was not NULL, we set the previous to NULL
	// on the other hand when the first element is NULL, we have to set the 
	// last element of the list to NULL as well 
	if(list->firstElement != NULL) {
		list->firstElement->previousElement = NULL;
	} else {
		list->lastElement = NULL;
	}

	// decrement the length of the list
	(list->currentLength)--;
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast( DLList *list ) {

	if(list->currentLength == 0) {
		return;
	}

	// if the active element is the last one, we set it to NULL
	if(list->activeElement == list->lastElement) {
		list->activeElement = NULL;
	}

	// take track of the previous element of the last element
	DLLElementPtr previous = list->lastElement->previousElement;

	// free the last element
	free(list->lastElement);

	// set the last element to the previous one
	list->lastElement = previous;

	// if the last element is not NULL, we set its next element to NULL because 
	// it is last one in the list
	// if the last element is NULL, we have to set first element of the list to 
	// the NULL as well
	if(list->lastElement != NULL) {
		list->lastElement->nextElement = NULL;
	} else {
		list->firstElement = NULL;
	}

	// decrement the length of the list
	(list->currentLength)--;
}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter( DLList *list ) {

	// if the list is not active or the active element is the last one, do nothing
	if(list->activeElement == NULL || list->activeElement == list->lastElement) {
		return;
	}

	// if the element we want to delete is the last element in the list,
	// we must update the last element to be the active element
	if(list->activeElement->nextElement == list->lastElement) {
		list->lastElement = list->activeElement;
	}

	// take track of the element after the one we are deleting
	DLLElementPtr next = list->activeElement->nextElement->nextElement;	

	// free the element after the active element
	free(list->activeElement->nextElement);
	// set the next element of the active element to the one we took track of
	list->activeElement->nextElement = next;

	// if the element is not NULL, we set the previous element to the active element
	if(next != NULL){
		next->previousElement = list->activeElement;
	}

	// decrement the length of the list
	(list->currentLength)--;
}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore( DLList *list ) {

	// if the list is not active or the active element is the last one, do nothing
	if(list->activeElement == NULL || list->activeElement == list->firstElement) {
		return;
	}

	// if the element we want to delete is the first element in the list
	// we must update the first element to be the active element now
	if(list->activeElement->previousElement == list->firstElement) {
		list->firstElement = list->activeElement;
	}

	// take track of the element after the one we are deleting
	DLLElementPtr previous = list->activeElement->previousElement->previousElement;	

	// free the element after the active element
	free(list->activeElement->previousElement);
	// set the next element of the active element to the one we took track of
	list->activeElement->previousElement = previous;

	// if the element is not NULL, we set the previous element to the active element
	if(previous != NULL){
		previous->nextElement = list->activeElement;
	}

	// decrement the length of the list
	(list->currentLength)--;
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter( DLList *list, long data ) {

	if(list->activeElement == NULL) {
		return;
	}
	
	// allocate the memory and check if it was successful
	DLLElementPtr new = malloc(sizeof(struct DLLElement));
	if(new == NULL) {
		DLL_Error();
		return;
	}

	// reorganize the pointers
	new->data = data;
	new->previousElement = list->activeElement;
	new->nextElement = list->activeElement->nextElement;
	list->activeElement->nextElement = new;
	if(new->nextElement != NULL) {
		new->nextElement->previousElement = new;
	}

	// if the active element was the last one in the list, we update it to the new one
	if(list->activeElement == list->lastElement) {
		list->lastElement = list->activeElement->nextElement;
	}

	// increment the length of the list
	(list->currentLength)++;
}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore( DLList *list, long data ) {
	
	if(list->activeElement == NULL) {
		return;
	}

	// allocate the memory and check if it was successful
	DLLElementPtr new = malloc(sizeof(struct DLLElement));
	if(new == NULL) {
		DLL_Error();
		return;
	}

	// reorganize the pointers
	new->data = data;
	new->nextElement = list->activeElement;
	new->previousElement = list->activeElement->previousElement;
	list->activeElement->previousElement = new;
	if(new->previousElement != NULL) {
		new->previousElement->nextElement = new;
	}

	// if the active element is the first element in the list, we have to update it
	// to the one created 
	if(list->activeElement == list->firstElement) {
		list->firstElement = list->activeElement->previousElement;
	}

	// increment the length of the list
	(list->currentLength)++;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue( DLList *list, long *dataPtr ) {
	
	if(list->activeElement == NULL) {
		DLL_Error();
		return;
	}

	*dataPtr = list->activeElement->data;
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue( DLList *list, long data ) {
	
	if(list->activeElement == NULL) {
		return;
	}

	list->activeElement->data = data;

}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next( DLList *list ) {
	
	if(list->activeElement == NULL) {
		return;
	}

	list->activeElement = list->activeElement->nextElement;
}


/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous( DLList *list ) {
	
	if(list->activeElement == NULL) {
		return;
	}

	list->activeElement = list->activeElement->previousElement;
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
bool DLL_IsActive( DLList *list ) {
	return (list->activeElement != NULL);
}

/* Konec c206.c */
