/**
 * @file stack.h
 * @brief Javni API za stogu (LIFO - Last In, First Out)
 * 
 * Stog je linearna struktura u kojoj se dodavaju i uklanjaju elementi sa vrha.
 * Ovaj header definiše javni interfejs bez otkrivanja implementacijskih detalja.
 * 
 * Korisnik može izvabiti između različitih implementacija (array-based ili
 * linked-list-based) pri kreiranju pomoću StackImplType enum-a.
 * Javni API ostaje identičan bez obzira na odabranu implementaciju.
 */

#ifndef STACK_H
#define STACK_H

#include "common.h"

/**
 * @enum StackImplType
 * @brief Tipovi dostupnih implementacija za stog
 * 
 * Koristi se pri kreiranju stoga pomoću stack_create() da bi se specificirala
 * zeljeni tip memorijske organizacije.
 */
typedef enum {
    STACK_IMPL_ARRAY,          /**< Array-based implementacija (fiksna kapaciteta) */
    STACK_IMPL_LINKED_LIST     /**< Linked-list-based implementacija (dinamički) */
} StackImplType;

/**
 * @typedef Stack
 * @brief Opaque tip za stogu
 * 
 * Stvarna struktura je definirana samo u internal/stack_internal.h
 * Korisnik nikad ne pristupa poljima strukture direktno.
 */
typedef struct Stack Stack;

/**
 * @brief Kreira novi stog sa zadanom implementacijom
 * 
 * Alocira i inicijalizira novu stogu sa odabranom implementacijom.
 * Sve operacije na stogi će se delegirati na vtable odabranog tipa.
 * 
 * @param impl Tip implementacije (STACK_IMPL_ARRAY ili STACK_IMPL_LINKED_LIST)
 * @param cmp  Funkcija za poređenje elemenata (može biti NULL ako se ne koristi)
 * @param print Funkcija za ispis elemenata (može biti NULL)
 * @param destructor Funkcija za oslobađanje memorije elementa (može biti NULL)
 * @param capacity Kapaciteta (ignorirana za STACK_IMPL_LINKED_LIST)
 * 
 * @return Pokazivač na novokranu stogu, ili NULL ako je došlo do greške
 * 
 * @note Vlasnik memorije: Korisnik - obaveza je pozvati stack_destroy()
 *       kada više nije potrebna stoga
 * 
 * @note Ako proslijedi NULL za destructor, stoga neće pokušavati osloboditi
 *       memoriju elemenata pri destroy() - korisnik je odgovoran
 */
Stack* stack_create(StackImplType impl, cmp_fn cmp, print_fn print,
                    free_fn destructor, size_t capacity);

/**
 * @brief Uništava stogu i oslobađa svu pridruženu memoriju
 * 
 * Oslobađa sve elemente u stogi (koristeći destructor ako je proslijeđen)
 * i zatim oslobađa memoriju samog stoga.
 * 
 * @param stack Pokazivač na stogu (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je uspješno uništena
 * @return STATUS_NULL_ARG ako je stack NULL
 * 
 * @note Nakon poziva ove funkcije, pokazivač stack postaje nevažeći
 */
Status stack_destroy(Stack* stack);

/**
 * @brief Dodaje element na vrh stoga (push operacija)
 * 
 * Ide u stog sa zadanim elementom. Ako je stog pun (array-based implementacija),
 * vraća STATUS_FULL.
 * 
 * @param stack Pokazivač na stogu (ne smije biti NULL)
 * @param data Pokazivač na element koji se dodaje (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je element uspješno dodan
 * @return STATUS_NULL_ARG ako je stack ili data NULL
 * @return STATUS_FULL ako je stog pun (array-based implementacija)
 * @return STATUS_ALLOC_ERROR ako alokacija memorije nije uspješna
 * 
 * @note Vlasnik memorije: Korisnik zadržava vlasništvo - stog samo kopira
 *       sadržaj na koji data pokazuje (ili čuva sam pokazivač, zavisno od
 *       implementacije). Korisnik je odgovoran da memorija ostane validna
 *       dok se element nalazi u stogi.
 */
Status stack_push(Stack* stack, const void* data);

/**
 * @brief Uklanja i vraća element sa vrha stoga (pop operacija)
 * 
 * Uklanja element sa vrha stoga i vraća ga kroz out parametar.
 * 
 * @param stack Pokazivač na stogu (ne smije biti NULL)
 * @param out Pokazivač gdje će biti pohranjen uklanjeni element
 *            (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je element uspješno uklonjen
 * @return STATUS_NULL_ARG ako je stack ili out NULL
 * @return STATUS_EMPTY ako je stog prazna
 * 
 * @note Vlasnik memorije: Nakon pop-a, korisnik postaje vlasnik elementa
 *       (ako je to dinamički alocirana memorija). Korisnik mora paziti
 *       da je ne oslobodi dva puta - destructor se neće pozvati.
 */
Status stack_pop(Stack* stack, void** out);

/**
 * @brief Vraća element sa vrha stoga bez uklanjanja (peek operacija)
 * 
 * Čita element sa vrha stoga bez mijenjanja stoga.
 * 
 * @param stack Pokazivač na stogu (ne smije biti NULL)
 * @param out Pokazivač gdje će biti pohranjen element
 *            (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je element uspješno pročitan
 * @return STATUS_NULL_ARG ako je stack ili out NULL
 * @return STATUS_EMPTY ako je stog prazna
 * 
 * @note Vlasnik memorije: Korisnik NE postaje vlasnik - element ostaje
 *       vlasništvo stoga. Korisnik smije samo čitati memoriju, ne smije
 *       je mijenjati niti brisati.
 */
Status stack_peek(Stack* stack, void** out);

/**
 * @brief Provjeri je li stog prazna
 * 
 * @param stack Pokazivač na stogu (ne smije biti NULL)
 * 
 * @return 1 ako je stog prazna
 * @return 0 ako stog nije prazna ili ako je stack NULL
 */
int stack_is_empty(const Stack* stack);

/**
 * @brief Vraća broj elemenata u stogi
 * 
 * @param stack Pokazivač na stogu (ne smije biti NULL)
 * 
 * @return Broj elemenata, ili 0 ako je stack NULL
 */
size_t stack_size(const Stack* stack);

/**
 * @brief Ispisuje sve elemente stoge
 * 
 * Koristi proslijeđenu print funkciju (ili onu iz stack_create())
 * da ispise sve elemente od vrha prema dnu.
 * 
 * Ako print funkcija nije proslijeđena, ista se ne ispisuje ništa
 * osim broja elemenata.
 * 
 * @param stack Pokazivač na stogu (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je ispis uspješan
 * @return STATUS_NULL_ARG ako je stack NULL
 */
Status stack_print(const Stack* stack);

#endif // STACK_H
