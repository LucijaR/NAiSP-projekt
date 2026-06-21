/**
 * @file list.h
 * @brief Javni API za listu (linearnu strukturu sa pristupom na bilo kojem mjestu)
 * 
 * Lista je linearna struktura u kojoj se elementi mogu dodavati i uklanjati
 * sa bilo kojeg kraja (front/back) i pristupati na bilo kojem indeksu.
 * 
 * Ovaj header definira javni interface bez otkrivanja implementacijskih detalja.
 * Korisnik može izvabiti između različitih implementacija (singly-linked ili
 * doubly-linked) pri kreiranju pomoću ListImplType enum-a.
 */

#ifndef LIST_H
#define LIST_H

#include "common.h"

/**
 * @enum ListImplType
 * @brief Tipovi dostupnih implementacija za listu
 * 
 * Koristi se pri kreiranju liste pomoću list_create() da bi se specificirala
 * željeni tip memorijske organizacije.
 */
typedef enum {
    LIST_IMPL_SINGLY,      /**< Singly-linked list (jedan smjer - prema sljedećem) */
    LIST_IMPL_DOUBLY       /**< Doubly-linked list (dva smjera - prije i nakon) */
} ListImplType;

/**
 * @typedef List
 * @brief Opaque tip za listu
 * 
 * Stvarna struktura je definirana samo u internal/list_internal.h
 * Korisnik nikad ne pristupa poljima strukture direktno.
 */
typedef struct List List;

/**
 * @brief Kreira novu listu sa zadanom implementacijom
 * 
 * Alocira i inicijalizira novu listu sa odabranom implementacijom.
 * Sve operacije na listi će se delegirati na vtable odabranog tipa.
 * 
 * @param impl Tip implementacije (LIST_IMPL_SINGLY ili LIST_IMPL_DOUBLY)
 * @param cmp  Funkcija za poređenje elemenata (može biti NULL ako se ne koristi)
 * @param print Funkcija za ispis elemenata (može biti NULL)
 * @param destructor Funkcija za oslobađanje memorije elementa (može biti NULL)
 * 
 * @return Pokazivač na novokranu listu, ili NULL ako je došlo do greške
 * 
 * @note Vlasnik memorije: Korisnik - obaveza je pozvati list_destroy()
 *       kada više nije potrebna lista
 * 
 * @note Ako proslijedi NULL za destructor, lista neće pokušavati osloboditi
 *       memoriju elemenata pri destroy() - korisnik je odgovoran
 */
List* list_create(ListImplType impl, cmp_fn cmp, print_fn print,
                  free_fn destructor);

/**
 * @brief Uništava listu i oslobađa svu pridruženu memoriju
 * 
 * Oslobađa sve elemente u listi (koristeći destructor ako je proslijeđen)
 * i zatim oslobađa memoriju same liste.
 * 
 * @param list Pokazivač na listu (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je uspješno uništena
 * @return STATUS_NULL_ARG ako je list NULL
 * 
 * @note Nakon poziva ove funkcije, pokazivač list postaje nevažeći
 */
Status list_destroy(List* list);

/**
 * @brief Dodaje element na početak liste
 * 
 * Novi element postaje novi prvi element liste.
 * 
 * @param list Pokazivač na listu (ne smije biti NULL)
 * @param data Pokazivač na element koji se dodaje (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je element uspješno dodan
 * @return STATUS_NULL_ARG ako je list ili data NULL
 * @return STATUS_ALLOC_ERROR ako alokacija memorije nije uspješna
 * 
 * @note Vlasnik memorije: Korisnik zadržava vlasništvo - lista samo kopira
 *       sadržaj na koji data pokazuje (ili čuva sam pokazivač, zavisno od
 *       implementacije). Korisnik je odgovoran da memorija ostane validna
 *       dok se element nalazi u listi.
 */
Status list_insert_front(List* list, const void* data);

