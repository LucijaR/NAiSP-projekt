/**
 * @file list_internal.h
 * @brief Privatni header za listu - SAMO za interne .c datoteke
 * 
 * Ovaj header se NIKAD ne uključuje u javne headere ili korisnički kod.
 * Definira stvarnu implementaciju strukture List i vtable za sve dostupne
 * implementacije liste.
 * 
 * VAŽNO: Samo src/list.c i konkretne implementacije (list_singly.c, list_doubly.c)
 *        smiju uključiti ovaj header!
 */

#ifndef LIST_INTERNAL_H
#define LIST_INTERNAL_H

#include "common.h"

/**
 * @struct ListVtable
 * @brief Virtual table (vtable) za listu
 * 
 * Definira "ugovor" koji svaka konkretna implementacija liste mora ispuniti.
 * Svaka implementacija (singly-linked ili doubly-linked) mora osigurati
 * pokazivače na sve ove funkcije.
 * 
 * Pokazivači u vtable-u se pozivaju sa impl pokazivačem koji pokazuje na
 * internu strukturu konkretne implementacije (npr. ListSingly ili ListDoubly).
 */
typedef struct {
    /**
     * @brief Dodaj element na početak liste
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @param data Pokazivač na element koji se dodaje
     * @return Status operacije
     */
    Status (*insert_front)(void* impl, const void* data);

    /**
     * @brief Dodaj element na kraj liste
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @param data Pokazivač na element koji se dodaje
     * @return Status operacije
     */
    Status (*insert_back)(void* impl, const void* data);

    /**
     * @brief Ukloni element sa početka liste
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @param out Gdje će biti pohranjen uklanjani element
     * @return Status operacije
     */
    Status (*remove_front)(void* impl, void** out);

    /**
     * @brief Ukloni element sa kraja liste
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @param out Gdje će biti pohranjen uklanjani element
     * @return Status operacije
     */
    Status (*remove_back)(void* impl, void** out);

    /**
     * @brief Dohvati element na zadanom indeksu
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @param index Indeks elementa (0-based)
     * @param out Gdje će biti pohranjen element
     * @return Status operacije
     */
    Status (*get)(void* impl, size_t index, void** out);

    /**
     * @brief Provjeri je li lista prazna
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @return 1 ako je prazna, 0 inače
     */
    int (*is_empty)(const void* impl);

    /**
     * @brief Vrati broj elemenata u listi
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @return Broj elemenata
     */
    size_t (*size)(const void* impl);

    /**
     * @brief Ispis svih elemenata liste
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
} ListVtable;

/**
 * @struct List
 * @brief Stvarna struktura za listu - PRIVATNA
 * 
 * Ova struktura se NIKAD ne otkriva javnom header-u (list.h).
 * Korisnik samo dobija opaque pokazivač List* bez znanja o ovoj strukturi.
 * 
 * Sve operacije na listi prosljeđuju kontrolu odgovarajućoj funkciji iz vtable-a.
 */
struct List {
    void* impl;                     /**< Interni pokazivač na konkretnu implementaciju */
    const ListVtable* vt;           /**< Virtualna tablica funkcija */
    cmp_fn cmp;                     /**< Funkcija za poređenje elemenata */
    print_fn print;                 /**< Funkcija za ispis elementa */
    free_fn destructor;             /**< Funkcija za oslobađanje memorije elementa */
    size_t count;                   /**< Brojač elemenata (maintained by dispatcher) */
};

#endif // LIST_INTERNAL_H
