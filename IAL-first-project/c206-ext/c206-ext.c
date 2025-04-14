/**
 * Implementation author : Adam Běhoun (xbehoua00@fit.vutbr.cz)
 */

/*
 *  Předmět: Algoritmy (IAL) - FIT VUT v Brně
 *  Rozšíření pro příklad c206.c (Dvousměrně vázaný lineární seznam)
 *  Vytvořil: Daniel Dolejška, září 2024
 */

#include "c206-ext.h"

bool error_flag;
bool solved;

/**
 * Tato metoda simuluje příjem síťových paketů s určenou úrovní priority.
 * Přijaté pakety jsou zařazeny do odpovídajících front dle jejich priorit.
 * "Fronty" jsou v tomto cvičení reprezentovány dvousměrně vázanými seznamy
 * - ty totiž umožňují snazší úpravy pro již zařazené položky.
 * 
 * Parametr `packetLists` obsahuje jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Pokud fronta s odpovídající prioritou neexistuje, tato metoda ji alokuje
 * a inicializuje. Za jejich korektní uvolnení odpovídá volající.
 * 
 * V případě, že by po zařazení paketu do seznamu počet prvků v cílovém seznamu
 * překročil stanovený MAX_PACKET_COUNT, dojde nejdříve k promazání položek seznamu.
 * V takovémto případě bude každá druhá položka ze seznamu zahozena nehledě
 * na její vlastní prioritu ovšem v pořadí přijetí.
 * 
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param packet Ukazatel na strukturu přijatého paketu
 */
void receive_packet( DLList *packetLists, PacketPtr packet ) {

	if(MAX_PACKET_COUNT == 0) {
		fprintf(stderr, "Cannot receive any more packets.\n");
		return;
	}

	// take track of the first element of the list of the packets lists
	DLLElementPtr tmpElement = packetLists->firstElement;

	while(tmpElement != NULL) {
		QosPacketListPtr tmpList = (QosPacketListPtr) tmpElement->data;
		// if we find the list with correct priority, add the packet here
		if(packet->priority == tmpList->priority) {

			if((tmpList->list->currentLength)+1 > MAX_PACKET_COUNT) {

				// if the MAX_PACKER_COUNT is 1, and there is already one packet,
				// then we ignore the received packet
				if(MAX_PACKET_COUNT == 1) {
					return;
				} else {
					// take the track of the first element
					DLLElementPtr tmpDelete = tmpList->list->firstElement;
					// set the active element to the first one
					DLL_First(tmpList->list);

					// while tmpDelete is not NULL, then delete the element after the active
					// element and continue by move the pointer by one (so the pointer now
					// point to the next odd element), and we can continue with deleting
					// every even element
					while(tmpDelete != NULL) {
						DLL_DeleteAfter(tmpList->list);
						tmpDelete = tmpDelete->nextElement;
						DLL_Next(tmpList->list);
					}
				}
			}

			// lastly insert the packet to the last position in the list
			DLL_InsertLast(tmpList->list, (long)packet);

			return;
		}

		// update the element to take track of the next QosPacketList
		tmpElement = tmpElement->nextElement;
	}

	// **********************************************************************
	// We didnt find the list with the wanted priority, so we must create one
	// **********************************************************************

	// create a new element in the packets list
	QosPacketListPtr new = malloc(sizeof(QosPacketList));
	if(new == NULL) {
		fprintf(stderr, "allocation error\n");
		return;
	}

	// allocate memory for the new list of packets for the new priority
	new->list = malloc(sizeof(DLList));
	if(new->list == NULL) {
        free(new);
		fprintf(stderr, "allocation error\n");
		return;
	}

	// set the priority of the element in the packets list to the correct number
	new->priority = packet->priority;

	// initialize the list
	DLL_Init(new->list);

	// insert the packet in the first position of the new list
	DLL_InsertFirst(new->list, (long)packet);
	

	if(packetLists->currentLength == 0) {
		DLL_InsertFirst(packetLists, (long)new);
	} else {
		// take track of the first element in the packets list
		DLLElementPtr current = packetLists->firstElement;
		// set the first element in the list to the active one
		DLL_First(packetLists);
		while(current != NULL) {
			// access the data and check the priorities
			QosPacketListPtr data = (QosPacketListPtr) current->data;
			if(packet->priority > data->priority) {
				// if we find the correct spot to insert the new list, break from while
				break;
			}
			// set active element to the next element
			DLL_Next(packetLists);
			current = current->nextElement;
		}

		DLL_InsertBefore(packetLists, (long)new);
	}

}

/**
 * Tato metoda simuluje výběr síťových paketů k odeslání. Výběr respektuje
 * relativní priority paketů mezi sebou, kde pakety s nejvyšší prioritou
 * jsou vždy odeslány nejdříve. Odesílání dále respektuje pořadí, ve kterém
 * byly pakety přijaty metodou `receive_packet`.
 * 
 * Odeslané pakety jsou ze zdrojového seznamu při odeslání odstraněny.
 * 
 * Parametr `packetLists` obsahuje ukazatele na jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Parametr `outputPacketList` obsahuje ukazatele na odeslané pakety (`PacketPtr`).
 * 
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param outputPacketList Ukazatel na seznam paketů k odeslání
 * @param maxPacketCount Maximální počet paketů k odeslání
 */
void send_packets( DLList *packetLists, DLList *outputPacketList, int maxPacketCount ) {

	if(packetLists == NULL || outputPacketList == NULL || maxPacketCount == 0) {
		return;
	}

	int packetsSent = 0;

	DLLElementPtr tmpList = packetLists->firstElement;
	while(tmpList != NULL && packetsSent < maxPacketCount) {
		// take track of the first list of packets
		QosPacketListPtr tmpPacketList = (QosPacketListPtr)tmpList->data;
		// traverse the linked list and add each packet to the output packets list
		// and delete it
		DLLElementPtr tmpElement = tmpPacketList->list->firstElement;
		while(tmpElement != NULL && packetsSent < maxPacketCount) {
			DLL_InsertLast(outputPacketList, (long)tmpElement->data);
			DLL_DeleteFirst(tmpPacketList->list);

			packetsSent++;

			// because we deleted the first elements, we want to take track of another one
			tmpElement = tmpPacketList->list->firstElement;
		}

		tmpList = tmpList->nextElement;
	}
}