/**
 * @brief Dodaje element na kraj liste
 * 
 * Novi element postaje novi zadnji element liste.
 * 
 * @param list Pokazivač na listu (ne smije biti NULL)
 * @param data Pokazivač na element koji se dodaje (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je element uspješno dodan
 * @return STATUS_NULL_ARG ako je list ili data NULL
 * @return STATUS_ALLOC_ERROR ako alokacija memorije nije uspješna
 * 
 * @note Vlasnik memorije: Korisnik zadržava vlasništvo - lista samo kopira
 *       sadržaj na koji data pokazuje (ili čuva sam pokazivač, zavisno od
 *       implementacije). Korisnik je odgovoran da memorija ostane validna
 *       dok se element nalazi u listi.
 */
Status list_insert_back(List* list, const void* data);

/**
 * @brief Uklanja element sa početka liste
 * 
 * @param list Pokazivač na listu (ne smije biti NULL)
 * @param out Pokazivač gdje će biti pohranjen uklanjani element
 *            (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je element uspješno uklonjen
 * @return STATUS_NULL_ARG ako je list ili out NULL
 * @return STATUS_EMPTY ako je lista prazna
 * 
 * @note Vlasnik memorije: Nakon remove-a, korisnik postaje vlasnik elementa
 *       (ako je to dinamički alocirana memorija). Korisnik mora paziti
 *       da je ne oslobodi dva puta - destructor se neće pozvati.
 */
Status list_remove_front(List* list, void** out);

/**
 * @brief Uklanja element sa kraja liste
 * 
 * @param list Pokazivač na listu (ne smije biti NULL)
 * @param out Pokazivač gdje će biti pohranjen uklanjani element
 *            (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je element uspješno uklonjen
 * @return STATUS_NULL_ARG ako je list ili out NULL
 * @return STATUS_EMPTY ako je lista prazna
 * 
 * @note Vlasnik memorije: Nakon remove-a, korisnik postaje vlasnik elementa
 *       (ako je to dinamički alocirana memorija). Korisnik mora paziti
 *       da je ne oslobodi dva puta - destructor se neće pozvati.
 */
Status list_remove_back(List* list, void** out);

/**
 * @brief Dohvaća element na zadanom indeksu
 * 
 * @param list Pokazivač na listu (ne smije biti NULL)
 * @param index Indeks elementa (0-based, od početka liste)
 * @param out Pokazivač gdje će biti pohranjen traženi element
 *            (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je element uspješno pročitan
 * @return STATUS_NULL_ARG ako je list ili out NULL
 * @return STATUS_EMPTY ako je lista prazna
 * @return STATUS_ALLOC_ERROR ako index izlazi van granica
 * 
 * @note Vlasnik memorije: Korisnik NE postaje vlasnik - element ostaje
 *       vlasništvo liste. Korisnik smije samo čitati memoriju, ne smije
 *       je mijenjati niti brisati.
 */
Status list_get(List* list, size_t index, void** out);

/**
 * @brief Provjeri je li lista prazna
 * 
 * @param list Pokazivač na listu (ne smije biti NULL)
 * 
 * @return 1 ako je lista prazna
 * @return 0 ako lista nije prazna ili ako je list NULL
 */
int list_is_empty(const List* list);

/**
 * @brief Vraća broj elemenata u listi
 * 
 * @param list Pokazivač na listu (ne smije biti NULL)
 * 
 * @return Broj elemenata, ili 0 ako je list NULL
 */
size_t list_size(const List* list);

/**
 * @brief Ispisuje sve elemente liste
 * 
 * Koristi proslijeđenu print funkciju (ili onu iz list_create())
 * da ispise sve elemente od početka prema kraju.
 * 
 * Ako print funkcija nije proslijeđena, ista se ne ispisuje ništa
 * osim broja elemenata.
 * 
 * @param list Pokazivač na listu (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je ispis uspješan
 * @return STATUS_NULL_ARG ako je list NULL
 */
Status list_print(const List* list);

#endif // LIST_H
