#ifndef QUEUE_ARRAY_INTERNAL_H
#define QUEUE_ARRAY_INTERNAL_H

#include "common.h"
#include <stddef.h>

/**
 * @struct QueueArray
 * @brief Interna struktura za circular array-based red
 *
 * Koristi kružni buffer sa front i rear indeksima.
 */
typedef struct {
    void** data;        /**< Dinamički alocirani array elemenata */
    size_t capacity;    /**< Ukupni kapacitet buffer-a */
    size_t count;       /**< Broj elemenata u redu */
    size_t front;       /**< Indeks elementa na početku */
    size_t rear;        /**< Indeks sljedeće slobodne pozicije */
} QueueArray;

#endif // QUEUE_ARRAY_INTERNAL_H
