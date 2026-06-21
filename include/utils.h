/**
 * @file utils.h
 * @brief Pomoćne funkcije za usporedbu, ispis i oslobađanje osnovnih tipova
 * 
 * Ovaj header sadrži konkretne implementacije cmp_fn, print_fn i free_fn
 * funkcijskih pokazivača za standardne tipove podataka (int, string),
 * koje korisnik može direktno koristiti pri kreiranju struktura.
 */

#ifndef UTILS_H
#define UTILS_H

#include "common.h"

/**
 * @brief Uspoređuje dvije int vrijednosti
 * 
 * @param a Pokazivač na prvi int (const void*, mora pokazivati na int)
 * @param b Pokazivač na drugi int (const void*, mora pokazivati na int)
 * 
 * @return Negativan broj ako je a < b, 0 ako su jednaki, pozitivan ako je a > b
 */
int int_cmp(const void* a, const void* b);

/**
 * @brief Uspoređuje dva stringa
 * 
 * Koristi se kao cmp_fn pri kreiranju struktura koje sadrže string podatke.
 * Interno koristi standardnu funkciju strcmp().
 * 
 * @param a Pokazivač na prvi element (mora pokazivati na char*)
 * @param b Pokazivač na drugi element (mora pokazivati na char*)
 * 
 * @return Negativan broj ako je a < b (leksikografski),
 *         0 ako su stringovi jednaki,
 *         Pozitivan broj ako je a > b (leksikografski)
 * 
 * @note Vlasnik memorije: Korisnik - funkcija samo čita memoriju na koju
 *       pokazivači a i b pokazuju.
 */

int string_cmp(const void* a, const void* b);

/**
 * @brief Ispisuje int vrijednost na standardni izlaz
 * 
 * Koristi se kao print_fn pri ispisu struktura koje sadrže int podatke.
 * 
 * @param data Pokazivač na element koji treba ispisati (mora pokazivati na int)
 * 
 * @note Vlasnik memorije: Korisnik - funkcija samo čita memoriju na koju
 *       data pokazuje. Ne smije oslobađati memoriju.
 */
void int_print(const void* data);

/**
 * @brief Ispisuje string na standardni izlaz
 * 
 * Koristi se kao print_fn pri ispisu struktura koje sadrže string podatke.
 * 
 * @param data Pokazivač na element koji treba ispisati (mora pokazivati na char*)
 * 
 * @note Vlasnik memorije: Korisnik - funkcija samo čita memoriju na koju
 *       data pokazuje. Ne smije oslobađati memoriju.
 */
void string_print(const void* data);

/**
 * @brief Oslobađa memoriju dinamički alociranog elementa
 * 
 * Generička free funkcija koja se može koristiti za bilo koji tip podatka
 * koji je dinamički alociran (malloc/strdup), bez obzira radi li se
 * o int, stringu ili strukturi.
 * 
 * @param data Pokazivač na element čija memorija treba biti oslobođena
 * 
 * @note Vlasnik memorije: Ova funkcija - oslobađa memoriju na koju
 *       data pokazuje. Nakon poziva, pokazivač postaje nevažeći.
 */
void generic_free(void* data);

#endif // UTILS_H