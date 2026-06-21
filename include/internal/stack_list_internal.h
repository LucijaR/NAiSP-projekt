#ifndef STACK_LIST_INTERNAL_H
#define STACK_LIST_INTERNAL_H

#include "common.h"
#include <stddef.h>

/**
 * @struct StackNode
 * @brief Čvor u linked-list implementaciji stoga
 */
typedef struct StackNode {
    void* data;
    struct StackNode* next;
} StackNode;

/**
 * @struct StackList
 * @brief Interna struktura za linked-list-based stog
 */
typedef struct {
    StackNode* head;
    size_t size;
} StackList;

#endif // STACK_LIST_INTERNAL_H
