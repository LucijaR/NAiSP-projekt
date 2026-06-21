/**
 * @file stack_internal.h
 * @brief Privatni header za stogu - SAMO za interne .c datoteke
 * 
 * Ovaj header se NIKAD ne uključuje u javne headere ili korisnički kod.
 * Definira stvarnu implementaciju strukture Stack i vtable za sve dostupne
 * implementacije stoga.
 * 
 * VAŽNO: Samo src/stack.c i konkretne implementacije (stack_array.c, stack_list.c)
 *        smiju uključiti ovaj header!
 */

#ifndef STACK_INTERNAL_H
#define STACK_INTERNAL_H

#include "common.h"

/**
 * @struct StackVtable
 * @brief Virtual table (vtable) za stogu
 * 
 * Definira "ugovor" koji svaka konkretna implementacija stoga mora ispuniti.
 * Svaka implementacija (array-based ili linked-list-based) mora osigurati
 * pokazivače na sve ove funkcije.
 * 
 * Pokazivači u vtable-u se pozivaju sa impl pokazivačem koji pokazuje na
 * internu strukturu konkretne implementacije (npr. StackArray ili StackList).
 */
typedef struct {
    /**
     * @brief Push operacija: dodaj element na vrh stoga
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @param data Pokazivač na element koji se dodaje
     * @return Status operacije
     */
    Status (*push)(void* impl, const void* data);

    /**
     * @brief Pop operacija: ukloni i vrati element sa vrha stoga
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @param out Gdje će biti pohranjen uklanjani element
     * @return Status operacije
     */
    Status (*pop)(void* impl, void** out);

    /**
     * @brief Peek operacija: pročitaj element sa vrha bez uklanjanja
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @param out Gdje će biti pohranjen element
     * @return Status operacije
     */
    Status (*peek)(const void* impl, void** out);

    /**
     * @brief Provjeri je li stog prazna
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @return 1 ako je prazna, 0 inače
     */
    int (*is_empty)(const void* impl);

    /**
     * @brief Vrati broj elemenata u stogi
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @return Broj elemenata
     */
    size_t (*size)(const void* impl);

    /**
     * @brief Ispis svih elemenata stoge
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @param pf Print funkcija za ispis svakog elementa
     * @return Status operacije
     */
    Status (*print)(const void* impl, print_fn pf);

    /**
     * @brief Uništi internu implementaciju i oslobodi memoriju
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @param ff Free funkcija za oslobađanje elemenata (može biti NULL)
     * @return Status operacije
     */
    Status (*destroy)(void* impl, free_fn ff);
} StackVtable;

/**
 * @struct Stack
 * @brief Stvarna struktura za stogu - PRIVATNA
 * 
 * Ova struktura se NIKAD ne otkriva javnom header-u (stack.h).
 * Korisnik samo dobija opaque pokazivač Stack* bez znanja o ovoj strukturi.
 * 
 * Sve operacije na stogi prosljeđuju kontrolu odgovarajućoj funkciji iz vtable-a.
 */
struct Stack {
    void* impl;                     /**< Interni pokazivač na konkretnu implementaciju */
    const StackVtable* vt;          /**< Virtualna tablica funkcija */
    cmp_fn cmp;                     /**< Funkcija za poređenje elemenata */
    print_fn print;                 /**< Funkcija za ispis elementa */
    free_fn destructor;             /**< Funkcija za oslobađanje memorije elementa */
    size_t count;                   /**< Brojač elemenata (maintained by dispatcher) */
};

#endif // STACK_INTERNAL_H
