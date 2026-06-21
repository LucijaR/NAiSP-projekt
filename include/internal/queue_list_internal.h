#ifndef QUEUE_LIST_INTERNAL_H
#define QUEUE_LIST_INTERNAL_H

#include "common.h"
#include <stddef.h>

/**
 * @struct QueueNode
 * @brief Čvor u linked-list implementaciji reda čekanja
 */
typedef struct QueueNode {
    void* data;
    struct QueueNode* next;
} QueueNode;

/**
 * @struct QueueList
 * @brief Interna struktura za linked-list-based red
 */
typedef struct {
    QueueNode* head;
    QueueNode* tail;
    size_t count;
} QueueList;

#endif // QUEUE_LIST_INTERNAL_H
