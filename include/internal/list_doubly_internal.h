#ifndef LIST_DOUBLY_INTERNAL_H
#define LIST_DOUBLY_INTERNAL_H

#include "common.h"
#include <stddef.h>

/**
 * @struct ListNodeD
 * @brief Čvor u dvostruko povezanoj listi
 *
 * Svaki čvor sadrži pokazivač na podatak i pokazivače na susjedne čvorove.
 */
typedef struct ListNodeD {
    void* data;                 /**< Pokazivač na podatak */
    struct ListNodeD* next;     /**< Pokazivač na sljedeći čvor */
    struct ListNodeD* prev;     /**< Pokazivač na prethodni čvor */
} ListNodeD;

/**
 * @struct ListDoubly
 * @brief Struktura za dvostruko povezanu listu
 *
 * Čuva pokazivače na prvi i posljednji čvor, te broj elemenata.
 */
typedef struct {
    ListNodeD* head;    /**< Pokazivač na prvi čvor (NULL ako je prazna) */
    ListNodeD* tail;    /**< Pokazivač na posljednji čvor (NULL ako je prazna) */
    size_t count;       /**< Broj elemenata u listi */
} ListDoubly;

#endif // LIST_DOUBLY_INTERNAL_H
