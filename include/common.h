/**
 * @file common.h
 * @brief Zajedničke type definition-e i enumi za sve linearne strukture podataka
 * 
 * Ovaj header sadrži:
 * - Function pointer typedef-e (cmp_fn, print_fn, free_fn)
 * - Status enum za konsistentno rukovanje greškama
 * 
 * Svaka linearna struktura koristi ove tipove za fleksibilnost i genereičnost.
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>

/**
 * @typedef cmp_fn
 * @brief Pokazivač na funkciju za poređenje dva elementa
 * 
 * Koristi se za pronalaženje, sortiranje i jednakost elemenata.
 * 
 * @param a Prvi element (const, ne smije se mijenjati)
 * @param b Drugi element (const, ne smije se mijenjati)
 * 
 * @return Negativan broj ako je a < b,
 *         0 ako je a == b,
 *         Pozitivan broj ako je a > b
 * 
 * @note Vlasnik memorije: Korisnik - funkcija samo čita memoriju na koju
 *       pokazivači a i b pokazuju.
 */
typedef int (*cmp_fn)(const void* a, const void* b);

/**
 * @typedef print_fn
 * @brief Pokazivač na funkciju za ispis elementa
 * 
 * Koristi se za ispis sadržaja strukture pri pozovu funkcije *_print().
 * 
 * @param data Pokazivač na element koji treba ispisati (const, ne smije se mijenjati)
 * 
 * @note Vlasnik memorije: Korisnik - funkcija samo čita memoriju na koju
 *       data pokazuje. Ne smije oslobađati memoriju.
 */
typedef void (*print_fn)(const void* data);

/**
 * @typedef free_fn
 * @brief Pokazivač na funkciju za oslobađanje memorije elementa
 * 
 * Koristi se pri uništavanju strukture (destroy) za oslobađanje memorije
 * koju je korisnik alocirao za svaki element.
 * 
 * Ako je NULL, struktura će pretpostaviti da elementi nisu dinamički alocirani
 * i neće pokušati osloboditi memoriju.
 * 
 * @param data Pokazivač na element čija memorija treba biti oslobođena
 * 
 * @note Vlasnik memorije: free_fn - funkcija je odgovorna za oslobađanje
 *       memorije na koju data pokazuje. Struktura će prepisati/zaboraviti
 *       taj pokazivač nakon što destroy() završi.
 * 
 * @note Ako korisnik proslijedi NULL kao free_fn pri kreiranju strukture,
 *       struktura će ostavljati memoriju elementa neoiluđenu.
 */
typedef void (*free_fn)(void* data);

/**
 * @enum Status
 * @brief Status kodovi za sve operacije koje vraćaju Status
 * 
 * Konzistentno rukovanje greškama kroz sve linearne strukture.
 */
typedef enum {
    STATUS_OK             = 0,  /**< Operacija je uspješna */
    STATUS_EMPTY          = 1,  /**< Struktura je prazna (pop, peek, itd.) */
    STATUS_FULL           = 2,  /**< Struktura je puna (push, insert, itd. - samo za array-based) */
    STATUS_ALLOC_ERROR    = 3,  /**< Neuspješna alokacija memorije */
    STATUS_NULL_ARG       = 4   /**< Proslijeđeni argument je NULL (obavezna provera) */
} Status;

#endif // COMMON_H
