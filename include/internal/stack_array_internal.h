#ifndef STACK_ARRAY_INTERNAL_H
#define STACK_ARRAY_INTERNAL_H

#include "common.h"
#include <stddef.h>

/**
 * @struct StackArray
 * @brief Interna struktura za array-based stog
 *
 * Ova struktura čuva dinamički alocirani array pokazivača i trenutni kapacitet.
 */
typedef struct {
    void** data;           /**< Dinamički alocirani array pokazivača na elemente */
    size_t size;           /**< Broj elemenata trenutno u stogu */
    size_t capacity;       /**< Maksimalna veličina array-a pre resize-a */
} StackArray;

#endif // STACK_ARRAY_INTERNAL_H
