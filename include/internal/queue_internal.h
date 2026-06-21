/**
 * @file queue_internal.h
 * @brief Privatni header za red čekanja - SAMO za interne .c datoteke
 * 
 * Ovaj header se NIKAD ne uključuje u javne headiere ili korisnički kod.
 * Definira stvarnu implementaciju strukture Queue i vtable za sve dostupne
 * implementacije reda čekanja.
 * 
 * VAŽNO: Samo src/queue.c i konkretne implementacije (queue_array.c, queue_list.c)
 *        smiju uključiti ovaj header!
 */

#ifndef QUEUE_INTERNAL_H
#define QUEUE_INTERNAL_H

#include "common.h"

/**
 * @struct QueueVtable
 * @brief Virtual table (vtable) za red čekanja
 * 
 * Definira "ugovor" koji svaka konkretna implementacija reda čekanja mora ispuniti.
 * Svaka implementacija (array-based ili linked-list-based) mora osigurati
 * pokazivače na sve ove funkcije.
 * 
 * Pokazivači u vtable-u se pozivaju sa impl pokazivačem koji pokazuje na
 * internu strukturu konkretne implementacije (npr. QueueArray ili QueueList).
 */
typedef struct {
    /**
     * @brief Enqueue operacija: dodaj element na kraj reda čekanja
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @param data Pokazivač na element koji se dodaje
     * @return Status operacije
     */
    Status (*enqueue)(void* impl, const void* data);

    /**
     * @brief Dequeue operacija: ukloni i vrati element sa početka reda čekanja
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @param out Gdje će biti pohranjen uklanjani element
     * @return Status operacije
     */
    Status (*dequeue)(void* impl, void** out);

    /**
     * @brief Front operacija: pročitaj element sa početka bez uklanjanja
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @param out Gdje će biti pohranjen element
     * @return Status operacije
     */
    Status (*front)(const void* impl, void** out);

    /**
     * @brief Provjeri je li red čekanja prazan
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @return 1 ako je prazan, 0 inače
     */
    int (*is_empty)(const void* impl);

    /**
     * @brief Vrati broj elemenata u redu čekanja
     * 
     * @param impl Interni pokazivač na konkretnu implementaciju
     * @return Broj elemenata
     */
    size_t (*size)(const void* impl);

    /**
     * @brief Ispis svih elemenata reda čekanja
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
} QueueVtable;

/**
 * @struct Queue
 * @brief Stvarna struktura za red čekanja - PRIVATNA
 * 
 * Ova struktura se NIKAD ne otkriva javnom header-u (queue.h).
 * Korisnik samo dobija opaque pokazivač Queue* bez znanja o ovoj strukturi.
 * 
 * Sve operacije na redu čekanja prosljeđuju kontrolu odgovarajućoj funkciji
 * iz vtable-a.
 */
struct Queue {
    void* impl;                     /**< Interni pokazivač na konkretnu implementaciju */
    const QueueVtable* vt;          /**< Virtualna tablica funkcija */
    cmp_fn cmp;                     /**< Funkcija za poređenje elemenata */
    print_fn print;                 /**< Funkcija za ispis elementa */
    free_fn destructor;             /**< Funkcija za oslobađanje memorije elementa */
    size_t count;                   /**< Brojač elemenata (maintained by dispatcher) */
};

#endif // QUEUE_INTERNAL_H
