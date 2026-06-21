/**
 * @file queue.h
 * @brief Javni API za red čekanja (FIFO - First In, First Out)
 * 
 * Red čekanja je linearna struktura u kojoj se elementi dodavaju na kraju
 * (enqueue) i uklanjaju sa početka (dequeue).
 * 
 * Ovaj header definira javni interface bez otkrivanja implementacijskih detalja.
 * Korisnik može izvabiti između različitih implementacija (array-based ili
 * linked-list-based) pri kreiranju pomoću QueueImplType enum-a.
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

/**
 * @enum QueueImplType
 * @brief Tipovi dostupnih implementacija za red čekanja
 * 
 * Koristi se pri kreiranju reda čekanja pomoću queue_create() da bi se specificirala
 * željeni tip memorijske organizacije.
 */
typedef enum {
    QUEUE_IMPL_ARRAY           /**< Array-based implementacija (kružna, dinamički proširiva) */
} QueueImplType;

/**
 * @typedef Queue
 * @brief Opaque tip za red čekanja
 * 
 * Stvarna struktura je definirana samo u internal/queue_internal.h
 * Korisnik nikad ne pristupa poljima strukture direktno.
 */
typedef struct Queue Queue;

/**
 * @brief Kreira novi red čekanja sa zadanom implementacijom
 * 
 * Alocira i inicijalizira novi red čekanja sa odabranom implementacijom.
 * Sve operacije na redu čekanja će se delegirati na vtable odabranog tipa.
 * 
 * @param impl Tip implementacije (QUEUE_IMPL_ARRAY)
 * @param cmp  Funkcija za poređenje elemenata (može biti NULL ako se ne koristi)
 * @param print Funkcija za ispis elemenata (može biti NULL)
 * @param destructor Funkcija za oslobađanje memorije elementa (može biti NULL)
 * @param capacity Kapaciteta kružnog polja
 * 
 * @return Pokazivač na novo kranirani red čekanja, ili NULL ako je došlo do greške
 * 
 * @note Vlasnik memorije: Korisnik - obaveza je pozvati queue_destroy()
 *       kada više nije potreban red čekanja
 * 
 * @note Ako proslijedi NULL za destructor, red čekanja neće pokušavati osloboditi
 *       memoriju elemenata pri destroy() - korisnik je odgovoran
 */
Queue* queue_create(QueueImplType impl, cmp_fn cmp, print_fn print,
                    free_fn destructor, size_t capacity);

/**
 * @brief Uništava red čekanja i oslobađa svu pridruženu memoriju
 * 
 * Oslobađa sve elemente u redu čekanja (koristeći destructor ako je proslijeđen)
 * i zatim oslobađa memoriju samog reda čekanja.
 * 
 * @param queue Pokazivač na red čekanja (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je uspješno uništen
 * @return STATUS_NULL_ARG ako je queue NULL
 * 
 * @note Nakon poziva ove funkcije, pokazivač queue postaje nevažeći
 */
Status queue_destroy(Queue* queue);

/**
 * @brief Dodaje element na kraj reda čekanja (enqueue operacija)
 * 
 * Novi element se dodaje na kraj reda čekanja. Ako je red čekanja pun
 * (array-based implementacija), vraća STATUS_FULL.
 * 
 * @param queue Pokazivač na red čekanja (ne smije biti NULL)
 * @param data Pokazivač na element koji se dodaje (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je element uspješno dodan
 * @return STATUS_NULL_ARG ako je queue ili data NULL
 * @return STATUS_FULL ako je red čekanja pun (array-based implementacija)
 * @return STATUS_ALLOC_ERROR ako alokacija memorije nije uspješna
 * 
 * @note Vlasnik memorije: Korisnik zadržava vlasništvo - red čekanja samo kopira
 *       sadržaj na koji data pokazuje (ili čuva sam pokazivač, zavisno od
 *       implementacije). Korisnik je odgovoran da memorija ostane validna
 *       dok se element nalazi u redu čekanja.
 */
Status queue_enqueue(Queue* queue, const void* data);

/**
 * @brief Uklanja i vraća element sa početka reda čekanja (dequeue operacija)
 * 
 * Uklanja element sa početka (čela) reda čekanja i vraća ga kroz out parametar.
 * 
 * @param queue Pokazivač na red čekanja (ne smije biti NULL)
 * @param out Pokazivač gdje će biti pohranjen uklanjani element
 *            (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je element uspješno uklonjen
 * @return STATUS_NULL_ARG ako je queue ili out NULL
 * @return STATUS_EMPTY ako je red čekanja prazan
 * 
 * @note Vlasnik memorije: Nakon dequeue-a, korisnik postaje vlasnik elementa
 *       (ako je to dinamički alocirana memorija). Korisnik mora paziti
 *       da je ne oslobodi dva puta - destructor se neće pozvati.
 */
Status queue_dequeue(Queue* queue, void** out);

/**
 * @brief Vraća element sa početka reda čekanja bez uklanjanja (front operacija)
 * 
 * Čita element sa početka (čela) reda čekanja bez mijenjanja reda čekanja.
 * 
 * @param queue Pokazivač na red čekanja (ne smije biti NULL)
 * @param out Pokazivač gdje će biti pohranjen element
 *            (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je element uspješno pročitan
 * @return STATUS_NULL_ARG ako je queue ili out NULL
 * @return STATUS_EMPTY ako je red čekanja prazan
 * 
 * @note Vlasnik memorije: Korisnik NE postaje vlasnik - element ostaje
 *       vlasništvo reda čekanja. Korisnik smije samo čitati memoriju, ne smije
 *       je mijenjati niti brisati.
 */
Status queue_front(Queue* queue, void** out);

/**
 * @brief Provjeri je li red čekanja prazan
 * 
 * @param queue Pokazivač na red čekanja (ne smije biti NULL)
 * 
 * @return 1 ako je red čekanja prazan
 * @return 0 ako red čekanja nije prazan ili ako je queue NULL
 */
int queue_is_empty(const Queue* queue);

/**
 * @brief Vraća broj elemenata u redu čekanja
 * 
 * @param queue Pokazivač na red čekanja (ne smije biti NULL)
 * 
 * @return Broj elemenata, ili 0 ako je queue NULL
 */
size_t queue_size(const Queue* queue);

/**
 * @brief Ispisuje sve elemente reda čekanja
 * 
 * Koristi proslijeđenu print funkciju (ili onu iz queue_create())
 * da ispise sve elemente od početka prema kraju.
 * 
 * Ako print funkcija nije proslijeđena, ista se ne ispisuje ništa
 * osim broja elemenata.
 * 
 * @param queue Pokazivač na red čekanja (ne smije biti NULL)
 * 
 * @return STATUS_OK ako je ispis uspješan
 * @return STATUS_NULL_ARG ako je queue NULL
 */
Status queue_print(const Queue* queue);

#endif // QUEUE_H
