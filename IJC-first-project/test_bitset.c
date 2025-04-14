#include <stdio.h>
#include "bitset.h"

void vypis(bitset_t pole, bitset_index_t velikost_pole)
{
    printf("Vypisuju...\n");
    for (bitset_index_t i = 0; i < velikost_pole; i++)
    {
        printf("%lu\n", pole[i]);
    }
}

int main()
{
    printf("Alokuju pole...\n");
    bitset_alloc(pole, 100L);
    bitset_index_t bitset_size_velikost = bitset_size(pole);

    printf("bitset_size promenna:\nbitset_size makro: %lu\n\n", bitset_size_velikost);

    vypis(pole, bitset_size_velikost);

    printf("\n");

    printf("Nastavuju 64 bit na true...\n");
    bitset_setbit(pole, 64, true);

    vypis(pole, bitset_size_velikost);

    printf("Dostavam bity z pole...\n");
    bitset_index_t nulka = bitset_getbit(pole, 0);
    bitset_index_t sedesatctyri = bitset_getbit(pole, 64);

    printf("nulka: %lu\nsedesatctyri: %lu\n", nulka, sedesatctyri);

    printf("Vyplnuju pole 1...\n");
    bitset_fill(pole, true);

    vypis(pole, bitset_size_velikost);

    printf("\nULONG_MAX pro kontrolu: %lu\n\n", ULONG_MAX);

    printf("Vyplnuju pole 0...\n");
    bitset_fill(pole, false);

    vypis(pole, bitset_size_velikost);

    printf("\n");

    bitset_free(pole);
    return 0;
}